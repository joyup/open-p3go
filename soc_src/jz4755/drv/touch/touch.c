/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2008. All rights reserved.
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
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn> 
 *
 *  Create:   2008-06-26, by dsqiu
 *            
 *  Maintain: 2008-06-26, by jgao
 *            
 *
 *******************************************************************************
 */

#include <jztouch.h>

#if (TOUCH == 1)
	#include <sadc.c>
#elif (TOUCH == 2)
	#include <ak4182.c>
#endif

/*
#define TS_PIN  (98)
#define TS_IRQ  (EIRQ_GPIO_BASE + TS_PIN)
#define TOUCH_TASK_PRIO	3
#define TASK_STK_SIZE	1024
#define ts_TimeOut  100



//--------------------------------------------------------
				ak4182 command
//--------------------------------------------------------
#define TOUCH_PD_MODE   2
#define ADC_PD_MODE     3
#define ADC_STARTBITNUM (1 << 7)  
#define ADC_CHANNEL(x) (x << 4)
#define ADC_MODE(x)    (x << 3)
#define ADC_SER(x)     (x << 2)
#define ADC_PD(y)      (y)
#define ADC_COMMAND(x,y,m) (ADC_STARTBITNUM | \
			    ADC_CHANNEL(x)  | \
			    ADC_MODE(0)     | \
			    ADC_SER(m)      | \
			    ADC_PD(y))

#define X ADC_COMMAND(5,2,0)
#define Y ADC_COMMAND(1,2,0)
#define BAT ADC_COMMAND(2,3,1)
#define INIT ADC_COMMAND(0,0,1)

static OS_EVENT * Ak4182Event;
static OS_EVENT * ssiEvent;

int update;


#define ak4182_reg_write(val)	OUTREG32(A_SSI0_DR,val)


#define ak4148_ssi_irq_set() \
do{                          \
	CLRREG32(A_SSI0_CR1,SSI_RTRG_MASK);		 \
	SETREG32(A_SSI0_CR1,SSI0_RTRG_1);	 \
	CLRREG16(A_SSI0_CR0,SSI_TIE | SSI_TEIE);		 \
	SETREG16(A_SSI0_CR0,SSI_RIE | SSI_REIE);	 \
}while(0);

#define ak4182_reg_read() 	INREG32(A_SSI0_DR)

#define ak4182_ssi_enable() SETREG16(A_SSI0_CR0,SSI_SSIE)
#define ak4182_ssi_disable() CLRREG16(A_SSI0_CR0,SSI_SSIE)
static ak4182_ssi_set_trans_mode_format() 
{                                        
	CLRREG32(A_SSI0_CR1,SSI_FMAT_MASK);		
	SETREG32(A_SSI0_CR1,SSI_FMAT_MW1);		
	CLRREG32(A_SSI0_CR1,SSI_TFVCK_MASK|SSI_TCKFI_MASK);		 
	SETREG32(A_SSI0_CR1,SSI_TFVCK_3|SSI_TCKFI_3);	 
	CLRREG16(A_SSI0_CR0,SSI_RFINE); 	
	CLRREG32(A_SSI0_CR1,SSI_LFST); 		
	CLRREG32(A_SSI0_CR1,SSI_MCOM_MASK);		 
	SETREG32(A_SSI0_CR1,SSI_MCOM_8); 		
	CLRREG32(A_SSI0_CR1,SSI_FLEN_MASK);		
	SETREG32(A_SSI0_CR1,SSI_FLEN_12); 
	OUTREG16(A_SSI0_ITR, 0x0001);

	printf("cr0 is %x\n",INREG16(A_SSI0_CR0));	
	printf("cr0 is %x\n",INREG32(A_SSI0_CR1));
	printf("cr0 is %x\n",INREG16(A_SSI0_ITR));
	printf("cr0 is %x\n",INREG8(A_SSI0_ICR));

			
}
static  void ak4182_ssi_reset()           
{				   
	OUTREG16(A_SSI0_CR0, 0x0000);        
	OUTREG32(A_SSI0_CR1, 0x00007960);    
	OUTREG32(A_SSI0_SR, 0x00000098);    
	OUTREG16(A_SSI0_ITR, 0x0000);        
	OUTREG8(A_SSI0_ICR, 0x00);          
	OUTREG16(A_SSI0_GR, 0x0000);        
    SETREG16(A_SSI0_CR0,SSI_TFLUSH);	
    SETREG16(A_SSI0_CR0,SSI_RFLUSH);	
}

static void ak4182_ssi_set_clk_div_ratio()	
{  
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_SSI0);
	OUTREG32(A_CPM_SSICDR, 5); 
	OUTREG16(A_SSI0_GR, 0x00F0);            
	SETREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);
	CLRREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);
	
}   
    
static void ak4182_handler(unsigned int arg)
{
	disable_irq(TS_IRQ);
	os_SemaphorePost(Ak4182Event);
}

static void ssi_handler(unsigned int arg)
{
	disable_irq(IRQ_SSI0);
	os_SemaphorePost(ssiEvent);
}

static int read_data(unsigned int command)
{
	int temp;
	unsigned char err;
	ak4182_reg_write(command);
	os_SemaphorePend(ssiEvent,ts_TimeOut,&err);
	temp=ak4182_reg_read();

	//clearbuffer();
	enable_irq(IRQ_SSI0);
	return temp;
}

static int data_bat;
static int data_x;
static int data_y;


 int read_bat()
{   static int data=-1;
	data_bat=read_data(BAT);
	printf("\n battery valtage value is %x \n", data_bat);
	return data_bat;
}
static void read_tsp()
{
	data_x=read_data(X);
	data_y=read_data(Y);

	if (__gpio_get_pin(TS_PIN) == 0)
		update=1;//to make sure every value is sampled when pin is down.
	else
		update=0;
}
static char touch=0;
static char battery=0;
static void ak4182_task()
{
	unsigned char err;
	while(1)
	{
		os_SemaphorePend(Ak4182Event,ts_TimeOut,&err);
		if (touch == 1)
			{
				read_tsp();
				touch=0;
				enable_irq(TS_IRQ);
			}
		if (battery == 1)
			{
				read_bat();
				battery=0;
			}		
	}
}
#define TASK_STK_SIZE	1024
static OS_TASK_STACK touchTaskStack[TASK_STK_SIZE];

int TS_init(void)
{
	Ak4182Event = os_SemaphoreCreate(0);
	ssiEvent = os_SemaphoreCreate(0);
	
	//ssi_init
	__gpio_as_ssi0(GPIO_SSI0);
	ak4182_ssi_set_clk_div_ratio();//DCLK is 125K Hz max
	ak4182_ssi_reset();
	ak4182_ssi_set_trans_mode_format();
	ak4148_ssi_irq_set();
	ak4182_ssi_enable();
	
	os_TaskCreate(ak4182_task, (void *)0,
		     (void *)&touchTaskStack[TASK_STK_SIZE - 1],
		     TOUCH_TASK_PRIO);

	request_irq(IRQ_SSI0, ssi_handler, 0);
	
	 //touch_pen_init
	disable_irq(TS_IRQ);
	__gpio_as_irq_fall_edge(TS_PIN);		     
	__gpio_ack_irq(TS_PIN); 
	request_irq(TS_IRQ, ak4182_handler,0);	
	
//	ak4182_reg_write(ADC_COMMAND(0,0,1));
//	ak4182_reg_write(INIT);
	printf("AK4182 touch screen driver initialized\n");
	return 0;
}
*/
