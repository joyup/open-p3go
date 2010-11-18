/********************** BEGIN LICENSE BLOCK **********************************
 *
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co., Ltd. 2008. All rights reserved.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * http://www.ingenic.cn
 *
 ********************** END LICENSE BLOCK ************************************
 *
 *  Author:  <dsqiu@ingenic.cn>  <zzhang@ingenic.cn>
 *
 *  Create:   2008-10-11, by zhang zhi
 *
 *  Maintain: 2008-10-11, by whxu
 *
 *  Maintain: 2008-11-29, by whxu, change the pwm function
 *
 *  Maintain: 2008-12-04, by whxu, change the JZ_StartTimer nad JZ_StopTimer
 *
 ****************************************************************************/

#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>

//----------------------------------------------------------------------------

void hw_reset(void)
{
	printf("hw_reset\n");
	OUTREG16(A_WDT_TCSR, WDT_CLK_EXTAL);
	SETREG16(A_WDT_TCSR, WDT_CLK_PRESCALE_CLK1024);
	OUTREG16(A_WDT_TDR, 3);
	OUTREG16(A_WDT_TCNT, 0);
	SETREG8(A_WDT_TCER, WDT_ENABLE);
	while (1);
}

//----------------------------------------------------------------------------

void JZ_StopTicker(void)
{
	printf("WARMING: System Timer will STOP!!!\n");

	OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(TCU_SYSTEM_TIMER_CHN));
	OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_SYSTEM_TIMER_CHN));
}

//----------------------------------------------------------------------------

void JZ_StartTicker(unsigned int TicksPerSec)
{
	unsigned int match_counter;

	// 1. Start TCU Clock
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);

	// 2. Disable TCU.
	OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_SYSTEM_TIMER_CHN));

	// 3. Config Clock
	OUTREG16(A_TCU_TCSR(TCU_SYSTEM_TIMER_CHN), TCU_CLK_EXTAL | TCU_CLK_PRESCALE_CLK64);

	// 4. Config Counter According to Clock
	match_counter = (OSC_CLOCK / 64) / TicksPerSec;
	OUTREG16(A_TCU_TDFR(TCU_SYSTEM_TIMER_CHN), match_counter);
	OUTREG16(A_TCU_TDHR(TCU_SYSTEM_TIMER_CHN), match_counter);
	OUTREG16(A_TCU_TCNT(TCU_SYSTEM_TIMER_CHN), 0x0000);

	// 5. Clear TCU Intr Flag and Enable Intr
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_SYSTEM_TIMER_CHN));
	OUTREG32(A_TCU_TMCR, TCU_INTR_FLAG(TCU_SYSTEM_TIMER_CHN));

	// 6. Enable TCU_SYSTEM_TIMER_CHN TCU
	OUTREG16(A_TCU_TESR, TCU_TIMER(TCU_SYSTEM_TIMER_CHN));

	// 7. Enable IRQ
	enable_irq(IRQ_SYSTEM_TIMER);
}

//----------------------------------------------------------------------------

void Stop_PerformanceCounter(void)
{
	printf("WARMING: Performance Counter will STOP!!!\n");

	OUTREG32(A_TCU_TMSR, TCU_OSTMST);
	OUTREG16(A_TCU_TECR, TCU_OSTCL);
}

//----------------------------------------------------------------------------

int Init_PerformanceCounter(void)
{
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);
	OUTREG16(A_TCU_TECR, TCU_OSTCL);

	OUTREG32(A_OST_CSR, TCU_CLK_EXTAL | TCU_CLK_PRESCALE_CLK4);
	OUTREG32(A_OST_DR, 0xFFFFFFFF);
	OUTREG32(A_OST_CNT, 0);

	SETREG32(A_TCU_TESR, TCU_OSTEN);

	return ((OSC_CLOCK / 4 ) / 1000000);
}

//----------------------------------------------------------------------------

unsigned int Get_PerformanceCounter(void)
{
     return (INREG32(A_OST_CNT));
}

//----------------------------------------------------------------------------

void JZ_StopTimer(void)
{
	Stop_PerformanceCounter();
}

//----------------------------------------------------------------------------

void JZ_StopTimerEx(int chn)
{
	OUTREG16(A_TCU_TECR, TCU_TIMER(chn));
	OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(chn));
}

