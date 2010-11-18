/********************** BEGIN LICENSE BLOCK **********************************
 *
 * JZ4750  mobile_tv  Project  V1.0.0
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
 *  Author:  <jbyu@ingenic.cn>
 *
 *	Create: 2008-09-26, by jbyu
 *
 *	Create: 2008-11-29, by whxu
 *
 *****************************************************************************
 */

#if (LCDTYPE == 99)
#include <jz4755.h>

#define PWM_FREQ		(50 * 1000)	// Condigure according to the BKL chip

//----------------------------------------------------------------------------
void lcd_board_init(void)
{
}

//----------------------------------------------------------------------------
static unsigned int level = 0;
void lcd_set_backlight(unsigned int percent)
{
	if (is_tv_show())
	{
		level = percent;
		return;
	}
	level = percent;
}
//----------------------------------------------------------------------------
int lcd_get_backlight()
{
	return level;
}

//----------------------------------------------------------------------------

void lcd_close_backlight(void)
{
}

//----------------------------------------------------------------------------
/*
#ifdef LCD_STANDBY_PIN
void lcd_display_on(void)
{
	gpio_set_dir(LCD_STANDBY_PIN, 1, LCD_STANDBY_LEVEL);
}

void lcd_display_off(void)
{
	unsigned int level = LCD_STANDBY_LEVEL ? GPIO_LOW_LEVEL : GPIO_HIGH_LEVEL;
	gpio_set_dir(LCD_STANDBY_PIN, 1, level);
}
#endif
*/
void lcd_display_on(void)
{
}

void lcd_display_off(void)
{
}
//----------------------------------------------------------------------------

#endif
