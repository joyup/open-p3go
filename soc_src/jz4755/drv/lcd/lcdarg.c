/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
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
 *	Author:  <jbyu@ingenic.cn>
 *
 *	Create: 2008-09-26, by jbyu
 *
 *******************************************************************************
 */

#include <jz4755lcd.h>
#include "lcdc.h"

// TV parameter
#define TVE_WIDTH_PAL 				( 720 )
#define TVE_HEIGHT_PAL 				( 573 )
#define DISP_WIDTH_PAL				( 704 - 32)
#define DISP_HEIGHT_PAL				( 573 - 21 )
#define TVE_FREQ_PAL 				( 50 )
#define TVE_WIDTH_NTSC 				( 720 )
#define TVE_HEIGHT_NTSC 			( 482 )
#define DISP_WIDTH_NTSC				( 704 - 32 )
#define DISP_HEIGHT_NTSC			( 482 - 12 )
#define TVE_FREQ_NTSC 				( 60 )

LCD_PARAM lcd_param =
{
#if (LCDTYPE == 1)
	.lcd_mode =
	{
		.vendor = "Samsung",
		.type = "tft",
	//  width,height,freq,hsync,vsync,elw,blw,efw,bfw
		480, 272, 60, 41, 10, 2, 2, 2, 2,
		.mode_bitmask = (unsigned int *)0,
		.cfg = MODE_TFT_GEN | MODE_TFT_18BIT | LCDCFG_NEWDES | HSYNC_N | VSYNC_N,
		.ctrl = LCD_CTRL_BST_16,// | LCD_CTRL_EOFM,
	},
	.osd_mode =
	{
		.osd_cfg = LCD_OSDC_OSDEN | 		// Use OSD mode
		 LCD_OSDC_ALPHAEN | 				// enable alpha
		 LCD_OSDC_ALPHAMD			|		// alpha mode: 0, alpha register; 1, each pixel
		 LCD_OSDC_F0EN				|		// enable Foreground0		
		 LCD_OSDC_F1EN,						// enable Foreground1
		 .osd_ctrl = 0,						// disable ipu,
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, 				// set background color Black
		 .colorkey0 = 0,					// disable colorkey
		 .colorkey1 = 0,					// disable colorkey
		 .alpha = 0xA0,						// alpha value
		 .ipu_restart = 0x80001000,			// ipu restart
		 .fg_change = FG_CHANGE_ALL, 		// change all initially
		 .foreground0 = {32, 0, 0, 0, 0}, 		// bpp, x, y, w, h
#if (LCD32BPP)
		 .foreground1 = {32, 0, 0, 480, 272}, 	// bpp, x, y, w, h
#else
		 .foreground1 = {16, 0, 0, 480, 272}, 	// bpp, x, y, w, h
#endif
	}
#endif

#if (LCDTYPE == 2)
	.lcd_mode =
	{
		.vendor = "FOXCONN",
		.type = "tft",
		320, 240, 60, 1, 1, 10, 50, 10, 13,
		.mode_bitmask = (unsigned int *)0,
		.cfg = MODE_8BIT_SERIAL_TFT | LCDCFG_NEWDES | PCLK_N | HSYNC_N | VSYNC_N,
	},
	.osd_mode =
	{
		.osd_cfg = LCD_OSDC_OSDEN | 		// Use OSD mode
		 LCD_OSDC_ALPHAEN | 				// enable alpha
		 LCD_OSDC_ALPHAMD			|		// alpha mode: 0, alpha register; 1, each pixel
		 LCD_OSDC_F0EN				|		// enable Foreground0		
		 LCD_OSDC_F1EN,						// enable Foreground1
		 .osd_ctrl = 0,						// disable ipu,
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, 				// set background color Black
		 .colorkey0 = 0,					// disable colorkey
		 .colorkey1 = 0,					// disable colorkey
		 .alpha = 0xA0,						// alpha value
		 .ipu_restart = 0x80001000,			// ipu restart
		 .fg_change = FG_CHANGE_ALL, 		// change all initially
		 .foreground0 = {8, 0, 0, 0, 0}, 		// bpp, x, y, w, h
		 .foreground1 = {16, 0, 0, 320, 240}, 	// bpp, x, y, w, h
	}
#endif

#if (LCDTYPE == 4)
	.lcd_mode =
	{
		.vendor = "AUO_A043FL01_V1",
		.type = "tft",
		480, 272, 60, 41, 10, 8, 4, 4, 2,
		.mode_bitmask = (unsigned int *)0,
		.cfg = MODE_TFT_GEN | MODE_TFT_24BIT | LCDCFG_NEWDES | HSYNC_N | VSYNC_N | LCDCFG_RECOVER,
		.ctrl = LCD_CTRL_BST_16,
	},
	.osd_mode =
	{
		.osd_cfg =  LCD_OSDC_OSDEN	| 		// Use OSD mode
		 LCD_OSDC_ALPHAEN 			| 		// enable alpha
		 LCD_OSDC_ALPHAMD			|		// alpha mode: 0, alpha register; 1, each pixel
		 LCD_OSDC_F0EN				|		// enable Foreground0
		 LCD_OSDC_F1EN,						// enable Foreground1
		 .osd_ctrl = 0,						// disable ipu,
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, 				// set background color Black
		 .colorkey0 = 0,					// disable colorkey
		 .colorkey1 = 0,					// disable colorkey
		 .alpha = 0xa0,						// alpha value
		 .ipu_restart = 0x8000085d,			// ipu restart
		 .fg_change = FG_CHANGE_ALL, 		// change all initially
		 .foreground0 = {32, 0, 0, 0, 0}, 	// bpp, x, y, w, h
#if (LCD32BPP)
		 .foreground1 = {32, 0, 0, 480, 272}, 	// bpp, x, y, w, h
#else
		 .foreground1 = {16, 0, 0, 480, 272}, 	// bpp, x, y, w, h
#endif
	}
#endif

#if (LCDTYPE == 5)
	.lcd_mode =
	{
		.vendor = "TD043MGEB1",
		.type = "tft",
		// width,height,freq,hsync,vsync,elw,blw,efw,bfw
		800, 480, 50, 1, 1, 40, 215, 10, 34,
		.mode_bitmask = (unsigned int *)0,
		.cfg = MODE_TFT_GEN | MODE_TFT_24BIT | LCDCFG_NEWDES | HSYNC_N | VSYNC_N | LCDCFG_RECOVER,
		.ctrl = LCD_CTRL_BST_16 | LCD_CTRL_EOFM,
	},
	.osd_mode =
	{
		.osd_cfg =  LCD_OSDC_OSDEN	| 		// Use OSD mode
		 LCD_OSDC_ALPHAEN 			| 		// enable alpha
		 LCD_OSDC_ALPHAMD			|		// alpha mode: 0, alpha register; 1, each pixel
		 LCD_OSDC_F0EN				|		// enable Foreground1
		 LCD_OSDC_F1EN,						// enable Foreground1
		 .osd_ctrl = 0,						// disable ipu,
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, 				// set background color Black
		 .colorkey0 = 0,					// disable colorkey
		 .colorkey1 = 0,					// disable colorkey
		 .alpha = 0xa0,						// alpha value
		 .ipu_restart = 0x80002969,			// ipu restart
		 .fg_change = FG_CHANGE_ALL, 		// change all initially
		 .foreground0 = {32, 0, 0, 0, 0}, 	// bpp, x, y, w, h
#if (LCD32BPP)
		 .foreground1 = {32, 0, 0, 800, 480}, 	// bpp, x, y, w, h
#else
		 .foreground1 = {16, 0, 0, 800, 480}, 	// bpp, x, y, w, h
#endif
	}
#endif

#if (LCDTYPE == 6)
    .lcd_mode =
    {
            .vendor = "tmt035dnafwu24-2",
            .type = "tft",
            320, 240, 60, 1, 1, 10, 7, 2, 1,
            .mode_bitmask = (unsigned int *)0,
            .cfg =MODE_TFT_GEN|MODE_TFT_24BIT|LCDCFG_NEWDES|HSYNC_N|VSYNC_N|LCDCFG_RECOVER|PCLK_N,
            .ctrl = LCD_CTRL_BST_16, //| LCD_CTRL_OFUM,
    },
    .osd_mode =
    {
            .osd_cfg =  LCD_OSDC_OSDEN      		|  // Use OSD mode
             LCD_OSDC_ALPHAEN                       |  // enable alpha
             LCD_OSDC_ALPHAMD                       |  // alpha mode: 0, alpha register; 1, each pixel
             LCD_OSDC_F0EN                          |
             LCD_OSDC_F1EN,                            // enable Foreground1
             .osd_ctrl = 0,                            // disable ipu,
             .rgb_ctrl = 0,
             .bgcolor = 0x000000,                      // set background color Black
             .colorkey0 = 0,                           // disable colorkey
             .colorkey1 = 0,                           // disable colorkey
             .alpha = 0xa0,                            // alpha value
             .ipu_restart = 0x800002AF,                // ipu restart
             .fg_change = FG_CHANGE_ALL,               // change all initially
             .foreground0 = {24, 0, 0, 0, 0},   	   // bpp, x, y, w, h
#if (LCD32BPP)
             .foreground1 = {24, 0, 0, 320, 240},  	   // bpp, x, y, w, h
#else
             .foreground1 = {16, 0, 0, 320, 240},      // bpp, x, y, w, h
#endif
    }
#endif

//-----------------------------xltao add start-------------------------------
#if (LCDTYPE == 7)
	.lcd_mode =
	{
		.vendor = "FTG500C01Z",
		.type = "tft",
		// width,height,freq,hsync,vsync,elw,blw,efw,bfw
		//800, 480, 50, 1, 1, 40, 215, 10, 34, //xltao del
		800, 480, 30, 128,  2, 40, 215, 10, 34,
		//800, 480, 50, 128,  2, 41, 215, 10, 35,
		.mode_bitmask = (unsigned int *)0,
		.cfg = MODE_TFT_GEN | MODE_TFT_24BIT | LCDCFG_NEWDES | HSYNC_N | VSYNC_N | LCDCFG_RECOVER,
		.ctrl = LCD_CTRL_BST_16 | LCD_CTRL_EOFM,
	},
	.osd_mode =
	{
		.osd_cfg =  LCD_OSDC_OSDEN	| 		// Use OSD mode
		 LCD_OSDC_ALPHAEN 			| 		// enable alpha
		 LCD_OSDC_ALPHAMD			|		// alpha mode: 0, alpha register; 1, each pixel
		 LCD_OSDC_F0EN				|		// enable Foreground1
		 LCD_OSDC_F1EN,						// enable Foreground1
		 .osd_ctrl = 0,						// disable ipu,
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, 				// set background color Black
		 .colorkey0 = 0,					// disable colorkey
		 .colorkey1 = 0,					// disable colorkey
		 .alpha = 0xa0,						// alpha value
		 .ipu_restart = 0x80002969,			// ipu restart
		 .fg_change = FG_CHANGE_ALL, 		// change all initially
		 .foreground0 = {32, 0, 0, 0, 0}, 	// bpp, x, y, w, h
#if (LCD32BPP)
		 .foreground1 = {32, 0, 0, 800, 480}, 	// bpp, x, y, w, h
#else
		 .foreground1 = {16, 0, 0, 800, 480}, 	// bpp, x, y, w, h
#endif
	}
#endif
//-----------------------xltao add end---------------------------------------------

};

