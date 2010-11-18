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
 *  Author:  <whxu@ingenic.cn>
 *
 *  Create:   2008-10-13, by whxu
 *
 *  Maintain:   2008-10-28, by jbyu
 *
 *  Maintain:   2008-11-04, by whxu, add macro DEVICE_CLOCK for apus v1.1
 *
 *  Maintain:   2008-11-29, by whxu, add LCD GPIO configuration
 *
 ****************************************************************************/


#ifndef __APUS_CFG_H__
#define __APUS_CFG_H__

//----------------------------------------------------------------------------
//	Main Oscillator Frequency
//----------------------------------------------------------------------------
#define OSC_CLOCK       24000000	// The main OSC clock(PLL-IN, Unit: Hz)
#define	DEVICE_CLOCK	12000000	// The clock for i2c, uart, tcu, aic, udc,
									// owi and sadc(Unit: Hz)

//----------------------------------------------------------------------------
//	CPU Clock Configuration
//----------------------------------------------------------------------------
//#define CFG_CPU_SPEED		360000000	// Unit: Hz
#define CFG_CPU_SPEED		384000000


#if (OSC_CLOCK == 24000000)
	// Default CPCCR Configuration, C:H:P:M = 3:1:1:1
	#define CPCCR_CFG	( (XDIV_1 << CPCCR_CDIV_BIT)		\
						| (XDIV_2 << CPCCR_H1DIV_BIT)		\
						| (XDIV_3 << CPCCR_H0DIV_BIT)		\
						| (XDIV_3 << CPCCR_PDIV_BIT)		\
						| (XDIV_3 << CPCCR_MDIV_BIT) 		\
						| CPCCR_PLLCLK_DIV2					\
						| CPCCR_ECS_EXCLK_DIV2 )
#elif (OSC_CLOCK == 12000000)
// Default CPCCR Configuration, C:H:P:M = 3:1:1:1
#define CPCCR_CFG	( (XDIV_1 << CPCCR_CDIV_BIT)		\
					| (XDIV_3 << CPCCR_H0DIV_BIT)		\
					| (XDIV_3 << CPCCR_PDIV_BIT)		\
					| (XDIV_3 << CPCCR_MDIV_BIT) 		\
					| CPCCR_PLLCLK_DIV2 )
#endif

#define	CPPCR_CFG	( ((CFG_CPU_SPEED / (OSC_CLOCK/2) - 2) << CPPCR_PLLM_BIT)\
					| (0x20 << CPPCR_PLLST_BIT)								\
					| (CPPCR_PLLEN) )


//----------------------------------------------------------------------------
//	CPU Clock Configuration
//----------------------------------------------------------------------------
#define	DEBUG_UART_BASE			UART1_BASE_U_VIRTUAL
#define DEBUG_UART_BAUDRATE		57600


//----------------------------------------------------------------------------
//	Cache Configurations
//----------------------------------------------------------------------------
#define CFG_DCACHE_SIZE		(16 * 1024)		// Unit: Byte
#define CFG_ICACHE_SIZE		(16 * 1024)		// Unit: Byte
#define CFG_CACHELINE_SIZE	(32)


//----------------------------------------------------------------------------
//	SDRAM Configurations
//----------------------------------------------------------------------------
// SDRAM Timings, unit: ns
#define SDRAM_TRAS		45	/* RAS# Active Time */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
//#define SDRAM_TREF		15625	/* Refresh period: 4096 refresh cycles/64ms */
#define SDRAM_TREF		7812	/* Refresh period: 8192 refresh cycles/64ms */


//----------------------------------------------------------------------------
//	GPIO Configurations
//----------------------------------------------------------------------------
#define GPIO_UDC_DETE_PIN		(GPIO_GROUP_E + 8)

/*xltao del
#define MMC0_POWER_PIN			(GPIO_GROUP_E + 0)
#define MMC0_CD_PIN				(GPIO_GROUP_E + 1)
#define MMC0_SD_WP				(GPIO_GROUP_E + 2)
#define CARD_INSERT_VALUE0		GPIO_LOW_LEVEL
*/

