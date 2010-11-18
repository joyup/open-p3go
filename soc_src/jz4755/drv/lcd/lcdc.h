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
 *  Author:  <jbyu@ingenic.cn>
 *
 *	Create: 2008-09-26, by jbyu
 *
 *******************************************************************************
 */

#ifndef __LCDC_H__
#define __LCDC_H__

#include <jz4755.h>

//#undef printf

#define PANEL_MODE_LCD_PANEL		0
#define PANEL_MODE_TVE_PAL			IOCTL_LCD_TO_TVE_PAL
#define PANEL_MODE_TVE_NTSC			IOCTL_LCD_TO_TVE_NTSC

//#define NR_PALETTE	256
#define NR_PALETTE	256

typedef struct jzfb_info
{
	unsigned char *cpal;						// Cacheable Palette Buffer
	unsigned char *pal;							// Non-cacheable Palette Buffer
	unsigned char *cframe;						// Cacheable Frame Buffer
	unsigned char *frame;						// Non-cacheable Frame Buffer

	struct
	{
		unsigned char red;
		unsigned char green;
		unsigned char blue;
	} palette[NR_PALETTE];
}JZFB_INFO, *PJZFB_INFO;

int jzlcd_init(void);

#undef LCD_UNCACHED
#define LCD_UNCACHED(addr)	((unsigned int)(addr) | 0xa0000000)
#define STATIC_MALLOC

#endif /* __LCDC_H__ */
