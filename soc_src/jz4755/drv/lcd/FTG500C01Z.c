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
 *****************************************************************************
 */

#if (LCDTYPE == 7)
#include <jz4755.h>

#define PWM_FREQ		(50 * 1000)	// Condigure according to the BKL chip

//----------------------------------------------------------------------------

void lcd_board_init(unsigned int bReInit)
{
	__gpio_as_lcd_24bit();


#if 0 //xltao add for test 
	lcd_power_on_xltao();	
#endif 

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
	mdelay(5);		// The Delay should be > 1ms.
#endif
  	os_TimeDelay(20); //xltao add
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
#if 0  //xltao del
#ifdef LCD_STANDBY_PIN
	gpio_set_dir(LCD_STANDBY_PIN, 1, LCD_STANDBY_LEVEL);
#else
	gpio_set_dir(LCD_POWER_PIN, 1, !LCD_POWER_ON_LEVEL);
#endif
#endif
}


//----------------------------------------------------------------------------

#endif