LCD_PARAM tv_param_pal =
{
	.lcd_mode =
	{
		.vendor = (unsigned char *)0,
		.type = "tv",
		// width, height, freq, hsync, vsync, elw,	blw, efw, bfw
		TVE_WIDTH_PAL, TVE_HEIGHT_PAL, TVE_FREQ_PAL, 0, 0, 0, 0, 0, 0,
		.mode_bitmask = (unsigned int *)0,
		.cfg =	LCDCFG_TVEN	| LCDCFG_TVEPEH /*| LCDCFG_RECOVER */| LCDCFG_NEWDES | MODE_CCIR656_INT,
		.ctrl = LCD_CTRL_BST_16,
	},
	.osd_mode =
	{
		.osd_cfg = LCD_OSDC_OSDEN |			// Use OSD mode
		LCD_OSDC_ALPHAEN          | 		// enable alpha
		LCD_OSDC_ALPHAMD          |	    	// alpha mode: 0, alpha register; 1, each pixel
		LCD_OSDC_F0EN	          |
		LCD_OSDC_F1EN,						// enable Foreground1
		.osd_ctrl = 0,						// disable ipu,
	 	.rgb_ctrl = 0,//LCD_RGBC_YCC, 		// enable RGB => YUV
		.bgcolor = 0x008080, 				// set background color Black
		.colorkey0 = 0,						// disable colorkey
		.colorkey1 = 0,						// disable colorkey
		.alpha = 0xff,						// alpha value
		.ipu_restart = 0x80000100,			// ipu restart
		.fg_change = FG_CHANGE_ALL, 		// change all initially
		.foreground0 = {16, 0, 0, 0, 0}, 	// bpp, x, y, w, h
		.foreground1 = {16, (TVE_WIDTH_PAL - DISP_WIDTH_PAL) / 2, (TVE_HEIGHT_PAL - DISP_HEIGHT_PAL) / 2, DISP_WIDTH_PAL, DISP_HEIGHT_PAL}, 		// bpp, x, y, w, h
	}
};

