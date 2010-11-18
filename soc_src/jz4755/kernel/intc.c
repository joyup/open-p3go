/********************** BEGIN LICENSE BLOCK **********************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
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
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn>
 *
 *  Create:   2008-06-26, by dsqiu
 *
 *  Maintain: 2008-09-26, by zzhang, migrate from JZ4740
 *
 *  Maintain: 2008-10-09, by whxu, clean up the code
 *
 *  Maintain: 2008-10-10, by whxu, Remove ack_irq(), fix TCU2 bug
 *
 *****************************************************************************
 */
#include <stdio.h>
#include <mipsregs.h>
#include <jz4755.h>


static struct {
	void (*handler)(unsigned int);
	unsigned int arg;
} irq_table[IRQ_MAX];

//----------------------------------------------------------------------------

static void default_handler(unsigned int arg)
{
#if (RELEASE == 0)
	#undef printf
	register unsigned int	group, bit;
	if (arg < IRQ_INTC_MAX)
	{
		printf("Get the irq%d of INTC without irq handler, STOP here for ever\n", arg);
	}
	else if (arg < EIRQ_TCU2_MAX)
	{
		printf("Get the irq%d of TCU2 without irq handler, STOP here for ever\n", (arg - EIRQ_TCU2_BASE));
	}
	else if (arg < EIRQ_DMA_MAX)
	{
		printf("Get the irq%d of DMA without irq handler, STOP here for ever\n", (arg - EIRQ_DMA_BASE));
	}
	else if (arg < EIRQ_GPIO_MAX)
	{

		group = (arg - EIRQ_GPIO_BASE) / 32;
		bit   = (arg - EIRQ_GPIO_BASE) % 32;
		printf("Get the irq%d of GPIO group%d without irq handler, STOP here for ever\n", bit,group);

	}
	printf("ICMR= 0x%08x \n", INREG32(0xb0001004));
	printf("ICPR= 0x%08x \n", INREG32(0xb0001010));
	while(1);
#endif
}

//----------------------------------------------------------------------------

void enable_irq(unsigned int irq)
{
	register unsigned int	group, bit;

	if (irq < IRQ_INTC_MAX)
	{
		OUTREG32(A_INTC_ICMCR, 1 << irq);
	}
	else if (irq < EIRQ_TCU2_MAX)
	{
		OUTREG32(A_INTC_ICMCR, EIRQ_TCU2_BIT);
		OUTREG32(A_TCU_TMCR, TCU_INTR_FLAG(irq - EIRQ_TCU2_BASE));
	}
	else if (irq < EIRQ_DMA_MAX)
	{
		OUTREG32(A_INTC_ICMCR, EIRQ_DMA_A2B_BIT);
		SETREG32(A_DMA_DCM(irq - EIRQ_DMA_BASE), DCM_TRANS_INTR_EN);
	}
	else if (irq < EIRQ_GPIO_MAX)
	{
		OUTREG32(A_INTC_ICMCR, EIRQ_PORT_A2F_BIT);

		group = (irq - EIRQ_GPIO_BASE) / 32;
		bit   = (irq - EIRQ_GPIO_BASE) % 32;
		OUTREG32(A_GPIO_PXFLGC(group), 1 << bit);
		OUTREG32(A_GPIO_PXIMC(group), 1 << bit);
	}
}

//----------------------------------------------------------------------------

void disable_irq(unsigned int irq)
{
	register unsigned int	group, bit;

	if (irq < IRQ_INTC_MAX)
	{
		OUTREG32(A_INTC_ICMSR, 1 << irq);
	}
	else if (irq < EIRQ_TCU2_MAX)
	{
		OUTREG32(A_TCU_TMSR, TCU_INTR_FLAG(irq - EIRQ_TCU2_BASE));
	}
	else if (irq < EIRQ_DMA_MAX)
	{
		CLRREG32(A_DMA_DCM(irq - EIRQ_DMA_BASE), DCM_TRANS_INTR_EN);
	}
	else if (irq < EIRQ_GPIO_MAX)
	{
		group = (irq - EIRQ_GPIO_BASE) / 32;
		bit   = (irq - EIRQ_GPIO_BASE) % 32;
		OUTREG32(A_GPIO_PXIMS(group), 1 << bit);
	}
}

