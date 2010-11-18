/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * http://www.ingenic.cn 
 *
 ********************** END LICENSE BLOCK **************************************
 *
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn> <zzhagn@ingenic.cn> 
 *
 *  Create:   2008-10-21, by zhang zhi
 *      
 *            
 *
 *******************************************************************************
 */

#include <jz4755.h> 
#include <bsp.h>
#include <os_api.h>
#include <jztouch.h>
#include <function.h>
#include <threadprio.h>


#ifdef USE_MIDWARE
#include "midware.h"
#endif

#define BAT_TIMER	TCU_BAT_CHK_CHN
/*             ____________
VBAT----|     | SOC       |
       R1     |           |
        |_____|ADIN       |
       R2     |           |
        |     |___________|
       GND

//Note !  for example R1=1500ohm R2=4000ohm 
//To have a good resolustion ,you should define R1&R2 as 15&40  or  3&8 . 
*/
#define R1  0
#define R2  100  //kohm


#define RESOLUTION	      (R1+R2)*7500/(4096*R2)         //((R1+R2)/R2)*1000*7.5/4096  mV
#define COMPENSATION  0	//mV

#define TS_AD_COUNT             5
#define M_SADC_CFG_SNUM_5	  ADCFG_SNUM(TS_AD_COUNT - 1)

#define SADC_CFG_INIT (   \
                            ADCFG_SDELAY(3) 	| \
                            ADCFG_XYZ_X_Y_Z1_Z2	| \
							ADCFG_SNUM(5)      	| \
							ADCFG_CMD_1100  )
									    								    
static unsigned char sadc_uninit=1;									    
static unsigned char run_flag = 0;
static unsigned int ts_TimeOut = 0;
static unsigned short padkey = 0;

#define TASK_STK_SIZE	1024
static OS_TASK_STACK SADCTaskStack[TASK_STK_SIZE];
static OS_EVENT *sadcEvent;
static OS_EVENT *batteryEvent;
static OS_EVENT *adkeyEvent;
#define SADC_TASK_PRIO	SADC_THREAD_PRIO 

static PFN_SADC touchFunction = 0;
static PFN_SADC sadcFunction = 0;
static PFN_SADC batFunction = 0;
static unsigned int batCountperSec = 0;
static unsigned int sadcCountperSec = 0;
static unsigned char dCount = 0;
unsigned short tsxData = -1,tsyData = -1;
static unsigned pendown_flag = 0;

static void read_ts_data()
{
	unsigned int dat;
	unsigned short xData,yData;
	short tsz1Data,tsz2Data;
    
	
	dat = INREG32(A_SADC_ADTCH);
	
	xData = (dat >>  0) & 0xfff;
	yData = (dat >> 16) & 0xfff;
	
    //printf("%d -> %d %d\r\n",dCount,xData,yData);		
	dat = INREG32(A_SADC_ADTCH);
    tsz1Data = (dat >>  0) & 0xfff;
	tsz2Data = (dat >> 16) & 0xfff;
	//printf("%d -> %d %d\r\n",dCount,tsz1Data,tsz2Data);		
	tsz1Data = tsz2Data - tsz1Data;
	if(pendown_flag != 1)
		return ;
	
	
	if((tsz1Data > 15) || (tsz1Data < -15))
    {
		if(tsxData == (unsigned short)-1)
			tsxData = xData;
		else
			tsxData = (tsxData + xData) / 2;
		
		if(tsyData == (unsigned short)-1)
			tsyData = yData;
		else
			tsyData = (tsyData + yData) / 2;
		
    }

	//printf("0x%08x \r\n",dat);	
    dCount ++;
	
	if(dCount > TS_AD_COUNT - 1)
	{
		if(tsxData != (unsigned short) -1)
		{
			
		    if(touchFunction)
			{
				dat = tsxData + (tsyData << 16);
				touchFunction((unsigned short *)&dat);
			}
			printf("x: 0x%d y: 0x%d\r\n",tsxData,tsyData);	
			tsxData = -1;
			tsyData = -1;
		}
		
		dCount = 0;
	}
	
}
#ifdef USE_MIDWARE
unsigned short pbat = BAT_MAX_VOLT; //battery voltage
#else
unsigned short pbat= 2280; //battery voltage
#endif


