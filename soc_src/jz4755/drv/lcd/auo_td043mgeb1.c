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
 *  Author:  <whxu@ingenic.cn>
 *
 *	Create: 2008-11-19, by whxu
 *
 *	Maintain: 2008-11-29, by whxu, change backlight and gpio
 *
 *****************************************************************************
 */

#if (LCDTYPE == 5)
#include <jz4755.h>
#include "lcdc.h"

#define PWM_FREQ		(50 * 1000)	// Condigure according to the BKL chip

//----------------------------------------------------------------------------

void lcd_board_init(unsigned int bReInit)
{
	__gpio_as_lcd_24bit();

#ifdef LCD_POWER_PIN
	// Power ON LCD33.
	gpio_set_dir(LCD_POWER_PIN, 1, LCD_POWER_ON_LEVEL);
	udelay(10);
#endif

#ifdef LCD_STANDBY_PIN
	// Set Standby Mode LCD33.
	gpio_set_dir(LCD_STANDBY_PIN, 1, !LCD_STANDBY_LEVEL);
	udelay(10);
#endif

#ifdef LCD_RESET_PIN
	// Hardware Reset
	gpio_set_dir(LCD_RESET_PIN, 1, LCD_RESET_LEVEL);
	mdelay(20);		// The Delay should be > 10ms.
	gpio_set_dir(LCD_RESET_PIN, 1, !LCD_RESET_LEVEL);
	mdelay(5);
#endif

	// L3 Init
	l3_gpio_init();

	// Configure LCD Registers
	l3_write_reg(0x02, 0x07);
	l3_write_reg(0x03, 0x5F);
	l3_write_reg(0x04, 0x17);
	l3_write_reg(0x05, 0x20);
	l3_write_reg(0x06, 0x08);
	l3_write_reg(0x07, 0x20);
	l3_write_reg(0x08, 0x20);
	l3_write_reg(0x09, 0x20);
	l3_write_reg(0x0A, 0x20);
	l3_write_reg(0x0B, 0x20);
	l3_write_reg(0x0C, 0x20);
	l3_write_reg(0x0D, 0x22);

	// R, G, B Brightness
	l3_write_reg(0x0E, 0x2F);
	l3_write_reg(0x0F, 0x2F);
	l3_write_reg(0x10, 0x2F);

/*
	// Gamma setting(1)
	l3_write_reg(0x11, 0x15);
	l3_write_reg(0x12, 0xAA);
	l3_write_reg(0x13, 0xFF);
	l3_write_reg(0x14, 0x86);
	l3_write_reg(0x15, 0x84);
	l3_write_reg(0x16, 0xC3);
	l3_write_reg(0x17, 0xD8);
	l3_write_reg(0x18, 0x01);
	l3_write_reg(0x19, 0x28);
	l3_write_reg(0x1A, 0x53);
	l3_write_reg(0x1B, 0xC5);
	l3_write_reg(0x1C, 0x26);
	l3_write_reg(0x1D, 0x74);
	l3_write_reg(0x1E, 0xAE);
	l3_write_reg(0x1F, 0xFF);

	// Gamma setting(2)
	l3_write_reg(0x11, 0x15);
	l3_write_reg(0x12, 0xaa);
	l3_write_reg(0x13, 0xFF);
	l3_write_reg(0x14, 0x86);
	l3_write_reg(0x15, 0x89);
	l3_write_reg(0x16, 0xc6);
	l3_write_reg(0x17, 0xea);
	l3_write_reg(0x18, 0x0c);
	l3_write_reg(0x19, 0x33);
	l3_write_reg(0x1A, 0x5e);
	l3_write_reg(0x1B, 0xd0);
	l3_write_reg(0x1C, 0x33);
	l3_write_reg(0x1D, 0x7e);
	l3_write_reg(0x1E, 0xb3);
	l3_write_reg(0x1F, 0xFF);

	// Gamma setting(3)
	l3_write_reg(0x11, 0x15);
	l3_write_reg(0x12, 0x6a);
	l3_write_reg(0x13, 0xFF);
	l3_write_reg(0x14, 0x86);
	l3_write_reg(0x15, 0x7c);
	l3_write_reg(0x16, 0xc2);
	l3_write_reg(0x17, 0xd1);
	l3_write_reg(0x18, 0xf5);
	l3_write_reg(0x19, 0x25);
	l3_write_reg(0x1A, 0x4a);
	l3_write_reg(0x1B, 0xbf);
	l3_write_reg(0x1C, 0x15);
	l3_write_reg(0x1D, 0x6a);
	l3_write_reg(0x1E, 0xa4);
	l3_write_reg(0x1F, 0xFF);
*/
	// Gamma setting(4)
	l3_write_reg(0x11, 0x15);
	l3_write_reg(0x12, 0xaa);
	l3_write_reg(0x13, 0xFF);
	l3_write_reg(0x14, 0x86);
	l3_write_reg(0x15, 0x8e);
	l3_write_reg(0x16, 0xd6);
	l3_write_reg(0x17, 0xfe);
	l3_write_reg(0x18, 0x28);
	l3_write_reg(0x19, 0x52);
	l3_write_reg(0x1A, 0x7c);
	l3_write_reg(0x1B, 0xe9);
	l3_write_reg(0x1C, 0x42);
	l3_write_reg(0x1D, 0x88);
	l3_write_reg(0x1E, 0xb8);
	l3_write_reg(0x1F, 0xff);

	l3_write_reg(0x20, 0xf0);
	l3_write_reg(0x21, 0xf0);
	l3_write_reg(0x22, 0x08);

	if (!bReInit)
	{
		pwm_init(LCD_BACKLIGHT_PWM_CHN, PWM_FREQ, 2);
	}
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
	pwm_set_duty(LCD_BACKLIGHT_PWM_CHN, percent);
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
	pwm_set_duty(LCD_BACKLIGHT_PWM_CHN, 0);
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
#if 0 //xltao del
#ifdef LCD_STANDBY_PIN
	gpio_set_dir(LCD_STANDBY_PIN, 1, !LCD_STANDBY_LEVEL);
#else
	gpio_set_dir(LCD_POWER_PIN, 1, LCD_POWER_ON_LEVEL);
	gpio_set_dir(LCD_RESET_PIN, 1, LCD_RESET_LEVEL);
	mdelay(20);		// The Delay should be > 10ms.
	gpio_set_dir(LCD_RESET_PIN, 1, !LCD_RESET_LEVEL);
	mdelay(5);
#endif
#endif
}

void lcd_display_off(void)
{
#if 0 //xltao del
#ifdef LCD_STANDBY_PIN
	gpio_set_dir(LCD_STANDBY_PIN, 1, LCD_STANDBY_LEVEL);
#else
	gpio_set_dir(LCD_POWER_PIN, 1, !LCD_POWER_ON_LEVEL);
#endif
#endif
}

//----------------------------------------------------------------------------
#endif