LCD_PARAM tv_param_ntsc =
{
	.lcd_mode =
	{
		.vendor = (unsigned char *)0,
		.type = "tv",
		TVE_WIDTH_NTSC, TVE_HEIGHT_NTSC, TVE_FREQ_NTSC, 0, 0, 0, 0, 0, 0,
		.mode_bitmask = (unsigned int *)0,
		.cfg = LCDCFG_TVEN | /*LCDCFG_RECOVER | */LCDCFG_NEWDES | MODE_CCIR656_INT,
		.ctrl = LCD_CTRL_BST_16,
	},
	.osd_mode =
	{
		.osd_cfg = LCD_OSDC_OSDEN	| 		// Use OSD mode
		LCD_OSDC_ALPHAEN			|		// enable alpha
		LCD_OSDC_ALPHAMD			|		// alpha mode: 0, alpha register; 1, each pixel
		LCD_OSDC_F0EN	            |
		LCD_OSDC_F1EN,						// enable Foreground1
		.osd_ctrl = 0,						// disable ipu,
		.rgb_ctrl = 0,//LCD_RGBC_YCC,
		.bgcolor = 0x008080, 				// set background color Black
		.colorkey0 = 0,						// disable colorkey
		.colorkey1 = 0,						// disable colorkey
		.alpha = 0xff,						// alpha value
		.ipu_restart = 0x80000100,			// ipu restart
		.fg_change = FG_CHANGE_ALL, 		// change all initially
		.foreground0 = {16, 0, 0, 0, 0}, 	// bpp, x, y, w, h
		.foreground1 = {16, (TVE_WIDTH_NTSC - DISP_WIDTH_NTSC) / 2, (TVE_HEIGHT_NTSC - DISP_HEIGHT_NTSC) / 2, DISP_WIDTH_NTSC, DISP_HEIGHT_NTSC}, // bpp, x, y, w, h
	}
};