//----------------------------------------------------------------------------

void JZ_StartTimer(void)
{
	Init_PerformanceCounter();
}

//----------------------------------------------------------------------------

void JZ_StartTimerEx(int chn, int frequence, void (*handler)())
{
	unsigned int match_counter;

	if ( (frequence<((OSC_CLOCK / 4) / 0xffff)+1) || (frequence>(OSC_CLOCK / 400)) )
	{
		printf("The frequence of timer shoud be in [%d,%d]\n",
		(((OSC_CLOCK / 4) / 0xffff)+1),((OSC_CLOCK / 400)));
		return ;
	}

	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);
	OUTREG16(A_TCU_TECR, TCU_TIMER(chn));

	OUTREG16(A_TCU_TCSR(chn), TCU_CLK_EXTAL | TCU_CLK_PRESCALE_CLK4);
	match_counter = (OSC_CLOCK / 4) / frequence;
	OUTREG16(A_TCU_TDFR(chn), match_counter);
	OUTREG16(A_TCU_TDHR(chn), match_counter);
	OUTREG16(A_TCU_TCNT(chn), 0x0000);

	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(chn));
	OUTREG32(A_TCU_TMCR, TCU_INTR_FLAG(chn));

	request_irq(EIRQ_TCU2_BASE + chn, handler, 0);
	OUTREG16(A_TCU_TESR, TCU_TIMER(chn));
	enable_irq(EIRQ_TCU2_BASE + chn);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//Note:
//    	frequence=[(OSC_CLOCK / 4) / 0xffff+1,((OSC_CLOCK / 4) / 100)]
//    	IF OSC_CLOCK=12,000,000  THEN  frequence=[190,120000]
//      mode: 	bit0---InitLevel  0--LowLevel  1--HightLevel
//				bit1---ShutDown   0--Gracefull 1--Abrutp
void pwm_init(unsigned int chn, unsigned int frequence, unsigned int mode)
{
	unsigned int full_counter;

	if ((frequence < ((OSC_CLOCK) / 0xffff) + 1) || (frequence > (OSC_CLOCK/100)))
	{
		printf("The frequence of pwm shoud be in [%d,%d]\n",
				(((OSC_CLOCK) / 0xffff)+1), ((OSC_CLOCK/100)) );
		return;
	}

	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);
	OUTREG16(A_TCU_TECR, TCU_TIMER(chn));

	OUTREG32(A_GPIO_PXFUNS(4), 1 << (20 + chn));
	OUTREG32(A_GPIO_PXSELC(4), 1 << (20 + chn));
	OUTREG32(A_GPIO_PXPES(4), 1 << (20 + chn));

	OUTREG16(A_TCU_TCSR(chn), TCU_CLK_EXTAL);
	full_counter = (OSC_CLOCK) / frequence;

	OUTREG16(A_TCU_TDFR(chn), full_counter);
	OUTREG16(A_TCU_TDHR(chn), 0x0000);
	OUTREG16(A_TCU_TCNT(chn), 0x0000);

	OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(chn));
	SETREG32((A_TCU_TCSR(chn)), (mode & 0x3) << 8);
	SETREG32((A_TCU_TCSR(chn)), TCSR_PWM_EN);
}

//----------------------------------------------------------------------------

void pwm_set_duty(unsigned int chn, unsigned int percent)
{
	unsigned int full_cycles;
	static unsigned int old_percent=1;

	if (percent == 108)
	{
		OUTREG16(A_TCU_TESR, TCU_TIMER(chn));
		SETREG32((A_TCU_TCSR(chn)), TCSR_PWM_EN);
		OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(chn));
		return;
	}

	if (percent > 100)
	{
		printf("The percent of PWM should be within 100\n");
		return;
	}

	if (percent == 0)
	{
		
		CLRREG32((A_TCU_TCSR(chn)), TCSR_PWM_EN);
		OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(chn));
		OUTREG16(A_TCU_TECR, TCU_TIMER(chn));
		if(old_percent!=0)
		{ 
			__gpio_as_output(4*32+20+chn);
			__gpio_clear_pin(4*32+20+chn);
		}
		old_percent=0;
	}
	else
	{
		if(old_percent==0)
		{		
			OUTREG32(A_GPIO_PXFUNS(4), 1 << (20 + chn));
			OUTREG32(A_GPIO_PXSELC(4), 1 << (20 + chn));
			OUTREG32(A_GPIO_PXPES(4), 1 << (20 + chn));
		}
		full_cycles = INREG16(A_TCU_TDFR(chn));
		CLRREG32((A_TCU_TCSR(chn)), TCSR_PWM_EN);
		OUTREG16(A_TCU_TDHR(chn), full_cycles - (full_cycles * percent) / 110);
		OUTREG16(A_TCU_TCNT(chn), 0x0000);
		OUTREG16(A_TCU_TESR, TCU_TIMER(chn));
		SETREG32((A_TCU_TCSR(chn)), TCSR_PWM_EN);
		old_percent=percent;

	}
}