static void adc_handler2()
{
	padkey = INREG16(A_SADC_ADSDAT);
	os_SemaphorePost(adkeyEvent);
}
static void bat_handler2()
{
	pbat = INREG32(A_SADC_ADBDAT);
}
static void touch_task_entry(void *arg)
{
		unsigned char state;
		unsigned char sadcstat;
		unsigned char err;
		
		while(run_flag)
		{
			os_SemaphorePend(sadcEvent,ts_TimeOut,&err);
			ts_TimeOut = 0;
			state = INREG32(A_SADC_ADSTATE);
			OUTREG32(A_SADC_ADSTATE,state);			
					
			if(state & ADSTATE_PEND)
			{
				  //printf("pen down irq \r\n");
				  SETREG32(A_SADC_ADCTRL,ADCTRL_PENDM);
				  CLRREG32(A_SADC_ADCTRL,ADCTRL_PENUM);
				  
				  pendown_flag = 1;
			}
			if(state & ADSTATE_PENU)
			{
				//printf("pen up irq \r\n");
				SETREG32(A_SADC_ADCTRL,ADCTRL_PENUM);
				CLRREG32(A_SADC_ADCTRL,ADCTRL_PENDM);
				
				pendown_flag = 0;
				tsxData = -1;
				tsyData = -1;
				if(touchFunction)
				{
					unsigned int dat;
					
					dat = tsxData + (tsyData << 16);
					touchFunction((unsigned short *)&dat);
				}
			    //printf("x: 0x%d y: 0x%d\r\n",tsxData,tsyData);	
			}
			if(state & ADSTATE_DTCH)
			{
				//printf("touch ad irq 0x%x \r\n",sadcstat);
					read_ts_data();
					
			}
			if(state & ADSTATE_DRDY)
			{
				bat_handler2();
				//printf("battery ad irq\r\n");
			}
			if(state & ADSTATE_SRDY)
			{				
				adc_handler2();				
			} 
			enable_irq(IRQ_SADC);
		}
		
}

static void handler(unsigned int arg)
{
	disable_irq(IRQ_SADC);
	os_SemaphorePost(sadcEvent);
}

static void  sadc_init(void)
{
	OUTREG32(A_SADC_ADENA,0);     
	OUTREG32(A_SADC_ADSTATE,0);
	OUTREG32(A_SADC_ADCTRL,0x1f);
			
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_SADC);
	
	OUTREG32(A_SADC_ADCFG,SADC_CFG_INIT);	     
	OUTREG32(A_SADC_ADCLK,((59<<16) |1));
	
	
	sadcEvent = os_SemaphoreCreate(0);
	run_flag = 1;
	ts_TimeOut = 0;
	os_TaskCreate(touch_task_entry, (void *)0,
		     (void *)&SADCTaskStack[TASK_STK_SIZE - 1],
		     SADC_TASK_PRIO);
		          
	request_irq(IRQ_SADC, handler, 0);
 	
}

static int sadc_deinit()
{
	OUTREG32(A_SADC_ADENA,0);
	disable_irq(IRQ_SADC);
	os_TimeDelay(20);
	run_flag = 0;
	SETREG32(A_CPM_CLKGR, CLKGR_STOP_SADC);
}
//-----------------------------------------------------------------
void ts_init(void)
{
	OUTREG32(A_SADC_ADSAME,3);
	OUTREG32(A_SADC_ADWAIT,1000);//per 100 HZ A_SADC_ADWAIT
	if(sadc_uninit)
	{
		sadc_init();
		sadc_uninit = 0;
	}
	CLRREG32(A_SADC_ADCTRL,(ADCTRL_PENDM | ADCTRL_DTCHM));
	SETREG32(A_SADC_ADENA,ADENA_TCHEN); 
}
//-----------------------------------------------------------------
void adkey_init(void)
{
	adkeyEvent = os_SemaphoreCreate(0);
	if(sadc_uninit)
	{
		sadc_init();
		sadc_uninit = 0;
	}
	CLRREG32(A_SADC_ADCTRL,ADCTRL_SRDYM);
}

int read_adkey(void)
{
	unsigned char err;

	if((INREG8(A_SADC_ADSTATE) & ADSTATE_SRDY) ==0)
	{
		SETREG8(A_SADC_ADENA, ADENA_SADCINEN);	
	}
	os_SemaphorePend(adkeyEvent,0,&err);

	return padkey;
}