//----------------------------------------------------------------------------
#if 0
static inline int intc_irq2(void)
{
	register unsigned int irq, ipl, dma_ctrller;
	unsigned int i, group;

	ipl = INREG32(A_INTC_ICPR);

	if (ipl & (1 << IRQ_IPU))
		return IRQ_IPU;

	if (ipl == 0)
		return -1;

	for (irq = 31; irq >= 0; irq--)
		if (ipl & (1 << irq))
			break;

	if (ipl & EIRQ_ALL_BIT)
	{
		if (ipl & EIRQ_DMA_A2B_BIT)
		{
			if (ipl & EIRQ_DMA_A_BIT)
				dma_ctrller = 0;
			else
				dma_ctrller = 1;
	
			ipl = INREG32(A_DMA_DIRQP(dma_ctrller));
			for (irq = 0; irq < DMA_CHANNEL_NUM; irq++)
				if (ipl & (1 << irq))
					break;
	
			irq += (EIRQ_DMA_BASE + dma_ctrller * DMA_CHANNEL_NUM);				
		}
		else if (ipl & EIRQ_TCU2_BIT)
		{
			ipl = INREG32(A_TCU_TFR) & (~INREG32(A_TCU_TMR));
			for (irq = 0; irq < NUM_TIMER_TCU2; irq++)
				if (ipl & TCU_INTR_FLAG(irq))
					break;
			irq += EIRQ_TCU2_BASE;
		}
		else
		{
			if (irq <= IRQ_PORTA)
			{
				group = IRQ_PORTA - irq;
				irq = EIRQ_GPIO_BASE + __gpio_group_irq(group) + group * 0x20;
			}
		}
	}

	return (irq);
}
#endif
static inline int intc_irq(void)
{
	register unsigned int irq, ipl, dma_ctrller;
	unsigned int i, group;

	ipl = INREG32(A_INTC_ICPR);

	if (ipl & (1 << IRQ_IPU))
		return IRQ_IPU;

	if (ipl == 0)
	{
		//printf("stf_a=%d\n",*(volatile unsigned int *)0xb3016000);
		if(*(volatile int *)0xb3016000)  //for irq_sft
		{
			return -1;
		}
		else
			irq = IRQ_SFT;
	}
	for (irq = 31; irq >= 0; irq--)
		if (ipl & (1 << irq))
			break;

	if (ipl & EIRQ_ALL_BIT)
	{
		if (ipl & EIRQ_DMA_A2B_BIT)
		{
			if (ipl & EIRQ_DMA_A_BIT)
				dma_ctrller = 0;
			else
				dma_ctrller = 1;
	
			ipl = INREG32(A_DMA_DIRQP(dma_ctrller));
			for (irq = 0; irq < DMA_CHANNEL_NUM; irq++)
				if (ipl & (1 << irq))
					break;
	
			irq += (EIRQ_DMA_BASE + dma_ctrller * DMA_CHANNEL_NUM);				
		}
		else if (ipl & EIRQ_TCU2_BIT)
		{
			ipl = INREG32(A_TCU_TFR) & (~INREG32(A_TCU_TMR));
			for (irq = 0; irq < NUM_TIMER_TCU2; irq++)
				if (ipl & TCU_INTR_FLAG(irq))
					break;
			irq += EIRQ_TCU2_BASE;
		}
		else
		{
			if (irq <= IRQ_PORTA)
			{
				group = IRQ_PORTA - irq;
				irq = EIRQ_GPIO_BASE + __gpio_group_irq(group) + group * 0x20;
			}
		}
	}

	return (irq);
}

//----------------------------------------------------------------------------

void intc_init(void)
{
	unsigned int i;

	for (i = 0; i < IRQ_MAX; i++)
	{
		disable_irq(i);
		irq_table[i].handler = default_handler;
		irq_table[i].arg = i;
	}
}

//----------------------------------------------------------------------------

int request_irq(unsigned int irq, void (*handler)(unsigned int), unsigned arg)
{
	if (irq > IRQ_MAX)
	{
		printf("The IRQ[%d] requested is larger than IRQ_MAX[%d]\n",
					irq, IRQ_MAX);
		return (-1);
	}

	if (irq_table[irq].handler != default_handler)
	{
		printf("The IRQ[%d] has been requested already\n", irq);
		return (-1);
	}

	irq_table[irq].handler = handler;
	irq_table[irq].arg = arg;

	enable_irq(irq);
	return 0;
	
}

//----------------------------------------------------------------------------

void free_irq(unsigned int irq)
{
	disable_irq(irq);
	irq_table[irq].handler = default_handler;
	irq_table[irq].arg = irq;
}

//----------------------------------------------------------------------------

typedef struct CP0_tstREGS
{
	unsigned int dwStatus;
	unsigned int dwCause;
} CP0_tstREGS;

void C_vINTHandler(CP0_tstREGS *pstC0)
{
	register int irq = intc_irq();

	if (irq < 0)
		return;

	if (irq == IRQ_SYSTEM_TIMER)
	{
		OUTREG32(A_TCU_TFCR, TCU_INTR_FLAG(TCU_SYSTEM_TIMER_CHN));
		os_SystemTick();
	}
	else
	{
		irq_table[irq].handler(irq_table[irq].arg);
	}
}

//----------------------------------------------------------------------------

unsigned int mips_get_sr(void)
{
	unsigned int t = read_c0_status();

	return (t);
}

//----------------------------------------------------------------------------

void cli(void)
{
	register unsigned int t;

	t = read_c0_status();
	t &= ~1;
	write_c0_status(t);
}

//----------------------------------------------------------------------------

void sti(void)
{
	register unsigned int t;

	t = read_c0_status();
	t |= 1;
	t &= ~2;
	write_c0_status(t);
}
//----------------------------------------------------------------------------
unsigned int spin_lock_irqsave(void)
{
	register unsigned int t;
	t = read_c0_status();
	write_c0_status((t & (~1)));
	return (t);
}

//----------------------------------------------------------------------------

void spin_unlock_irqrestore(unsigned int val)
{
	write_c0_status(val);
}

//----------------------------------------------------------------------------