//----------------------------------------------------------------------------

#define TCU_IDLE_TIMER_CHN 3
static volatile char IdleTimer_Flag = 0;
static void idle_timerhandle(unsigned int arg)
{
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
	JZ_StopTimerEx(TCU_IDLE_TIMER_CHN);
	IdleTimer_Flag=0;
	free_irq(EIRQ_TCU2_BASE + TCU_IDLE_TIMER_CHN);
}

void JZ_InitIdleTimer(void (*handler)(unsigned int))
{
	while(IdleTimer_Flag); 
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);
	OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
	OUTREG16(A_TCU_TCSR(TCU_IDLE_TIMER_CHN), TCU_CLK_EXTAL | TCU_CLK_PRESCALE_CLK256);
	OUTREG16(A_TCU_TDFR(TCU_IDLE_TIMER_CHN), 0xffff);
	OUTREG16(A_TCU_TDHR(TCU_IDLE_TIMER_CHN), 0xffff);
	OUTREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN), 0x0000);
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
	OUTREG32(A_TCU_TMCR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
	request_irq(EIRQ_TCU2_BASE + TCU_IDLE_TIMER_CHN, handler, 0);
}
//----------------------------------------------------------------------------
// aboute precision:  24000/256=93.75  (94-93.75)/93.75=0.00266  (93.75-93)/93.75=0.008

void JZ_StartIdleTimer(int time, void (*handler)(unsigned int))
{
	unsigned int match_counter;
	if ( time>(65535/(OSC_CLOCK / 256000)) )
	{
		printf("The msec for idletimer shoud be in [0,%d]\n",65535/(OSC_CLOCK /256000));
		return ;
	}
	JZ_InitIdleTimer(handler);
	match_counter = ((OSC_CLOCK /1000) * time) / 256;
	OUTREG16(A_TCU_TDFR(TCU_IDLE_TIMER_CHN), match_counter);
	OUTREG16(A_TCU_TESR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
}
#define MSEC_PER_TICK (1000 / OS_TICKS_PER_SEC) 
static void AssistTimerHandler(unsigned int arg)
{
	unsigned int tcount = INREG16(A_TCU_TDFR(TCU_IDLE_TIMER_CHN));// + INREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN)); //INREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN))
	unsigned int count = tcount; 
	//printf("b:count = %d\n",count);
	unsigned int ticks = (count * (256000 / MSEC_PER_TICK) + OSC_CLOCK - 1) / OSC_CLOCK;
	//printf("b:ticks = %d\n",ticks);
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
	ticks = OSTimeTicks(ticks);
	if(ticks)
	{
		//printf("continue time %d\n",ticks);
		if(ticks >= 60) ticks = 60;
		count = ((OSC_CLOCK / 1000 * MSEC_PER_TICK) * ticks + 255) / 256;
		OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
			
		if(count < tcount)
		{
			//OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));	
				
				OUTREG16(A_TCU_TDFR(TCU_IDLE_TIMER_CHN), count);
		
			OUTREG16(A_TCU_TESR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
		}else
		{
			OUTREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN), 0x0000);
			OUTREG16(A_TCU_TESR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
			OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
		}
		//OUTREG32(A_TCU_TMCR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
		
	}else
	{
		os_SystemTick();
		//printf("stop time\n");
		OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
		OUTREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN), 0x0000);
		OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
	}
	
}
//----------------------------------------------------------------------------
int osIdleDelay(unsigned short ticks)
{
	unsigned int savesr;
	int count;
	int cur_ticks = 0;
	if(ticks == 0) ticks = 1;
	if(ticks > 60) return 0;
	savesr = spin_lock_irqsave();
	//printf("set ticks = %d\n",ticks);
	//printf("a:%x\n",INREG16(A_TCU_TER) & TCU_TIMER(TCU_IDLE_TIMER_CHN));	
	if(INREG16(A_TCU_TER) & TCU_TIMER(TCU_IDLE_TIMER_CHN))
	{
		OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
		OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
		int fullcount = INREG16(A_TCU_TDFR(TCU_IDLE_TIMER_CHN));
		if(!(TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN) & INREG16(A_TCU_TFR)))
		{
				//printf("aa\n");
				count = INREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN));
				if(count < fullcount - 10)
					OUTREG16(A_TCU_TDFR(TCU_IDLE_TIMER_CHN), count + 10);
		}//else printf("bb\n");
	}else
	{
		
		cur_ticks = ticks;
		if(cur_ticks >= 60) cur_ticks = 60;
		//printf("a:cur_ticks %x\n",cur_ticks);
		count = ((OSC_CLOCK / 1000 * MSEC_PER_TICK) * cur_ticks + 255) / 256;
		//printf("a:%x\n",count);
		OUTREG16(A_TCU_TDFR(TCU_IDLE_TIMER_CHN), count);	
		

	}
	OUTREG16(A_TCU_TESR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
	OUTREG32(A_TCU_TMCR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
	
	spin_unlock_irqrestore(savesr);
	return 0;
}
//----------------------------------------------------------------------------
void InitAssistTimer()
{
	JZ_InitIdleTimer(AssistTimerHandler);
}
//----------------------------------------------------------------------------
void StopAssistTimer()
{
		OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
		OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
		OUTREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN), 0x0000);	
		free_irq(EIRQ_TCU2_BASE + TCU_IDLE_TIMER_CHN);
}
//----------------------------------------------------------------------------