//-----------------------------------------------------------------

  
int battery_init()
{
	CLRREG32(A_SADC_ADCFG,ADCFG_BAT_MD);	 // full baterry voltage >= 2.5V  
//	SETREG32(A_SADC_ADCFG,ADCFG_BAT_MD);	 // full baterry voltage < 2.5V
	if(sadc_uninit)
	{
		sadc_init();
		sadc_uninit = 0;
	}
	CLRREG32(A_SADC_ADCTRL,ADCTRL_DRDYM);
	SETREG32(A_SADC_ADENA,ADENA_PBATEN);	
}
static int CovertBatteryToVoltage(int bat)
{
#if 0 //xltao modify
	return bat*RESOLUTION+COMPENSATION;
#else
	return 2*(bat-1900) + 3500; //xltao add
#endif
}   
int read_battery(void)
{
	unsigned char err;

	if ((INREG32(A_SADC_ADSTATE) & ADSTATE_DRDY) == 0 )      
	{
		SETREG32(A_SADC_ADENA,ADENA_PBATEN);	//Enable pbat irq 
	}

	int ret = CovertBatteryToVoltage(pbat);

	printf("bat = %d\n", ret);
	//2010.02.11 danny added for power led control
#ifdef GPIO_POWER_LED_PIN
	if(ret < 3400)
		gpio_set_dir(GPIO_POWER_LED_PIN, 1, GPIO_POWER_LED_LOW);
	else
		gpio_set_dir(GPIO_POWER_LED_PIN, 1, GPIO_POWER_LED_NORMAL);
#endif
	return ret;
}

//-----------------------------------------------------------------
#ifdef USE_MIDWARE //tcu 2
volatile static MIDSRC rtcsrc;
static unsigned int  rtcid;
#define RTC_CLK					32768
void BAT_StopTimer(void)
{
	//printf("JZ_StopTimer2\n");
	disable_irq(EIRQ_TCU2_BASE + BAT_TIMER);
	OUTREG16(A_TCU_TECR, TCU_TIMER(BAT_TIMER));
}

static void GetRequest(MIDSRCDTA *dat)
{
	printf("TCU getrequest\n");
}

static void Response(MIDSRCDTA *dat)
{
	printf("dat->Val == %d\n",dat->Val);

}

static void info_set_timer()
{
	//printf("Send info set tcu battery timer!\n");
	rtcsrc.Src = TCU_SET_TIMER;
	rtcsrc.Event =  EVENT_SET_TIMER;
	os_QueuePost(rtcsrc.CurEvent1 , (void *)&rtcid);
	os_SemaphorePost(rtcsrc.CurEvent);
}

static void tcu_timer_handler(unsigned int arg)
{		
	//printf("tcu_timer_handler:arg = %d!!\n",arg);
	info_set_timer();
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(BAT_TIMER));
}

void tcu_timer_set(unsigned int d, unsigned int tps)
{
	unsigned int latch;
	latch = (tps * RTC_CLK / 1024);

	enable_irq(EIRQ_TCU2_BASE + BAT_TIMER);
	
	OUTREG16(A_TCU_TDFR(BAT_TIMER), latch);
	OUTREG16(A_TCU_TDHR(BAT_TIMER), latch);
	OUTREG16(A_TCU_TCNT(BAT_TIMER), 0x0000);
	
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(BAT_TIMER));

}

#define BATID (('B'<<24)|('A'<<16)|('A'<<8)|(':'<<0))
void JZ_Init_Battery_Timer()
{
#ifdef USE_MIDWARE
	rtcsrc.GetRequest = GetRequest;
	rtcsrc.Response = Response;
	rtcsrc.Name = "TCU";
	RegisterMidSrc((PMIDSRC)&rtcsrc);
#endif
	rtcid = rtcsrc.ID;
	
	//1.init controller
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);
	OUTREG16(A_TCU_TECR, TCU_TIMER(BAT_TIMER));
	//2.config clk and clear flag
	OUTREG16(A_TCU_TCSR(BAT_TIMER), TCU_CLK_RTC | TCU_CLK_PRESCALE_CLK1024);	
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(BAT_TIMER));
	//3.regist and enable irq
	request_irq(EIRQ_TCU2_BASE + BAT_TIMER, tcu_timer_handler, 0);
	
	PM_AddPowerCtrl(BATID,BAT_StopTimer,0);
	
}

void JZ_Start_Battery_Timer()
{
	enable_irq(EIRQ_TCU2_BASE + BAT_TIMER);
	OUTREG16(A_TCU_TESR, 1<<BAT_TIMER);
}


#endif
