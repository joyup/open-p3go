/********************** BEGIN LICENSE BLOCK ************************************
 *
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
 *  Create:   2008-10-23, by jbyu
 *
 *******************************************************************************
 */

#ifndef __LCD_APP_H__
#define __LCD_APP_H__

typedef struct __OSD_RECT__
{
	int x;
	int y;
	int w;
	int h;
}OSD_RECT, *POSD_RECT;

typedef struct __OSD_SRC__
{
	void *source;
	int w;
	int h;
}OSD_SRC, *POSD_SRC;


// Function prototype
int lcd_init(void);
int lcd_deinit(void);
int lcd_open(void);
int lcd_close(unsigned int type);
int lcd_ioctl(void *buff, unsigned int cmd);
int lcd_poweroff(void);
int lcd_poweron(void);
int osd_open(POSD_SRC psrc, POSD_RECT prc_src, POSD_RECT prc_dst);
int osd_close(void);
void set_video_output(int mode);

#define LCD_QUICK_CLOSE				0x00000009
#define LCD_NORMAL_CLOSE			0x0000000a

// ioctrl definition
#define IOCTL_GET_FRAME				0x00000011
#define IOCTL_GET_CFRAME			0x00000012
#define IOCTL_GET_LCD_CFG			0x00000013
#define IOCTL_SET_LCD_CFG			0x00000014
#define IOCTL_GET_OSD_CFG			0x00000015
#define IOCTL_SET_OSD_CFG			0x00000016
#define IOCTL_GET_LCD_BPP			0x00000017
#define IOCTL_GET_LCD_WIDTH			0x00000018
#define IOCTL_GET_LCD_HEIGHT		0x00000019
#define IOCTL_GET_FG0_BPP			0x0000001a
#define IOCTL_GET_FG0_WIDTH			0x0000001b
#define IOCTL_GET_FG0_HEIGHT		0x0000001c
#define IOCTL_GET_FG1_BPP			0x0000001d
#define IOCTL_GET_FG1_WIDTH			0x0000001e
#define IOCTL_GET_FG1_HEIGHT		0x0000001f
#define IOCTL_GET_FG0_INFO			0x00000020
#define IOCTL_GET_FG1_INFO			0x00000021
#define IOCTL_GET_FG0_CUR_WIDTH		0x00000022
#define IOCTL_GET_FG0_CUR_HEIGHT	0x00000023

// using ipu
//#define IOCTL_START_IPU				0x00000024
#define IOCTL_DMA1_TO_IPU			0x00000025
#define IOCTL_IPU_TO_DMA1			0x00000026
#define IOCTL_LCD_RESIZE			0x00000027
#define IOCTL_LCD_RESIZE_ALL		0x00000028
#define IOCTL_SET_IPU				0x00000029
#define IOCTL_PENDING_IPU_END		0x0000002a
#define IOCTL_START_IPU				0x0000002b

// TV module
#define IOCTL_TV_CHANGE_CLOCK		0x00000041
#define	IOCTL_TV_SET_NTSC_PARAM		0x00000042
#define IOCTL_TV_SET_PAL_PARAM		0x00000043
#define	IOCTL_TV_GET_NTSC_PARAM		0x00000044
#define IOCTL_TV_GET_PAL_PARAM		0x00000045
#define IOCTL_TV_SVIDEO_TO_CVBS		0x00000046
#define IOCTL_TV_CVBS_TO_SVIDEO		0x00000047

// LCD<->TVE
#define IOCTL_LCD_TO_TVE_NTSC  		0x00000052
#define IOCTL_LCD_TO_TVE_PAL   		0x00000053
#define IOCTL_TVE_TO_LCD	   		0x00000054
#define IOCTL_TVE_TO_LCD_VIDEO		0x00000055
#define IOCTL_LCD_TO_TV		   		0x00000056
#define IOCTL_TO_RGBLCD		   		0x00000057
#define IOCTL_PAL_TO_NTSC	   		0x00000058
#define IOCTL_NTSC_TO_PAL	   		0x00000059
#define IOCTL_GET_FRAME2	   		0x00000060
#define IOCTL_SVIDEO_TO_CVBS   		0x00000061
#define IOCTL_CVBS_TO_SVIDEO   		0x00000062
#define IOCTL_TVECR_SWRST	   		0x00000063
#define IOCTL_CLOSE_TVE		   		0x00000064

#define IOCTL_SYNC_LCDFLUSH     0x00000065

#define IOCTL_SYNC_VPSTOPFLUSH      0x00000066
#define IOCTL_SYNC_VPIMAGEFLUSH     0x00000067
#define IOCTL_GET_LCD_TYPE          0x00000068 //0: lcd 1: slcd
#define IOCTL_LCD_CLEARBACKLIGHT    0x00000069 
#define IOCTL_LCD_RESUMEBACKLIGHT   0x00000070

#endif // __LCD_APP_H__








