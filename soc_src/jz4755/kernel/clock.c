/********************** BEGIN LICENSE BLOCK **********************************
 *
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright Ingenic Semiconductor Co., Ltd. 2008. All rights reserved.
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
 *  Create:   2008-10-11, by zzhang
 *
 *  Maintain:   2008-10-13, by whxu
 *
 ****************************************************************************/
 
#include <bsp.h>
#include <jz4755.h>

//----------------------------------------------------------------------------

int get_sdram_configure(unsigned int pllin, unsigned int *sdram_freq)
{
	register unsigned int ps, dmcr = 0, tmp;
 
	ps = 1000000/(pllin / 1000000);
	tmp = (SDRAM_TRAS * 1000)/ps;
	if (tmp < 4) tmp = 4;
	if (tmp > 11) tmp = 11;
	dmcr |= ((tmp-4) << DMCR_TRAS_BIT);

	tmp = (SDRAM_RCD * 1000)/ps;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << DMCR_RCD_BIT);

	tmp = (SDRAM_TPC * 1000)/ps;
	if (tmp > 7) tmp = 7;
	dmcr |= (tmp << DMCR_TPC_BIT);

	tmp = (SDRAM_TRWL * 1000)/ps;
	if (tmp > 3) tmp = 3;
	dmcr |= (tmp << DMCR_TRWL_BIT);

	tmp = ((SDRAM_TRAS + SDRAM_TPC) * 1000)/ps;
	if (tmp > 14) tmp = 14;
	dmcr |= (((tmp + 1) >> 1) << DMCR_TRC_BIT);

	/* Set refresh registers */
	tmp = (SDRAM_TREF * 1000)/ps;
	tmp = tmp/64 + 1;
	if (tmp > 0xff) tmp = 0xff;
        *sdram_freq = tmp; 

	return dmcr;
}

void sys_pll_init(unsigned int reserved)
{
	unsigned int sdramclock = 0;

	OUTREG32(A_CPM_CPCCR, CPCCR_CFG);
	OUTREG32(A_CPM_CPPCR, CPPCR_CFG);

	get_sdram_configure(CFG_CPU_SPEED, &sdramclock);
	if (sdramclock > 0)
	{
		OUTREG16(A_EMC_RTCOR, sdramclock);
		OUTREG16(A_EMC_RTCNT, sdramclock);

  		// Config SDRAM to auto Power Down Mode
		//SETREG32(A_EMC_DMCR, DMCR_POWER_DOWN);	
	}
	else
	{
		while(1);
	}
}

//----------------------------------------------------------------------------

unsigned int CalcutatePLLClock(unsigned int cppcr,bool  bCheckPCSBit)
{
	unsigned int  m, n, od, pll, pcs;

	if ((cppcr & CPPCR_PLLEN) && !(cppcr & CPPCR_PLLBP))
	{
		m = ((cppcr & CPPCR_PLLM_MASK) >> CPPCR_PLLM_BIT) + 2;
		n = ((cppcr & CPPCR_PLLN_MASK) >> CPPCR_PLLN_BIT) + 2;
		od = ((cppcr & CPPCR_PLLOD_MASK) >> CPPCR_PLLOD_BIT) + 1;
		od = (od == 3) ? (od - 1) : od;
		pll = OSC_CLOCK * m / (n * od);
	}
	else
		pll = OSC_CLOCK;

	// Get PCS Bit for MSC, I2S, LCD and USB if need.
	if (bCheckPCSBit)
	{
		pcs = INREG32(A_CPM_CPCCR) & CPCCR_PLLCLK_DIV2;
		if (pcs == 0)
			pll >>= 1;
	}

	return (pll);
}

unsigned int GetCurrentPLLClock(bool bCheckPCSBit)
{
	return CalcutatePLLClock(INREG32(A_CPM_CPPCR), bCheckPCSBit);
}

//----------------------------------------------------------------------------
static unsigned int div_matrix[] = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

unsigned int GetCommonClock(CLKDIV	clk)
{
	unsigned int div, clock;
		div = (INREG32(A_CPM_CPCCR) >> (unsigned int)clk) & 0x0000000F;
		clock = GetCurrentPLLClock(0) / div_matrix[div];


	return (clock);
}

//----------------------------------------------------------------------------
int find_cpudiv(int div)
{
	int i;
	for(i = 0;i < sizeof(div_matrix)/sizeof(div_matrix[0]);i++)
		if(div == div_matrix[i])
			return i;
	return -1;
}
//----------------------------------------------------------------------------

void udelay(unsigned int usec)
{
	unsigned int i = usec * (GetCommonClock(CPM_CDIV) / 2000000);

	__asm__ __volatile__ (
		"\t.set noreorder\n"
		"1:\n\t"
		"bne\t%0, $0, 1b\n\t"
		"addi\t%0, %0, -1\n\t"
		".set reorder\n"
		: "=r" (i)
		: "0" (i)
	);
}

//----------------------------------------------------------------------------

void mdelay(unsigned int msec)
{
	unsigned int i;

	for (i = 0; i < msec; i++)
		udelay(1000);
}
//----------------------------------------------------------------------------