void idledelay(unsigned int msec)
{
	JZ_StartIdleTimer(msec, idle_timerhandle);
	IdleTimer_Flag=1;
	while(IdleTimer_Flag )
	{
		os_Schedule();
		jz_pm_idle();
	}
}
//----------------------------------------------------------------------------


#if 0
//idledelay2------------------------------------------------------------------
#define NUM  4
int g_counter=0;
int *counter[NUM]={0};

//----------------------------------------------------------------------------
static void idle_timerhandle2()
{
	int i,t,t2=1000;
	JZ_StopTimerEx(TCU_IDLE_TIMER_CHN);	
	for(i=0;i<NUM;i++)
	if(counter[i]!=0)
	{
		t=*counter[i];
		t-=g_counter;
		if (t==0)
			counter[i]=0;
		else 
			t2 = MIN(t,t2);
	}
	if (t2>0)
	{
		JZ_StartIdleTimer(t2, idle_timerhandle2);
		g_counter=t2;
	}
	else
		free_irq(EIRQ_TCU2_BASE + TCU_IDLE_TIMER_CHN);
}
//----------------------------------------------------------------------------
static void delayupdate(unsigned int *addr)
{	int rest,t,i,flag=0,counter2 = 0;
	//JZ_StopTimerEx(TCU_IDLE_TIMER_CHN);
	OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(TCU_IDLE_TIMER_CHN));
	OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_IDLE_TIMER_CHN));
	rest = INREG16(A_TCU_TCNT(TCU_IDLE_TIMER_CHN))/(OSC_CLOCK / 256000);
	if(g_counter>0)
	{
		counter2 = g_counter-rest ;
		g_counter = MIN(rest,*addr);
	}
	else
		g_counter=*addr;
	for(i=0;i<NUM;i++)
	{
		if(( counter[i]== 0 ) && ( flag== 1 ))
		{ 
			counter[i]=addr;
			flag = 0 ;
		}
		else
		{
			t = *counter[i];
			t-= counter2;
			*counter[i] = MAX(0,t);
		}
	}
	JZ_StartIdleTimer(g_counter, idle_timerhandle2);
}
//--------------------------------------------------------------
void idledelay2( unsigned int msec)
{	
    unsigned int idle_time;
    unsigned int *p_idle_time;
    idle_time = msec;
    p_idle_time=&idle_time;
    delayupdate(p_idle_time);
    while(*p_idle_time)
    {
    	os_Schedule();
    	jz_pm_idle();
    }
}
#endif