#define MMC1_POWER_PIN			(UNUSED_GPIO_PIN)
#define MMC1_CD_PIN				(GPIO_GROUP_E + 18)
#define MMC1_SD_WP				(UNUSED_GPIO_PIN)
#define CARD_INSERT_VALUE1		GPIO_LOW_LEVEL

// Keypad
#define AD_KEY_INT_PIN			(UNUSED_GPIO_PIN)
/*
#define	KEY_GAME_A_PIN			(GPIO_GROUP_F + 11)
#define	KEY_GAME_B_PIN		(GPIO_GROUP_D + 27)
#define	KEY_GAME_L_PIN			(GPIO_GROUP_C + 24)
#define	KEY_GAME_R_PIN			(GPIO_GROUP_C + 23)
#define	KEY_GAME_UP_PIN			(GPIO_GROUP_D + 25)
#define	KEY_GAME_DOWN_PIN			(GPIO_GROUP_D + 17)
#define	KEY_GAME_LEFT_PIN			(GPIO_GROUP_D + 16)
#define	KEY_GAME_RIGHT_PIN			(GPIO_GROUP_D + 18)
#define	KEY_GAME_START_PIN		(GPIO_GROUP_D + 24)
#define	KEY_GAME_SELECT_PIN		(GPIO_GROUP_D + 21)
#define	KEY_GAME_BRIGHT_PIN		(GPIO_GROUP_B + 23)
#endif

#define	KEY_DOWN_VALUE			( GPIO_LOW_LEVEL )
// The Special pin Key Down Value is NOT the KEY_DOWN_VALUE
#define	KEY_SPECIAL_PIN		(0)
*/
// LCD
#if 0
#if (LCDTYPE == 5) //xltao add the line 
#define	LCD_POWER_PIN		(GPIO_GROUP_E + 19)
#define	LCD_POWER_ON_LEVEL	(GPIO_HIGH_LEVEL)

#define	LCD_RESET_PIN		(UNUSED_GPIO_PIN)
#define	LCD_RESET_LEVEL		(GPIO_LOW_LEVEL)

#define	LCD_STANDBY_PIN		(GPIO_GROUP_E + 18)
#define	LCD_STANDBY_LEVEL	(GPIO_HIGH_LEVEL)
#endif
#define	LCD_BACKLIGHT_PWM_CHN	(2)

// SLCD
#define SLCD_CS_PIN			(GPIO_GROUP_D + 22)
#define	SLCD_CS_LEVEL		(GPIO_LOW_LEVEL)

#define SLCD_RESET_PIN 		(GPIO_GROUP_D + 23)
#define	SLCD_RESET_LEVEL	(GPIO_LOW_LEVEL)

#define SLCD_RS_PIN			(UNUSED_GPIO_PIN)
#define SLCD_BACKLIGHT_PWM_CHN	(2)
#endif

//Camera
//#define CAM_RESET_PIN		(GPIO_GROUP_B+27) 
//#define CAM_STANDBY_PIN		(GPIO_GROUP_B+28)  

// TV
#define TV_OUT_MODE_CVBS		0x00aa
#define TV_OUT_MODE_SVIDEO		0x00bb
#define TV_OUT_MODE_3DIV		0x00cc
//#define TV_VIDEO_OUT_MODE		TV_OUT_MODE_SVIDEO
#define TV_VIDEO_OUT_MODE		TV_OUT_MODE_CVBS
//#define TV_VIDEO_OUT_MODE		TV_OUT_MODE_3DIV

// Battery Monitor
/*
#define	DC_DETE_PIN		(GPIO_GROUP_E + 8)
#define	DC_DETE_LEVEL	(GPIO_HIGH_LEVEL)

#define	CHARG_STAT_PIN		(GPIO_GROUP_E + 10)
#define	CHARG_STAT_LEVEL	(GPIO_LOW_LEVEL)
*/
#define MODULE_SDA_PIN	(GPIO_GROUP_E + 13)
#define MODULE_CLK_PIN	(GPIO_GROUP_E + 12)

//2010.10.18 danny added
#define BOOT_COMMON_PARAMETER_ADDR		(0xa0122000 - 4)	//DRVMEMADDR-4
#endif	// __APUS_CFG_H__
