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

#if (LCDTYPE == 6)
#include <jz4755.h>

#define PWM_FREQ		(50 * 1000)	// Condigure according to the BKL chip

//----------------------------------------------------------------------------
#define EXT_PIN		(GPIO_GROUP_F + 12)
void lcd_board_init(void)
{
	__gpio_as_lcd_24bit();
#ifdef LCD_POWER_PIN
	// Power ON LCD33.
	gpio_set_dir(LCD_POWER_PIN, 1, LCD_POWER_ON_LEVEL);
	udelay(10);
#endif
gpio_set_dir(EXT_PIN, 1, 0);
#ifdef LCD_STANDBY_PIN
	// Set Standby Mode LCD33.
	gpio_set_dir(LCD_STANDBY_PIN, 1, LCD_STANDBY_LEVEL);
	udelay(10);
#endif

#ifdef LCD_RESET_PIN
	// Hardware Reset
	gpio_set_dir(LCD_RESET_PIN, 1, LCD_RESET_LEVEL);
	mdelay(20);		// The Delay should be > 10ms.
#if LCD_RESET_LEVEL
	gpio_set_dir(LCD_RESET_PIN, 1, GPIO_LOW_LEVEL);
#else
	gpio_set_dir(LCD_RESET_PIN, 1, GPIO_HIGH_LEVEL);
#endif
	mdelay(5);		// The Delay should be > 1ms.
#endif

	// L3 Init
	l3_gpio_init();

	l3_write_reg(0x00, 0x07);
	l3_write_reg(0x01, 0x00);
	l3_write_reg(0x02, 0x03);
	l3_write_reg(0x03, 0xCC);
	l3_write_reg(0x04, 0x46);
	l3_write_reg(0x05, 0x0D);
	l3_write_reg(0x06, 0x00);
	l3_write_reg(0x07, 0x00);
	l3_write_reg(0x08, 0x08);
	l3_write_reg(0x09, 0x40);
	l3_write_reg(0x0A, 0x88);
	l3_write_reg(0x0B, 0x88);
	l3_write_reg(0x0C, 0x20);
	l3_write_reg(0x0D, 0x20);
	l3_write_reg(0x0E, 0x10);
	l3_write_reg(0x0F, 0xA4);
	l3_write_reg(0x10, 0x04);
	l3_write_reg(0x11, 0x24);
	l3_write_reg(0x12, 0x24);
	l3_write_reg(0x1E, 0x00);
	l3_write_reg(0x20, 0x00);

	gpio_set_dir(EXT_PIN, 1, 1);
	pwm_init(LCD_BACKLIGHT_PWM_CHN, PWM_FREQ, 2);

}

//----------------------------------------------------------------------------
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
#ifdef LCD_STANDBY_PIN
	gpio_set_dir(LCD_STANDBY_PIN, 1, !LCD_STANDBY_LEVEL);
#else
	gpio_set_dir(LCD_POWER_PIN, 1, LCD_POWER_ON_LEVEL);
	gpio_set_dir(LCD_RESET_PIN, 1, LCD_RESET_LEVEL);
	mdelay(20);		// The Delay should be > 10ms.
	gpio_set_dir(LCD_RESET_PIN, 1, !LCD_RESET_LEVEL);
	mdelay(5);
#endif
}

void lcd_display_off(void)
{
#ifdef LCD_STANDBY_PIN
	gpio_set_dir(LCD_STANDBY_PIN, 1, LCD_STANDBY_LEVEL);
#else
	gpio_set_dir(LCD_POWER_PIN, 1, !LCD_POWER_ON_LEVEL);
#endif
}
//----------------------------------------------------------------------------

#endif
