/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright Ingenic Semiconductor Co. Ltd 2008. All rights reserved.
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
 *  Create:   2008-06-26, by dsqiu, dsqiu
 *
 *  Maintain: 2008-11-13, by whxu, Add gpio_intr_init
 *
 *  Maintain: 2008-11-19, by whxu, Add gpio_set_dir
 *
 *******************************************************************************
 */

#include <bsp.h>
#include <jz4755.h>

//----------------------------------------------------------------------------

unsigned int gpio_intr_init(GPIO_INTR_TRIGGER_TYPE gitt, unsigned int pin)
{
	unsigned int base, bit;

	if ((pin < NUM_GPIO) && (gitt < GITT_MAX))
	{
		base = GPIO_BASE_U_VIRTUAL + pin / 32 * 0x100;
		bit = 1 << (pin % 32);

		OUTREG32(base + GPIO_PXPIN_OFFSET, bit);
		if ((gitt == GITT_F_EDGE) || (gitt == GITT_R_EDGE))
			OUTREG32(base + GPIO_PXTRGS_OFFSET, bit);	// Edge Trigger
		else
			OUTREG32(base + GPIO_PXTRGC_OFFSET, bit);	// Level Trigger

		OUTREG32(base + GPIO_PXFUNC_OFFSET, bit);
		OUTREG32(base + GPIO_PXSELS_OFFSET, bit);

		if ((gitt == GITT_R_EDGE) || (gitt == GITT_H_LEVEL))	
			OUTREG32(base + GPIO_PXDIRS_OFFSET, bit);	// Rising Edge or High Level
		else
			OUTREG32(base + GPIO_PXDIRC_OFFSET, bit);	// Falling Edge or Low Level

		OUTREG32(base + GPIO_PXFLGC_OFFSET, bit);

		return (1);
	}
	else
		return (0);
}

// ---------------------------------------------------------------------------
//
// @func    void | gpio_set_dir | Configure the GPIO direction 
//
// @param
//			@1 unsigned int pin
//				-> The gpio index 
//			@2 bool output
//				-> The gpio direction, 1 is output and 0 is input
//			@3 bool level_or_pull
//				-> When the @2 is 1, the @3 is level, 1 is HIGH, 0 is LOW;
//				   When the @2 is 0, the @3 is PULL, 1 is PULL, 0 is NO-PULL;
//
// @rdesc   void.
//
// ---------------------------------------------------------------------------

void gpio_set_dir(unsigned int pin, bool output, bool level_or_pull)
{
	unsigned int base, bit;

	base = GPIO_BASE_U_VIRTUAL + pin / 32 * 0x100;
	bit = 1 << (pin % 32);

	OUTREG32(base + GPIO_PXFUNC_OFFSET, bit);	
	OUTREG32(base + GPIO_PXSELC_OFFSET, bit);
	if (output)
	{
		OUTREG32(base + GPIO_PXDIRS_OFFSET, bit);
		OUTREG32(base + GPIO_PXPES_OFFSET, bit);
		if (level_or_pull)
			OUTREG32(base + GPIO_PXDATS_OFFSET, bit);
		else
			OUTREG32(base + GPIO_PXDATC_OFFSET, bit);
	}
	else
	{
		OUTREG32(base + GPIO_PXDIRC_OFFSET, bit);
		if (level_or_pull)
			OUTREG32(base + GPIO_PXPEC_OFFSET, bit);
		else
			OUTREG32(base + GPIO_PXPES_OFFSET, bit);
	}
}

//----------------------------------------------------------------------------

void gpio_init(void)
{
#ifdef AMP_SHUT_DOWN_PIN
	// Shut Down AMP if NEEDED
	#if (AMP_SHUT_DOWN_LEVEL == GPIO_LOW_LEVEL)
		gpio_set_dir(AMP_SHUT_DOWN_PIN, 1, 0);
	#else
		gpio_set_dir(AMP_SHUT_DOWN_PIN, 1, 1);
	#endif
#endif
}

//----------------------------------------------------------------------------
