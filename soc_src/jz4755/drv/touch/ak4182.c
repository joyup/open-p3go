/********************** BEGIN LICENSE BLOCK **********************************
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
 ********************** END LICENSE BLOCK ************************************
 *
 *  Author:  <zzhang@ingenic.cn>
 *
 *  Create:   2008-11-7, by zhang zhi 
 *             
 *  Maintain: 2008-12-03, by Rocky Xu
 *            
 *
 *****************************************************************************
 */
#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>
#include <threadprio.h>

#ifdef USE_MIDWARE
	#include "midware.h"
#endif

#include <jztouch.h>
#include <function.h>

//----------------------------------------------------------------------------

int battery_init(void)
{
	printf("+battery_init\n");

	// Init SSI0 GPIO.
	OUTREG32(A_GPIO_PXFUNS(1), 0xFC000000);
	OUTREG32(A_GPIO_PXSELC(1), 0xFC000000);
	// Init SSI0 CLK
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_SSI0);
	OUTREG32(A_CPM_SSICDR, 5);
	SETREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);
	CLRREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);
	OUTREG16(A_SSI0_GR, 0x00F0);

	// Init SSI REG CFG
	OUTREG16(A_SSI0_CR0, 0x8006);
	OUTREG32(A_SSI0_CR1, 0x12078A0);
	OUTREG16(A_SSI0_ITR, 0x0001);
	OUTREG8(A_SSI0_ICR, 0x00);

	// send command
	OUTREG32(A_SSI0_DR, 0x000000A7);
}

//----------------------------------------------------------------------------

void BAT_StopTimer(void)
{
	disable_irq(EIRQ_TCU2_BASE + TCU_BAT_CHK_CHN);
	OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_BAT_CHK_CHN));
}

//----------------------------------------------------------------------------

#ifdef USE_MIDWARE

volatile static MIDSRC adcsrc;
static unsigned int adcid;

static void GetRequest(MIDSRCDTA *dat)
{
	printf("TCU get request\n");
}

//----------------------------------------------------------------------------

static void Response(MIDSRCDTA *dat)
{
	printf("dat->Val == %d\n", dat->Val);
}

//----------------------------------------------------------------------------

static void info_set_timer(void)
{
	adcsrc.Src = TCU_SET_TIMER;
	adcsrc.Event = EVENT_SET_TIMER;
	os_QueuePost(adcsrc.CurEvent1, (void *)&adcid);
	os_SemaphorePost(adcsrc.CurEvent);
}

//----------------------------------------------------------------------------

static void tcu_timer_handler(unsigned int arg)
{		
	info_set_timer();
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_BAT_CHK_CHN));
}

//----------------------------------------------------------------------------

void tcu_timer_set(unsigned int rsv, unsigned int secs)
{
	unsigned int ticks;

	ticks = secs * (32768 / 1024);
	if (ticks > 0xFFFF)
		ticks = 0xFFFF;

	enable_irq(EIRQ_TCU2_BASE + TCU_BAT_CHK_CHN);

	OUTREG16(A_TCU_TDFR(TCU_BAT_CHK_CHN), ticks);
	OUTREG16(A_TCU_TDHR(TCU_BAT_CHK_CHN), ticks);
	OUTREG16(A_TCU_TCNT(TCU_BAT_CHK_CHN), 0x0000);

	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_BAT_CHK_CHN));
}

//----------------------------------------------------------------------------

#define BATID (('B'<<24)|('A'<<16)|('A'<<8)|(':'<<0))
void JZ_Init_Battery_Timer(void)
{
	adcsrc.GetRequest = GetRequest;
	adcsrc.Response = Response;
	adcsrc.Name = "TCU";
	RegisterMidSrc((PMIDSRC)&adcsrc);
	adcid = adcsrc.ID;

	//1.init controller
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TCU);
	OUTREG16(A_TCU_TECR, TCU_TIMER(TCU_BAT_CHK_CHN));
	//2.config clk and clear flag
	OUTREG16(A_TCU_TCSR(TCU_BAT_CHK_CHN), TCU_CLK_RTC | TCU_CLK_PRESCALE_CLK1024);	
	OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_BAT_CHK_CHN));
	//3.regist and enable irq
	request_irq(EIRQ_TCU2_BASE + TCU_BAT_CHK_CHN, tcu_timer_handler, 0);
	
	PM_AddPowerCtrl(BATID,BAT_StopTimer,0);
}

//----------------------------------------------------------------------------

void JZ_Start_Battery_Timer(void)
{
	enable_irq(EIRQ_TCU2_BASE + TCU_BAT_CHK_CHN);
	OUTREG16(A_TCU_TESR, 1 << TCU_BAT_CHK_CHN);
}

//----------------------------------------------------------------------------

#define	AK4182_ADC_RESOLUTION		3

int read_battery(void)
{
	int timeout = 10000;

#ifdef USE_MIDWARE
	while ( (!(INREG32(A_SSI0_SR) & (1 << 7))) && (timeout--));
	if (timeout < 0)
	{
		printf("-read_battery timeout\n");
		OUTREG32(A_SSI0_DR, 0x000000A7);
		return (BAT_MAX_VOLT);
	}
	OUTREG32(A_SSI0_DR, 0x000000A7);

	return (INREG32(A_SSI0_DR) * AK4182_ADC_RESOLUTION);
#else
	return (4200);
#endif
}

//----------------------------------------------------------------------------

#endif
