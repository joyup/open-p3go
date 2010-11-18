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
 *  Create: 2008-09-26, by jbyu
 *
 *******************************************************************************
 */

#include <bsp.h>
#include <jz4755.h>
#include <dm.h>
#include <function.h>
#include <os_api.h>
#include <ipu_app.h>
#include <lcd_app.h>
#include "lcdc.h"

//#define DEBUG_MODE
//#define TEST_FOR_FPGA
extern LCD_PARAM	lcd_param;
extern LCD_PARAM	tv_param_ntsc;
extern LCD_PARAM	tv_param_pal;
static unsigned int bTvShowing = 0;

PLCD_MODE	plcd_mode = (PLCD_MODE)(&lcd_param.lcd_mode);
POSD_MODE	posd_mode = (POSD_MODE)(&lcd_param.osd_mode);
static JZFB_INFO jzfb;
static LCD_DESCRIPTOR_NEW lcd_palette_desc __attribute__ ((aligned (32)));
static PLCD_DESCRIPTOR_NEW lcd_frame_desc2;
static PLCD_DESCRIPTOR_NEW fg0_frame_desc;
static PLCD_DESCRIPTOR_NEW fg1_frame_desc;
static PLCD_DESCRIPTOR_NEW pCurDesc = 0;
static unsigned int needroom;
volatile static unsigned int bIsDirect = 0;
volatile static unsigned int outMode = 0;
static int bOsdOpen = 0;
static OSD_SRC save_src = {0};
static OSD_RECT save_rc_src = {0};
static OSD_RECT save_rc_dst = {0};
static int bSysBusy = 0;

#if DRIVER_ONLY
#define FRAME_BUFF_ADDRESS		(0x82000000)
#endif

static void foreground0_reset(void);

#ifdef DEBUG_MODE
void print_reg(void)
{
	printf("####### LCD registers #######\r\n");
	printf("A_LCD_LCDCFG[%#x]\r\n", INREG32(A_LCD_LCDCFG));
	printf("A_LCD_LCDSTATE[%#x]\r\n", INREG32(A_LCD_LCDSTATE));
	printf("A_LCD_LCDCTRL[%#x]\r\n", INREG32(A_LCD_LCDCTRL));
	printf("A_LCD_LCDVSYNC[%#x]\r\n", INREG32(A_LCD_LCDVSYNC));
	printf("A_LCD_LCDHSYNC[%#x]\r\n", INREG32(A_LCD_LCDHSYNC));
	printf("A_LCD_LCDVAT[%#x]\r\n", INREG32(A_LCD_LCDVAT));
	printf("A_LCD_LCDDAH[%#x]\r\n", INREG32(A_LCD_LCDDAH));
	printf("A_LCD_LCDDAV[%#x]\r\n", INREG32(A_LCD_LCDDAV));
	printf("####### DMA0 registers #######\r\n");
	printf("A_LCD_LCDDA0[%#x]\r\n", INREG32(A_LCD_LCDDA0));
	printf("A_LCD_LCDSA0[%#x]\r\n", INREG32(A_LCD_LCDSA0));
	printf("A_LCD_LCDFID0[%#x]\r\n", INREG32(A_LCD_LCDFID0));
	printf("A_LCD_LCDCMD0[%#x]\r\n", INREG32(A_LCD_LCDCMD0));
	printf("A_LCD_LCDOFFS0[%#x]\r\n", INREG32(A_LCD_LCDOFFS0));
	printf("A_LCD_LCDPW0[%#x]\r\n", INREG32(A_LCD_LCDPW0));
	printf("A_LCD_LCDCNUM0[%#x]\r\n", INREG32(A_LCD_LCDCNUM0));
	printf("A_LCD_LCDDESSIZE0[%#x]\r\n", INREG32(A_LCD_LCDDESSIZE0));
	printf("####### DMA1 registers #######\r\n");
	printf("A_LCD_LCDDA1[%#x]\r\n", INREG32(A_LCD_LCDDA1));
	printf("A_LCD_LCDSA1[%#x]\r\n", INREG32(A_LCD_LCDSA1));
	printf("A_LCD_LCDFID1[%#x]\r\n", INREG32(A_LCD_LCDFID1));
	printf("A_LCD_LCDCMD1[%#x]\r\n", INREG32(A_LCD_LCDCMD1));
	printf("A_LCD_LCDOFFS1[%#x]\r\n", INREG32(A_LCD_LCDOFFS1));
	printf("A_LCD_LCDPW1[%#x]\r\n", INREG32(A_LCD_LCDPW1));
	printf("A_LCD_LCDCNUM1[%#x]\r\n", INREG32(A_LCD_LCDCNUM1));
	printf("A_LCD_LCDDESSIZE1[%#x]\r\n", INREG32(A_LCD_LCDDESSIZE1));
	printf("####### OSD registers #######\r\n");
	printf("LCD_LCDOSDC[%#x]\r\n", INREG16(A_LCD_LCDOSDC));
	printf("LCD_LCDOSDCTRL[%#x]\r\n", INREG16(A_LCD_LCDOSDCTRL));
	printf("LCD_LCDOSDS[%#x]\r\n", INREG16(A_LCD_LCDOSDS));
	printf("A_LCD_LCDBGC[%#x]\r\n", INREG32(A_LCD_LCDBGC));
	printf("A_LCD_LCDKEY0[%#x]\r\n", INREG32(A_LCD_LCDKEY0));
	printf("A_LCD_LCDKEY1[%#x]\r\n", INREG32(A_LCD_LCDKEY1));
	printf("A_LCD_LCDALPHA[%#x]\r\n", INREG8(A_LCD_LCDALPHA));
	printf("A_LCD_LCDIPUR[%#x]\r\n", INREG32(A_LCD_LCDIPUR));
	printf("A_LCD_LCDXYP0[%#x]\r\n", INREG32(A_LCD_LCDXYP0));
	printf("A_LCD_LCDXYP1[%#x]\r\n", INREG32(A_LCD_LCDXYP1));
	printf("A_LCD_LCDSIZE0[%#x]\r\n", INREG32(A_LCD_LCDSIZE0));
	printf("A_LCD_LCDSIZE1[%#x]\r\n", INREG32(A_LCD_LCDSIZE1));
	printf("A_LCD_LCDRGBC[%#x]\r\n", INREG32(A_LCD_LCDRGBC));
	printf("####### slcd registers #######\r\n");
	printf("A_LCD_MCFG[%#x]\r\n", INREG32(A_LCD_MCFG));
	printf("A_LCD_MCTRL[%#x]\r\n", INREG32(A_LCD_MCTRL));
}

void print_lcd_param(void)
{
	printf("lcd vendor:%s\r\n", plcd_mode->vendor);
	printf("lcd type:%s\r\n", plcd_mode->type);
	printf("lcd width:%d\r\n", plcd_mode->width);
	printf("lcd height:%d\r\n", plcd_mode->height);
	printf("lcd bpp:%d\r\n", posd_mode->foreground0.bpp);
	printf("lcd freq:%d\r\n", plcd_mode->freq);
	printf("lcd hsync:%d\r\n", plcd_mode->hsync);
	printf("lcd vsync:%d\r\n", plcd_mode->vsync);
	printf("lcd elw:%d\r\n", plcd_mode->elw);
	printf("lcd blw:%d\r\n", plcd_mode->blw);
	printf("lcd efw:%d\r\n", plcd_mode->efw);
	printf("lcd bfw:%d\r\n", plcd_mode->bfw);
	printf("lcd cfg:%d\r\n", plcd_mode->cfg);
}

void print_fb_buff(void)
{
	printf("cpal:%#x\r\n", jzfb.cpal);
	printf("pal:%#x\r\n", jzfb.pal);
	printf("cframe:%#x\r\n", jzfb.cframe);
	printf("frame:%#x\r\n", jzfb.frame);
}
#endif

static void clear_screen(unsigned char *buf, int size, int color)
{
	int i;
	for (i = 0; i < size / 4; i++)
	{
		*((unsigned int *)buf) = color;
		buf += 4;
	}
}

//
// common lcd parameter function(without osd)
//
unsigned int lcd_get_bpp(void)
{
	if (posd_mode->foreground1.bpp > 16)
	{
		return (32);
	}
	return (posd_mode->foreground1.bpp);
}

unsigned int lcd_get_width(void)
{
	return (posd_mode->foreground1.w);
}

unsigned int lcd_get_height(void)
{
	return (posd_mode->foreground1.h);
}

unsigned int get_disp_width(int mode)
{
	switch (mode)
	{
		case 0:
		case 3:
			return (lcd_param.osd_mode.foreground1.w);
			break;
		case 1:
			return (tv_param_pal.osd_mode.foreground1.w);
			break;
		case 2:
			return (tv_param_ntsc.osd_mode.foreground1.w);
			break;
		default:
			printf("%s, unsurported mode :%d\n", __FUNCTION__, mode);
			break;
	}
}

unsigned int get_disp_height(int mode)
{
	switch (mode)
	{
		case 0:
		case 3:
			return (lcd_param.osd_mode.foreground1.h);
			break;
		case 1:
			return (tv_param_pal.osd_mode.foreground1.h);
			break;
		case 2:
			return (tv_param_ntsc.osd_mode.foreground1.h);
			break;
		default:
			printf("%s, unsurported mode :%d\n", __FUNCTION__, mode);
			break;
	}
}

unsigned char* lcd_get_frame(void)
{
	return (jzfb.frame);
}

unsigned char* lcd_get_cframe(void)
{
	return (jzfb.cframe);
}

unsigned char* lcd_get_change_frame(void)
{
	return (0);
}

unsigned char* lcd_get_change_cframe(void)
{
	return (0);
}

unsigned char* lcd_get_change_phyframe(void)
{
	return (0);
}
unsigned char * lcd_get_using_phyframe(void)
{
	return (0);
}
unsigned char* lcd_get_current_frame(void)
{
	return (0);
}

unsigned char* lcd_get_current_cframe(void)
{
	return (0);
}

unsigned char* lcd_get_current_phyframe(void)
{
	return (0);
}

void lcd_change_frame(void)
{
}

void lcd_clean_frame_all()
{
	clear_screen(jzfb.cframe, lcd_get_width() * lcd_get_height() * lcd_get_bpp() / 8, 0);
}

void lcd_flush_frame_all()
{
	__dcache_writeback_all();
}

static int jzfb_setcolreg(unsigned int regno, unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned short *ptr, ctmp;

	if (regno >= NR_PALETTE)
	{
		return (1);
	}

	red		&= 0xff;
	green	&= 0xff;
	blue	&= 0xff;

	jzfb.palette[regno].red		= red ;
	jzfb.palette[regno].green	= green;
	jzfb.palette[regno].blue	= blue;

	if (posd_mode->foreground0.bpp <= 8)
	{
		if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_SINGLE) ||
		    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
		{
			ctmp = (77L * red + 150L * green + 29L * blue) >> 8;
			ctmp = ((ctmp >> 3) << 11) | ((ctmp >> 2) << 5) |
				(ctmp >> 3);
		}
		else
		{
			// RGB 565
			if (((red >> 3) == 0) && ((red >> 2) != 0))
			{
				red = 1 << 3;
			}

			if (((blue >> 3) == 0) && ((blue >> 2) != 0))
			{
				blue = 1 << 3;
			}

			ctmp = ((red >> 3) << 11)
				| ((green >> 2) << 5) | (blue >> 3);
		}

		ptr = (unsigned short *)jzfb.pal;
		ptr[regno] = ctmp;

		OUTREG32(A_LCD_LCDDA0, PHYS(&lcd_palette_desc));
	}
	else
	{
		printf("No palette used.\n");
	}

	return (0);
}

#ifdef FRAME_BUFF_ADDRESS
static unsigned char *lcd_heap = (unsigned char *)FRAME_BUFF_ADDRESS;
static LCD_DESCRIPTOR_NEW *lcd_frame_desc2_room;
#else
#ifdef STATIC_MALLOC
static unsigned char lcd_heap[4096 + (480 * 272 * 4 + 4095) / 4096 * 4096] __attribute__((aligned(4096))); // 4Kb align
static LCD_DESCRIPTOR_NEW lcd_frame_desc2_room[4] __attribute__ ((aligned (32)));
#endif
#endif

//
// Map screen memory
//
static int fb_malloc(void)
{
	unsigned int i;
	unsigned int bpp;

	if (posd_mode->foreground1.bpp == 15)
	{
		bpp = 16;
	}

	if(posd_mode->foreground1.bpp > 16)
	{
		bpp = 32;
	}
	else
	{
		bpp = posd_mode->foreground1.bpp;
	}

	//needroom = ((plcd_mode->width * t + 7) >> 3) * plcd_mode->height;
	needroom = (((posd_mode->foreground1.w * bpp + 7) >> 3) * posd_mode->foreground1.h + 63) & ~63;
	printf("1frame buff addr = %x %d\n", lcd_heap, needroom);
#ifdef FRAME_BUFF_ADDRESS
	jzfb.cpal = (unsigned char*) lcd_heap;

	jzfb.cframe = (unsigned char *)((unsigned int)lcd_heap);
	printf("2frame buff addr = %x %d\n", jzfb.cframe, needroom );
	// lcd when 3buf is 16 bit
#ifdef FRAME_BUFF_DECS
	lcd_frame_desc2_room = (LCD_DESCRIPTOR_NEW *)FRAME_BUFF_DECS;
#else
	lcd_frame_desc2_room =  (LCD_DESCRIPTOR_NEW *)((((unsigned int)lcd_heap + (unsigned int)needroom) + 63) & ~63);
#endif
#else
	jzfb.cpal = (unsigned char *)(((unsigned int)lcd_heap) & ~0xfff);
	jzfb.cframe = (unsigned char *)((unsigned int)jzfb.cpal + 0x1000);
#endif

	jzfb.pal = (unsigned char *)LCD_UNCACHED(jzfb.cpal);
	jzfb.frame = (unsigned char *)LCD_UNCACHED(jzfb.cframe);

	memset(jzfb.cpal, 0, 512);

	return (0);
}

//
// This function used during osd display, to prevent the osd layer
// extend the panel edge
//
static void foreground_param_check(POSD_FG posd_fg, unsigned int fg_num)
{
	unsigned int width = plcd_mode->width;
	unsigned int height = plcd_mode->height;
	unsigned int bpp;

	// foreground width height x and y check
	posd_fg->x = (posd_fg->x >= width) ? 0 : posd_fg->x;
	posd_fg->y = (posd_fg->y >= width) ? 0 : posd_fg->y;
	posd_fg->w = (posd_fg->w < width) ? posd_fg->w : width;
	posd_fg->h = (posd_fg->h < height) ? posd_fg->h : height;
	posd_fg->x = (posd_fg->x + posd_fg->w > width) ? (width - posd_fg->w) / 2 : posd_fg->x;
	posd_fg->y = (posd_fg->y + posd_fg->h > height) ? (height - posd_fg->h) / 2 : posd_fg->y;

	if (fg_num == 0)
	{
		bpp = posd_mode->foreground0.bpp;
	}
	else
	{
		bpp = posd_mode->foreground1.bpp;
	}

	if (bpp == 15)
	{
		posd_fg->bpp = 16;
	}
	else if (bpp > 16)
	{
		posd_fg->bpp = 32;
	}
	else
	{
		posd_fg->bpp = bpp;
	}

}


static void lcd_descriptor_init(void)
{
	int i;
	unsigned int pal_size;
	unsigned int frm_size1;
	unsigned char dual_panel = 0;
	LCD_DESCRIPTOR_NEW *pal_desc, *frame_desc0, *frame_desc1;
	lcd_frame_desc2 = (LCD_DESCRIPTOR_NEW *)LCD_UNCACHED(lcd_frame_desc2_room);
	OSD_FG osd_fg0;
	OSD_FG osd_fg1;

	pal_desc	= &lcd_palette_desc;
	frame_desc0	= fg0_frame_desc = &lcd_frame_desc2[0];
	frame_desc1	= fg1_frame_desc = &lcd_frame_desc2[4];

	memcpy(&osd_fg0, &posd_mode->foreground0, sizeof(OSD_FG));
	memcpy(&osd_fg1, &posd_mode->foreground1, sizeof(OSD_FG));
	foreground_param_check(&osd_fg0, 0);
	foreground_param_check(&osd_fg1, 1);

	frm_size1 = (osd_fg1.w * osd_fg1.h * osd_fg1.bpp) >> 3;


	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
	{
		dual_panel = 1;
		frm_size1 >>= 1;
	}

	frm_size1 /= 4;

	switch (osd_fg0.bpp)
	{
		case 1:
			pal_size = 4;
			break;
		case 2:
			pal_size = 8;
			break;
		case 4:
			pal_size = 32;
			break;
		case 8:
		default:
			pal_size = 512;
			break;
	}

	pal_size /= 4;

	// Palette Descriptor
	pal_desc->NEXTDESC_PA	= PHYS(frame_desc0);
	pal_desc->DATA_PA		= PHYS(jzfb.pal);
	pal_desc->FRAME_ID		= 0xdeadbeaf;
	pal_desc->COMMAND		= pal_size | LCD_CMD_PAL; // Palette Descriptor

	//if (!(dual_panel))
	//	return;

  	// Frame Descriptor 0
	frame_desc0->NEXTDESC_PA	= PHYS(frame_desc0);
	frame_desc0->DATA_PA		= PHYS(jzfb.frame);
	frame_desc0->FRAME_ID		= 0xbeafbeae;
	frame_desc0->COMMAND		= 0;
	frame_desc0->DESC_SIZE	 	= 0;
	frame_desc0->OFFSET	 		= 0;
	frame_desc0->PAGE_WIDTH		= 0;
	memcpy(&fg0_frame_desc[1], frame_desc0, sizeof(LCD_DESCRIPTOR_NEW));

	frame_desc1->NEXTDESC_PA	= PHYS(frame_desc1);
	frame_desc1->DATA_PA		= PHYS(jzfb.frame);
	frame_desc1->FRAME_ID		= 0xdeaddeaf;
  	frame_desc1->COMMAND	 	= frm_size1;
  	frame_desc1->DESC_SIZE 		= (osd_fg1.h << 16) | osd_fg1.w;
  	frame_desc1->OFFSET	 		= 0;
	frame_desc1->PAGE_WIDTH		= 0;
}

static void tv_descriptor_init(void)
{
//  descriptor linker
//  A_LCD_LCDDA0->pdesc000->pdesc001->pdesc000 : fg0
//  or A_LCD_LCDDA0->pdesc010->pdesc011->pdesc010 : fg0
	LCD_DESCRIPTOR_NEW *pdesc000 = &fg0_frame_desc[0];
	LCD_DESCRIPTOR_NEW *pdesc001 = &fg0_frame_desc[1];
	LCD_DESCRIPTOR_NEW *pdesc010 = &fg0_frame_desc[2];
	LCD_DESCRIPTOR_NEW *pdesc011 = &fg0_frame_desc[3];

	memset(fg0_frame_desc, 0, 4 * sizeof(LCD_DESCRIPTOR_NEW));

	pdesc000->NEXTDESC_PA	= PHYS(pdesc001);
	pdesc000->DATA_PA		= PHYS(jzfb.frame);
	pdesc000->FRAME_ID		= 0xde000;

	pdesc001->NEXTDESC_PA	= PHYS(pdesc000);
	pdesc001->DATA_PA		= PHYS(jzfb.frame);
	pdesc001->FRAME_ID		= 0xde001;

	pdesc010->NEXTDESC_PA	= PHYS(pdesc011);
	pdesc010->DATA_PA		= PHYS(jzfb.frame);
	pdesc010->FRAME_ID		= 0xde010;

	pdesc011->NEXTDESC_PA	= PHYS(pdesc010);
	pdesc011->DATA_PA		= PHYS(jzfb.frame);
	pdesc011->FRAME_ID		= 0xde011;
}

static int print_lcd_clock(void)
{
	unsigned int pll_clock = GetCurrentPLLClock(1);
//	unsigned int lcd_clock = pll_clock / (((INREG32(A_CPM_CPCCR) & CPCCR_LDIV_MASK) >> CPCCR_LDIV_BIT) + 1);
	unsigned int pix_clock = pll_clock / ((INREG32(A_CPM_LPCDR) & 0x7ff) + 1);

	printf("pll_clock[%d], pixel clk[%d]\r\n", pll_clock, pix_clock);
}

static int controller_init(void)
{
	unsigned int val = 0, reg;
	unsigned int pclk;
	unsigned int stnH;
	int ret = 0;

	// Setting Control register
	val = plcd_mode->ctrl;

	switch (posd_mode->foreground0.bpp)
	{
		case 1:
			val |= LCD_CTRL_BPP_1;
			break;
		case 2:
			val |= LCD_CTRL_BPP_2;
			break;
		case 4:
			val |= LCD_CTRL_BPP_4;
			break;
		case 8:
			val |= LCD_CTRL_BPP_8;
			break;
		case 15:
			val |= LCD_CTRL_RGB555;
		case 16:
			val |= LCD_CTRL_BPP_16;
			break;
	//	case 18:
	//		val |= LCD_CTRL_BPP_18_24;
	//		break;
		case 17 ... 32:
			val |= LCD_CTRL_BPP_18_24;	// target is 4bytes/pixel
			break;
		default:
			printf("The BPP %d is not supported\n", posd_mode->foreground0.bpp);
			val |= LCD_CTRL_BPP_16;
			break;
	}

	switch (plcd_mode->cfg & MODE_MASK)
	{
		case MODE_STN_MONO_DUAL:
		case MODE_STN_COLOR_DUAL:
		case MODE_STN_MONO_SINGLE:
		case MODE_STN_COLOR_SINGLE:
			switch (posd_mode->foreground0.bpp)
			{
				case 1:
					// val |= LCD_CTRL_PEDN;
				case 2:
					val |= LCD_CTRL_FRC_2;
					break;
				case 4:
					val |= LCD_CTRL_FRC_4;
					break;
				case 8:
				default:
					val |= LCD_CTRL_FRC_16;
					break;
			}
			break;
		default:
			break;
	}

	switch (plcd_mode->cfg & MODE_MASK)
	{
		case MODE_STN_MONO_DUAL:
		case MODE_STN_COLOR_DUAL:
		case MODE_STN_MONO_SINGLE:
		case MODE_STN_COLOR_SINGLE:
			switch (plcd_mode->cfg & STN_DAT_PINMASK)
			{
				#define align2(n) (n)=((((n)+1)>>1)<<1)
				#define align4(n) (n)=((((n)+3)>>2)<<2)
				#define align8(n) (n)=((((n)+7)>>3)<<3)
				case STN_DAT_PIN1:
					// Do not adjust the hori-param value.
					break;
				case STN_DAT_PIN2:
					align2(plcd_mode->hsync);
					align2(plcd_mode->elw);
					align2(plcd_mode->blw);
					break;
				case STN_DAT_PIN4:
					align4(plcd_mode->hsync);
					align4(plcd_mode->elw);
					align4(plcd_mode->blw);
					break;
				case STN_DAT_PIN8:
					align8(plcd_mode->hsync);
					align8(plcd_mode->elw);
					align8(plcd_mode->blw);
					break;
			}
			break;
		default:
			break;
	}

	OUTREG32(A_LCD_LCDCTRL, val);

	switch (plcd_mode->cfg & MODE_MASK)
	{
		case MODE_STN_MONO_DUAL:
		case MODE_STN_COLOR_DUAL:
		case MODE_STN_MONO_SINGLE:
		case MODE_STN_COLOR_SINGLE:
			if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL) ||
			    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL))
			{
				stnH = plcd_mode->height >> 1;
			}
			else
			{
				stnH = plcd_mode->height;
			}

			val = (0 << 16) | plcd_mode->vsync;
			OUTREG32(A_LCD_LCDVSYNC, val);
			val = ((plcd_mode->blw + plcd_mode->width) << 16) |
					(plcd_mode->blw + plcd_mode->width + plcd_mode->hsync);
			OUTREG32(A_LCD_LCDHSYNC, val);

			// Screen setting
			val = ((plcd_mode->blw + plcd_mode->width + plcd_mode->hsync + plcd_mode->elw) << 16) |
					(stnH + plcd_mode->vsync + plcd_mode->bfw + plcd_mode->efw);
			OUTREG32(A_LCD_LCDVAT, val);
			val = (plcd_mode->blw << 16) | (plcd_mode->blw + plcd_mode->width);
			OUTREG32(A_LCD_LCDDAH, val);
			val = (0 << 16) | (stnH);
			OUTREG32(A_LCD_LCDDAV, val);

			// AC BIAs signal
			val = (0 << 16) | (stnH+plcd_mode->vsync+plcd_mode->efw+plcd_mode->bfw);
			OUTREG32(A_LCD_LCDPS, val);
			break;

		case MODE_TFT_GEN:
		case MODE_TFT_SHARP:
		case MODE_TFT_CASIO:
		case MODE_8BIT_SERIAL_TFT:
		case MODE_TFT_SAMSUNG:
		case MODE_CCIR656_INT:
 	  //     case LCDCFG_MODE_SLCD:
			val = (0 << 16) | plcd_mode->vsync;
			OUTREG32(A_LCD_LCDVSYNC, val);
			val = ((plcd_mode->vsync + plcd_mode->bfw) << 16) |
				  (plcd_mode->vsync + plcd_mode->bfw + plcd_mode->height);
			OUTREG32(A_LCD_LCDDAV, val);
			val = (((plcd_mode->blw + plcd_mode->width + plcd_mode->elw + plcd_mode->hsync)) << 16) | (plcd_mode->vsync + plcd_mode->bfw + plcd_mode->height + plcd_mode->efw);
			OUTREG32(A_LCD_LCDVAT, val);
			val = (0 << 16) | plcd_mode->hsync;
			OUTREG32(A_LCD_LCDHSYNC, val);
			val = ((plcd_mode->hsync + plcd_mode->blw) << 16) | (plcd_mode->hsync + plcd_mode->blw + plcd_mode->width);
			OUTREG32(A_LCD_LCDDAH, val);
			break;
	}

	switch (plcd_mode->cfg & MODE_MASK)
	{
		case MODE_TFT_SAMSUNG:
		case MODE_TFT_SHARP:
		case MODE_TFT_CASIO:
			printf("LCD DOES NOT supported.\n");
			break;
	}

	// Configure the LCD panel
	OUTREG32(A_LCD_LCDCFG, plcd_mode->cfg);
	OUTREG8(A_LCD_LCDSTATE, 0);

	// init osd if needed
	if ((posd_mode->osd_cfg & LCD_OSDC_OSDEN) != 0)
	{
		osd_init();
	}

	// Timing setting
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_LCD);

	val = plcd_mode->freq; // frame clk

	pclk = val * (plcd_mode->width + plcd_mode->hsync + plcd_mode->elw + plcd_mode->blw) *
	       (plcd_mode->height + plcd_mode->vsync + plcd_mode->efw + plcd_mode->bfw); // Pixclk

	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_SINGLE) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL))
	{
		pclk = (pclk * 3);
	}

	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_SINGLE) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_SINGLE) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
	{
		pclk = pclk >> ((plcd_mode->cfg & STN_DAT_PINMASK) >> 4);
	}

	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
	{
		pclk >>= 1;
	}

//  Jz4755 no this item
//	val = GetCurrentPLLClock(1) / GetCommonClock(CPM_HDIV);
//	reg = (INREG32(A_CPM_CPCCR) & ~CPCCR_LDIV_MASK);
//	OUTREG32(A_CPM_CPCCR, reg | ((val - 1) << (CPCCR_LDIV_BIT)));

	val = GetCurrentPLLClock(1) / pclk;
	OUTREG32(A_CPM_LPCDR, val - 1);
	SETREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);

	// test for fpga
#ifdef  TEST_FOR_FPGA
	OUTREG32(A_LCD_LCDREV, 1);
#endif

#ifdef DEBUG_MODE
	print_lcd_param();
	print_reg();
#endif

	print_lcd_clock();

	return (ret);
}

static void lcd_interrupt_handler(unsigned int irq)
{
	unsigned int state;

	state = INREG32(A_LCD_LCDSTATE);

	if (state & LCD_STATE_EOF) // End of frame
	{
		CLRREG8(A_LCD_LCDSTATE, LCD_STATE_EOF);
	}

	if (state & LCD_STATE_IFU0)
	{
//		printf("InFiFo0 underrun\n");
		CLRREG8(A_LCD_LCDSTATE, LCD_STATE_IFU0);
	}

	if (state & LCD_STATE_IFU1)
	{
//		printf("InFiFo1 underrun\n");
		CLRREG8(A_LCD_LCDSTATE, LCD_STATE_IFU1);
	}

	if (state & LCD_STATE_OUF)		// Out fifo underrun
	{
		printf("Out FiFo underrun.\n");
		CLRREG8(A_LCD_LCDSTATE, LCD_STATE_OUF);
	}
}

void FrameBufferMalloc(void)
{
	fb_malloc();
}

static void lcd_set_wakeup()
{
}

static void lcd_wakeup()
{
	lcd_board_init(1);
}

// set the LCD DE, PCLK, VSYNC, HSYNC pin in advance
// just for tpo800X480
static void lcd_preinit(void)
{
	OUTREG32(A_GPIO_PXFUNS(3), 0x003c0000);
	OUTREG32(A_GPIO_PXSELC(3), 0x003c0000);
	OUTREG32(A_GPIO_PXPES(3), 0x003c0000);
}

static void lcd_reinit();
//#define PWM_FREQ		(50 * 1000)	// Condigure according to the BKL chip
#define LcdID			(('L' << 24) | ('C' << 16) | ('D' << 16) | (':' << 16))
static void  mng_init_lcd(void);
int jzlcd_init(void)
{
	int err = -1;

	printf("LCD Init!\n");

	PM_AddWakeUp(LcdID, lcd_set_wakeup, lcd_wakeup);

	mng_init_lcd();
/*
	if(GetLcdInit())
	{
		fb_malloc();
		lcd_descriptor_init();
//      Jz4755 no this item
//		unsigned int val = GetCurrentPLLClock(1) / GetCommonClock(CPM_HDIV);
//		unsigned int reg = (INREG32(A_CPM_CPCCR) & ~CPCCR_LDIV_MASK);
//		OUTREG32(A_CPM_CPCCR, reg | ((val - 1) << (CPCCR_LDIV_BIT)));
		lcd_board_init(1);

		__dcache_writeback_all();

  		printf("lcd not use hwinit!\n");
  		return (0);
  	}

	SetLcdInit();
*/

	request_irq(IRQ_LCD, lcd_interrupt_handler, 0);
//	pwm_init(LCD_BACKLIGHT_PWM_CHN, PWM_FREQ, 2);

//	lcd_close_backlight();
	lcd_preinit();
	lcd_reinit();
	lcd_open();

	os_TimeDelay(50);

	lcd_board_init(0);

//	lcd_flush_frame_all();

	os_TimeDelay(30);

	if (lcd_get_backlight() != 0)
	{
		lcd_set_backlight(lcd_get_backlight());
	}

#ifdef DEBUG_MODE
	print_reg();
#endif

	return (0);

failed:
	return (err);
}

static void lcd_reinit()
{
	CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);
	fb_malloc();
	int t, i;

	__dcache_writeback_all();
	lcd_descriptor_init();
	controller_init();
	if (posd_mode->foreground0.bpp <= 8)
	{
		OUTREG32(A_LCD_LCDDA0, PHYS(&lcd_palette_desc));
	}
	else
	{
		OUTREG32(A_LCD_LCDDA0, PHYS(fg0_frame_desc));
	}

	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL)  ||
	    ((posd_mode->osd_cfg & LCD_OSDC_F1EN) != 0))
	{
		OUTREG32(A_LCD_LCDDA1, PHYS(fg1_frame_desc));
	}

	if ((INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN) == 0)
	{
		clear_screen((unsigned char *)jzfb.cframe, needroom, 0);
		lcd_flush_frame_all();
	}

//	SETREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);
}


#if (DM == 1)
int  lcd_poweron(void)
{
	lcd_display_on();
	//__gpio_as_lcd_24bit();
	//restore_gpio_regs2();

}
int  lcd_poweroff(void)
{
	lcd_display_off();
	//save_gpio_regs2();
	//config_gpio_savepower2();
}

int lcd_preconvert(PPLL_OPT pllopt)
{
	unsigned int val = 0;
	unsigned int pclk;
	unsigned int pllclock = pllopt->cpuclock;
	//printf("%d\n",pllopt->cpuclock);
	val = plcd_mode->freq; // frame clk
	pclk = val * (plcd_mode->width + plcd_mode->hsync + plcd_mode->elw + plcd_mode->blw) *
		(plcd_mode->height + plcd_mode->vsync + plcd_mode->efw + plcd_mode->bfw); // Pixclk
	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_SINGLE) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL))
	{
		pclk = (pclk * 3);
	}

	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_SINGLE) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL)   ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_SINGLE)  ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
	{
		pclk = pclk >> ((plcd_mode->cfg & STN_DAT_PINMASK) >> 4);
	}

	if (((plcd_mode->cfg & MODE_MASK) == MODE_STN_COLOR_DUAL) ||
	    ((plcd_mode->cfg & MODE_MASK) == MODE_STN_MONO_DUAL))
	{
		pclk >>= 1;
	}
	val = pllclock / pclk;
	if((val > 0) && (pllclock > OSC_CLOCK))
		val = val - 1;
	//printf("val = %d\n",val);
	return val;
}

int lcd_convert(unsigned int div)
{
	div = (INREG32(A_CPM_LPCDR) & (~0x3ff)) | div;
	OUTREG32(A_CPM_LPCDR, div);
	//printf("lcd div = %d\n",div);
	return 1;
}
static struct dm_jz4740_t lcd_dm={0};
static void  mng_init_lcd(void)
{

	lcd_dm.name = "LCD0";
	lcd_dm.convert = lcd_convert;
	lcd_dm.preconvert = lcd_preconvert;
	dm_register(0, &lcd_dm);
}

void lcd_stop()
{
    lcd_close(LCD_NORMAL_CLOSE);
#ifdef LCD_POWER_PIN
#ifdef LCD_POWER_ON_LEVEL
	int pwrLevel = LCD_POWER_ON_LEVEL ? 0 : 1;
#else
	int pwrLevel = 0;
#endif
	gpio_set_dir(LCD_POWER_PIN, 1, pwrLevel);
#endif
}
#endif

void lcd_start(void)
{
	controller_init();
	if (posd_mode->foreground0.bpp <= 8)
	{
		OUTREG32(A_LCD_LCDDA0, PHYS(&lcd_palette_desc));
	}
	else
	{
		OUTREG32(A_LCD_LCDDA0, PHYS(fg0_frame_desc));
	}
	lcd_open();
	idledelay(50);
}

void lcd_enter_frame(void)
{

}

void lcd_reset_frame(void)
{

}

void lcd_set_buf(void *source)
{
	PLCD_DESCRIPTOR_NEW pdesc10 = &fg1_frame_desc[0];
	PLCD_DESCRIPTOR_NEW pdesc11 = &fg1_frame_desc[1];

	memcpy(pdesc11, pdesc10, sizeof(LCD_DESCRIPTOR_NEW));
	pdesc11->NEXTDESC_PA = PHYS(pdesc11);
	pdesc11->DATA_PA = PHYS(source);
	pdesc10->NEXTDESC_PA = PHYS(pdesc11);
}

void lcd_reset_buf(void)
{
	PLCD_DESCRIPTOR_NEW pdesc10 = &fg1_frame_desc[0];
	PLCD_DESCRIPTOR_NEW pdesc11 = &fg1_frame_desc[1];

	pdesc10->NEXTDESC_PA = PHYS(pdesc10);
	pdesc11->NEXTDESC_PA = PHYS(pdesc10);
}

static OSD_FG old_osd_fg1;
void lcd_enter_window_mode(unsigned int w, unsigned int h)
{
	if ((posd_mode->osd_cfg & LCD_OSDC_OSDEN) == 0)
	{
		return;
	}

	if (old_osd_fg1.bpp == 0)
	{
		memcpy(&old_osd_fg1, &posd_mode->foreground1, sizeof(OSD_FG));
	}
	posd_mode->foreground1.w = w;
	posd_mode->foreground1.h = h;
	posd_mode->foreground1.x = (plcd_mode->width - posd_mode->foreground1.w) / 2;
	posd_mode->foreground1.y = (plcd_mode->height - posd_mode->foreground1.h) / 2;

	lcd_close_backlight();
	lcd_reinit();
	lcd_open();

	os_TimeDelay(30);

	if (lcd_get_backlight() != 0)
	{
		lcd_set_backlight(lcd_get_backlight());
	}
}

void lcd_leave_window_mode(void)
{
	if ((posd_mode->osd_cfg & LCD_OSDC_OSDEN) == 0)
	{
		return;
	}

	memcpy(&posd_mode->foreground1, &old_osd_fg1, sizeof(OSD_FG));

	lcd_close_backlight();
	lcd_reinit();
	lcd_open();

	os_TimeDelay(30);

	if (lcd_get_backlight() != 0)
	{
		lcd_set_backlight(lcd_get_backlight());
	}
}

#define MAX_SIZE		(1024 * 720)
static void lcd_to_tv(unsigned int tv_mode, img_param_t *img)
{
	OSD_FG fg1;

	if (tv_mode == PANEL_MODE_TVE_PAL)
	{
		plcd_mode = (PLCD_MODE)(&tv_param_pal.lcd_mode);
		posd_mode = (POSD_MODE)(&tv_param_pal.osd_mode);
	}
	else
	{
		plcd_mode = (PLCD_MODE)(&tv_param_ntsc.lcd_mode);
		posd_mode = (POSD_MODE)(&tv_param_ntsc.osd_mode);
	}

	__lcd_as_gpio();

	if (bIsDirect)
	{
		while ((INREG32(A_IPU_STATUS) & OUT_END) == 0);
		CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);
	}

	memcpy(&fg1, &posd_mode->foreground1, sizeof(OSD_FG));
// Fixed the odd and even line order
	unsigned int in_h = img->in_height;
	unsigned int out_h = img->out_height;
	unsigned int size = img->in_height * img->in_width;
	if (size > MAX_SIZE)
	{
		img->ipu_ctrl = LCDC_SEL | ADDR_SEL | FM_IRQ_EN; //| DISP_SEL;
		img->ipu_ctrl &= ~FIELD_SEL; // top field first
		img->out_height /= 2;
		img->in_height /= 2;
		img->stride->y *= 2;
		img->stride->u *= 2;
		img->stride->v *= 2;
	}
	else
	{
		img->ipu_ctrl = LCDC_SEL | ADDR_SEL | DISP_SEL;
		img->ipu_ctrl &= ~FIELD_SEL; // top field first
	}

	// IPU setting
	ipu_init(img);

	if (size > MAX_SIZE)
	{
		img->out_height *= 2;
	}

	posd_mode->foreground1.w = img->out_width;
	posd_mode->foreground1.h = img->out_height;
	posd_mode->foreground1.x = ((plcd_mode->width - posd_mode->foreground1.w) / 2) & ~1;
	posd_mode->foreground1.y = ((plcd_mode->height - posd_mode->foreground1.h) / 2) & ~1; // keep top field first
	img->in_height = in_h;
	img->out_height = out_h;

	jz4755tve_init(tv_mode);

	foreground0_reset();
	lcd_reinit();
	jz4755tve_ioctl((void *)0, IOCTL_TV_CHANGE_CLOCK);
	tv_descriptor_init();

	OUTREG32(A_LCD_LCDDA0, PHYS(fg0_frame_desc));
	OUTREG32(A_LCD_LCDDA1, PHYS(fg1_frame_desc));
	SETREG32(A_LCD_LCDOSDCTRL, LCD_OSDCTRL_IPU);
	OUTREG32(A_LCD_LCDSTATE, 0); // clear the lcd state register

	bIsDirect = 1;

	ipu_open();
	idledelay(10);
	lcd_open();
	jz4755tve_open();

	bTvShowing = 1;

	bSysBusy = 0;

	if (save_src.w && save_src.h)
	{
		osd_open(&save_src, &save_rc_src, &save_rc_dst);
		memset(&save_src, 0, sizeof(OSD_SRC));
	}

	memcpy(&posd_mode->foreground1, &fg1, sizeof(OSD_FG));

	print_lcd_clock();
}

static void tv_to_lcd(void)
{
	plcd_mode = (PLCD_MODE)(&lcd_param.lcd_mode);
	posd_mode = (POSD_MODE)(&lcd_param.osd_mode);

	while ((INREG32(A_IPU_STATUS) & OUT_END) == 0);

	jz4755tve_close();
	jz4755tve_deinit();

	ipu_close();
	ipu_deinit();

	CLRREG32(A_CPM_LPCDR, LPCDR_LTCS);

	foreground0_reset();
	lcd_reinit();
	lcd_open();

	bTvShowing = 0;
	bSysBusy   = 0;
	bIsDirect = 0;

}

int lcd_open(void)
{
	unsigned int enable = 1, i;

	if(!(INREG32(A_LCD_LCDCTRL) & LCD_CTRL_ENA))
		SETREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);

	for (i = 0; i < 10; i++)
	{
		if (!(INREG32(A_LCD_LCDCTRL) & LCD_CTRL_ENA))
		{
			enable = 0;
			break;
		}
	}

	if (enable == 0)
	{
		printf("Put CPU into hibernate mode.\n");
		serial_waitfinish();

		RTC_OUTREG32(A_RTC_RTCCR, RTCCR_AIE | RTCCR_AE | RTCCR_RTCEN);
		RTC_OUTREG32(A_RTC_HWRSR, 0);
		RTC_OUTREG32(A_RTC_HSPR, 0x12345678);
		RTC_OUTREG32(A_RTC_RTCSAR, RTC_INREG32(A_RTC_RTCSR) + 2);
		RTC_SETREG32(A_RTC_HWCR, HWCR_EALM);
		RTC_SETREG32(A_RTC_HRCR, 0x0fe0);
		RTC_SETREG32(A_RTC_HWFCR, (0x00FF << 4));
	   	RTC_SETREG32(A_RTC_HCR, HCR_PD);
	}
	else
	{
		if (plcd_mode->cfg & LCDCFG_RECOVER)
		{
			CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);
        	SETREG32(A_LCD_LCDCFG, LCDCFG_RECOVER);
        	SETREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);
        }
		printf("get the ENA BIT 0x%x!!\n", INREG32(A_LCD_LCDCTRL));
	}

	return (0);
}

int lcd_close(unsigned int type)
{
	switch (type)
	{
		case LCD_NORMAL_CLOSE:
			if(INREG32(A_LCD_LCDCTRL) & LCD_CTRL_ENA)
			{
				SETREG32(A_LCD_LCDCTRL, LCD_CTRL_DIS);
				while ((INREG8(A_LCD_LCDSTATE) & LCD_STATE_LDD) == 0);
				CLRREG8(A_LCD_LCDSTATE, LCD_STATE_LDD);
				CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_DIS);
		  }
			break;

		case LCD_QUICK_CLOSE:
			CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);
			while ((INREG8(A_LCD_LCDSTATE) & LCD_STATE_QD) == 0);
			CLRREG8(A_LCD_LCDSTATE, LCD_STATE_QD);
			break;

		default:
			printf("not support close type!\r\n");
			return (-1);
			break;
	}

	return (0);
}

int lcd_init(void)
{
	if ((jzlcd_init() < 0))
	{
		return (-1);
	}

	return (0);
}

int lcd_deinit(void)
{
	free_irq(IRQ_LCD);

	__lcd_as_gpio();
}

#define CHANGE_MODE_SIZE 		(1 << 1)
#define CHANGE_MODE_POS 		(1 << 2)
//#define DEBUG_OSD_FG0
int osd_open(POSD_SRC psrc, POSD_RECT prc_src, POSD_RECT prc_dst)
{
	unsigned int frm_size0;
	unsigned int frm_size1;
	unsigned int offset, page_width;
	unsigned int byte_per_bpp;
	unsigned int bChangeMode = 0;
	unsigned char *pbuf;

	POSD_FG posd_fg = &posd_mode->foreground0;
	PLCD_DESCRIPTOR_NEW pdesc000;
	PLCD_DESCRIPTOR_NEW pdesc001;
	PLCD_DESCRIPTOR_NEW pdesc010;
	PLCD_DESCRIPTOR_NEW pdesc011;

	if (bSysBusy)
	{
		memcpy(&save_src, psrc, sizeof(OSD_SRC));
		memcpy(&save_rc_dst, prc_dst, sizeof(OSD_RECT));
		memcpy(&save_rc_src, prc_src, sizeof(OSD_RECT));
		return;
	}

	if ((prc_src == 0) || (prc_dst == 0) || (psrc == 0))
	{
		return (-1);
	}

#ifdef DEBUG_OSD_FG0
	printf("rc_dst x[%d], y[%d], w[%d], h[%d]\n", prc_dst->x, prc_dst->y, prc_dst->w, prc_dst->h);
	printf("rc_src x[%d], y[%d], w[%d], h[%d]\n", prc_src->x, prc_src->y, prc_src->w, prc_src->h);
	printf("src w[%d], h[%d]\n", psrc->w, psrc->h);
#endif

	// foreground0 setting
	prc_dst->y &= ~1;                  // keep foreground0 start form even line
	if ((posd_fg->x != prc_dst->x) || (posd_fg->y != prc_dst->y))
	{
		bChangeMode |= CHANGE_MODE_POS;
	}

	if ((posd_fg->w != prc_dst->w) || (posd_fg->h != prc_dst->h))
	{
		bChangeMode |=  CHANGE_MODE_SIZE;
	}

	if (!bChangeMode)
	{
		return (-1);
	}

	if (pCurDesc == &fg0_frame_desc[2])
	{
		pdesc010 = &fg0_frame_desc[0];
		pdesc011 = &fg0_frame_desc[1];
		pdesc000 = &fg0_frame_desc[2];
		pdesc001 = &fg0_frame_desc[3];
		pCurDesc = &fg0_frame_desc[0];
	}
	else
	{
		pdesc000 = &fg0_frame_desc[0];
		pdesc001 = &fg0_frame_desc[1];
		pdesc010 = &fg0_frame_desc[2];
		pdesc011 = &fg0_frame_desc[3];
		pCurDesc = &fg0_frame_desc[2];
	}

	posd_fg->x = prc_dst->x;
	posd_fg->y = prc_dst->y;
	posd_fg->w = prc_dst->w;
	posd_fg->h = prc_dst->h;
	foreground_param_check(posd_fg, 0);
	byte_per_bpp = posd_fg->bpp / 8;

	if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
	{
		byte_per_bpp *= 2;
		frm_size0 = ((posd_fg->h + 1) / 2) * posd_fg->w * byte_per_bpp / 4;
		frm_size1 = (posd_fg->h / 2) * posd_fg->w * byte_per_bpp / 4;
	}
	else
	{
		frm_size0 = posd_fg->h * posd_fg->w * byte_per_bpp / 4;
	}

	pbuf = (unsigned char *)psrc->source + (psrc->w * prc_src->y  + prc_src->x ) * byte_per_bpp;

#ifdef DEBUG_OSD_FG0
	printf("psrc->source[%#x]\n", psrc->source);
	printf("frm_size[%#x]\n", frm_size0);
	printf("pbuf[%#x]\n", pbuf);
#endif

	if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
	{
		offset = (psrc->w * 2 - prc_dst->w)* byte_per_bpp;
	}
	else
	{
		offset = (psrc->w - prc_dst->w)* byte_per_bpp;
	}

	page_width = posd_fg->w * byte_per_bpp;

#ifdef DEBUG_OSD_FG0
	printf("offset[%#x]\n", offset);
	printf("page_width[%#x]\n", page_width);
#endif

	if ((offset % 4 != 0) || (page_width % 4 != 0))
	{
		printf("dma offset[%d] or page_width[%d] not allign 4 bytes\n", offset, page_width);
		return (-1);
	}

//	unsigned int ret = spin_lock_irqsave();

	if (bChangeMode & CHANGE_MODE_POS)
	{
		while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);

		OUTREG32(A_LCD_LCDXYP0, (posd_fg->y << 16) | (posd_fg->x));
		//OUTREG32(A_LCD_LCDSIZE0, (posd_fg->h << 16) | (posd_fg->w));
		SETREG32(A_LCD_LCDOSDCTRL, LCD_OSDCTRL_CHANGE);
		while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);
	}

 	if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
	{
		if (bChangeMode & CHANGE_MODE_SIZE)
		{
			while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);

			SETREG32(A_LCD_LCDOSDCTRL, LCD_OSDCTRL_CHANGE);
			while ((INREG16(A_LCD_LCDOSDS) & LCD_OSDS_READY) == 0);

			pdesc010->DATA_PA		= PHYS(pbuf);
			pdesc010->COMMAND		= frm_size0;
			pdesc010->OFFSET		= offset >> 2;
			pdesc010->PAGE_WIDTH 	= page_width >> 2;
			pdesc010->DESC_SIZE		= (posd_fg->h << 16) | (posd_fg->w);

			pdesc011->DATA_PA		= PHYS(pbuf + psrc->w * 4);
			pdesc011->COMMAND		= frm_size1 | LCD_CMD_EOFINT;
			pdesc011->OFFSET		= offset >> 2;
			pdesc011->PAGE_WIDTH 	= page_width >> 2;
			pdesc011->DESC_SIZE		= (posd_fg->h << 16) | (posd_fg->w);

			pdesc011->NEXTDESC_PA = PHYS(pdesc010);
			pdesc010->NEXTDESC_PA = PHYS(pdesc011);
			pdesc001->NEXTDESC_PA = PHYS(pdesc010);

			while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);
		}
	}
	else
	{
		if (bChangeMode & CHANGE_MODE_SIZE)
		{
			while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);

			SETREG32(A_LCD_LCDOSDCTRL, LCD_OSDCTRL_CHANGE);
			while ((INREG16(A_LCD_LCDOSDS) & LCD_OSDS_READY) == 0);

			pdesc010->DATA_PA		= PHYS(pbuf);
			pdesc010->COMMAND		= frm_size0 | LCD_CMD_EOFINT;
			pdesc010->OFFSET		= offset >> 2;
			pdesc010->PAGE_WIDTH 	= page_width >> 2;
			pdesc010->DESC_SIZE		= (posd_fg->h << 16) | (posd_fg->w);

			pdesc010->NEXTDESC_PA = PHYS(pdesc010);
			pdesc000->NEXTDESC_PA = PHYS(pdesc010);

			while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);
		}
	}

	bOsdOpen = 1;
//	spin_unlock_irqrestore(ret);
}


int osd_init(void)
{
	OSD_FG osd_fg0;
	OSD_FG osd_fg1;

	memcpy(&osd_fg0, &posd_mode->foreground0, sizeof(OSD_FG));
	memcpy(&osd_fg1, &posd_mode->foreground1, sizeof(OSD_FG));
	foreground_param_check(&osd_fg0, 0);
	foreground_param_check(&osd_fg1, 1);

	if(osd_fg1.bpp > 16)
	{
		posd_mode->osd_ctrl = (posd_mode->osd_ctrl & ~LCD_BPP_MASK) | LCD_CTRL_BPP_18_24;
	}
	else
	{
		posd_mode->osd_ctrl = (posd_mode->osd_ctrl & ~LCD_BPP_MASK) | LCD_CTRL_BPP_16;
	}

	// OSD init
	OUTREG16(A_LCD_LCDOSDC, posd_mode->osd_cfg);
	OUTREG16(A_LCD_LCDOSDCTRL, posd_mode->osd_ctrl);
	OUTREG16(A_LCD_LCDRGBC, posd_mode->rgb_ctrl);
	OUTREG32(A_LCD_LCDBGC, posd_mode->bgcolor);
	OUTREG32(A_LCD_LCDKEY0, posd_mode->colorkey0);
	OUTREG32(A_LCD_LCDKEY1, posd_mode->colorkey1);
	OUTREG8(A_LCD_LCDALPHA, (unsigned char)posd_mode->alpha);
	OUTREG32(A_LCD_LCDIPUR, posd_mode->ipu_restart);
	OUTREG32(A_LCD_LCDXYP0, (posd_mode->foreground0.y << 16) | (posd_mode->foreground0.x << 0));
	OUTREG32(A_LCD_LCDXYP1, (posd_mode->foreground1.y << 16) | (posd_mode->foreground1.x << 0));
	OUTREG32(A_LCD_LCDSIZE0, (posd_mode->foreground0.h << 16) | (posd_mode->foreground0.w << 0));
	OUTREG32(A_LCD_LCDSIZE1, (posd_mode->foreground1.h << 16) | (posd_mode->foreground1.w << 0));
	OUTREG16(A_LCD_LCDOSDS, 0);
}

int osd_close(void)
{
	if (!bOsdOpen)
	{
		return;
	}

	PLCD_DESCRIPTOR_NEW pdesc000;
	PLCD_DESCRIPTOR_NEW pdesc001;
	PLCD_DESCRIPTOR_NEW pdesc010;
	PLCD_DESCRIPTOR_NEW pdesc011;

	bOsdOpen = 0;

	if (pCurDesc == &fg0_frame_desc[2])
	{
		pdesc010 = &fg0_frame_desc[0];
		pdesc011 = &fg0_frame_desc[1];
		pdesc000 = &fg0_frame_desc[2];
		pdesc001 = &fg0_frame_desc[3];
		pCurDesc = &fg0_frame_desc[0];
	}
	else
	{
		pdesc000 = &fg0_frame_desc[0];
		pdesc001 = &fg0_frame_desc[1];
		pdesc010 = &fg0_frame_desc[2];
		pdesc011 = &fg0_frame_desc[3];
		pCurDesc = &fg0_frame_desc[2];
	}


	while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);
	SETREG32(A_LCD_LCDOSDCTRL, LCD_OSDCTRL_CHANGE);
	while ((INREG16(A_LCD_LCDOSDS) & LCD_OSDS_READY) == 0);

	pdesc011->COMMAND		= 0;
	pdesc011->DESC_SIZE		= 0;
	pdesc010->COMMAND		= 0;
	pdesc010->DESC_SIZE		= 0;

 	if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
 	{
		pdesc011->NEXTDESC_PA = PHYS(pdesc010);
		pdesc010->NEXTDESC_PA = PHYS(pdesc011);
		pdesc001->NEXTDESC_PA = PHYS(pdesc010);
	}
	else
	{
		pdesc010->NEXTDESC_PA = PHYS(pdesc010);
		pdesc000->NEXTDESC_PA = PHYS(pdesc010);
	}

	posd_mode->foreground0.w = 0;
	posd_mode->foreground0.h = 0;

	while (INREG32(A_LCD_LCDOSDCTRL) & LCD_OSDCTRL_CHANGE);

	return (0);
}

void osd_pending_eof(void)
{
	if (!bOsdOpen)
		return;

	while ((INREG32(A_LCD_LCDOSDS) & LCD_OSDS_EOF0) == 0);
	CLRREG32(A_LCD_LCDOSDS, LCD_OSDS_EOF0);
}

static void foreground0_reset(void)
{
	posd_mode->foreground0.h = 0;
	posd_mode->foreground0.w = 0;
	posd_mode->foreground0.x = 0;
	posd_mode->foreground0.y = 0;
}

static void ipu_to_dma1(void)
{
	OSD_FG fg1;
	unsigned int ret;

	bOsdOpen = 0;
	outMode = 0;

	if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
	{
		lcd_ioctl((void *)0, IOCTL_TVE_TO_LCD);
		return;
	}

	memcpy(&fg1, &posd_mode->foreground1, sizeof(OSD_FG));
	foreground_param_check(&fg1, 1);

	foreground0_reset();
	lcd_close_backlight();

	lcd_clean_frame_all();
	lcd_flush_frame_all();

	SETREG32(A_LCD_LCDCTRL, LCD_CTRL_DIS);

//	ret = spin_lock_irqsave();
//
	while ((INREG8(A_LCD_LCDSTATE) & LCD_STATE_LDD) == 0);

	lcd_descriptor_init();
	OUTREG32(A_LCD_LCDXYP1, (fg1.y << 16) | (fg1.x << 0));
	OUTREG32(A_LCD_LCDSIZE1, (fg1.h << 16) | (fg1.w << 0));
	OUTREG32(A_LCD_LCDXYP0, 0);
	OUTREG32(A_LCD_LCDSIZE0, 0);
	CLRREG32(A_LCD_LCDOSDCTRL, LCD_OSDCTRL_IPU);
	OUTREG32(A_LCD_LCDDA1, PHYS(fg1_frame_desc));
	OUTREG32(A_LCD_LCDDA0, PHYS(fg0_frame_desc));
//	CLRREG16(A_LCD_LCDOSDC, LCD_OSDC_F0EN);
	OUTREG32(A_LCD_LCDSTATE, 0);

	ipu_close();
	ipu_deinit();

	CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_DIS);
	SETREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);

	bIsDirect = 0;
	bSysBusy = 0;

//	spin_unlock_irqrestore(ret);

	os_TimeDelay(30);

	if (lcd_get_backlight() != 0)
	{
		lcd_set_backlight(lcd_get_backlight());
	}
//	print_reg();
}

#define VGA_640X480		0
#define VGA_800X600		0
#define VGA_1280X720	0
static void dma1_to_ipu(img_param_t *img)
{
	OSD_FG fg1;

	img->ipu_ctrl = LCDC_SEL | ADDR_SEL;

	bOsdOpen = 0;
	pCurDesc = 0;

	lcd_close_backlight();

	if (outMode)
	{
		lcd_to_tv(outMode, img);
		return;
	}

#if (VGA_640X480) // 640 * 480
	img->out_width = 640;
	img->out_height = 480;
	plcd_mode->hsync = 96;
	plcd_mode->vsync = 2;
	plcd_mode->elw = 16;
	plcd_mode->blw = 48;
	plcd_mode->efw = 10;
	plcd_mode->bfw = 33;
	plcd_mode->width = 640;
	plcd_mode->height = 480;
	posd_mode->foreground1.w = 640;
	posd_mode->foreground1.h = 480;
#endif

#if (VGA_800X600) // 800 * 600
	img->out_width = 800;
	img->out_height = 600;
	plcd_mode->hsync = 128;
	plcd_mode->vsync = 4;
	plcd_mode->elw = 40;
	plcd_mode->blw = 88;
	plcd_mode->efw = 0;
	plcd_mode->bfw = 23;
	plcd_mode->width = 800;
	plcd_mode->height = 600;
	posd_mode->foreground1.w = 800;
	posd_mode->foreground1.h = 600;
#endif

#if (VGA_1280X720) // 1280 * 720
	img->out_width = 1280;
	img->out_height = 720;
	plcd_mode->hsync = 152;
	plcd_mode->vsync = 15;
	plcd_mode->freq = 50;
	plcd_mode->elw = 22;
	plcd_mode->blw = 200;
	plcd_mode->efw = 14;
	plcd_mode->bfw = 1;
	plcd_mode->width = 1280;
	plcd_mode->height = 720;
	posd_mode->foreground1.w = 1280;
	posd_mode->foreground1.h = 720;
#endif

// IPU setting
#if (LCD16BPP)
	img->ipu_d_fmt &= ~OUT_FMT_MASK;
	img->ipu_d_fmt |= OUT_FMT_RGB565;
#endif
	ipu_init(img);
	fg1.w = img->out_width;
	fg1.h = img->out_height;
	img->out_x = fg1.x = (plcd_mode->width - fg1.w) / 2;
	img->out_y = fg1.y = (plcd_mode->height - fg1.h) / 2;
	posd_mode->foreground0.w = 0;
	posd_mode->foreground0.h = 0;
	posd_mode->foreground0.x = 0;
	posd_mode->foreground0.y = 0;

	CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);

#if (VGA_640X480 || VGA_800X600 || VGA_1280X720)
	lcd_reinit();
	SETREG32(A_TV_TVECR, TV_TVECR_DAPD);
	CLRREG32(A_TV_TVECR, TV_TVECR_DAPD1 | TV_TVECR_DAPD2 | TV_TVECR_DAPD3);
#endif

	memset(fg0_frame_desc, 0, sizeof(LCD_DESCRIPTOR_NEW));
	fg0_frame_desc->NEXTDESC_PA = PHYS(fg0_frame_desc);
	fg0_frame_desc->DATA_PA = PHYS(jzfb.frame);
	OUTREG32(A_LCD_LCDXYP1, (fg1.y << 16) | (fg1.x << 0));
	OUTREG32(A_LCD_LCDSIZE1, (fg1.h << 16) | (fg1.w << 0));
	OUTREG32(A_LCD_LCDXYP0, 0);
	OUTREG32(A_LCD_LCDSIZE0, 0);
	OUTREG32(A_LCD_LCDDA0, PHYS(fg0_frame_desc));
	SETREG32(A_LCD_LCDOSDCTRL, LCD_OSDCTRL_IPU);
	OUTREG32(A_LCD_LCDSTATE, 0); // clear the lcd state register

	ipu_open();					 // enable ipu_open

	SETREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);			 // enable lcd

	bIsDirect = 1;

	__gpio_as_lcd_24bit();


	idledelay(100);

	bSysBusy = 0;

	if (save_src.w && save_src.h)
	{
		osd_open(&save_src, &save_rc_src, &save_rc_dst);
		memset(&save_src, 0, sizeof(OSD_SRC));
	}

	if (lcd_get_backlight() != 0)
	{
		lcd_set_backlight(lcd_get_backlight());
	}
}

#define TYPE_IPU_DIRECT		1
#define TYPE_IPU_NO_DIRECT	2
static void image_resize(img_param_t *img, unsigned int type)
{
	OSD_FG fg1;
	unsigned int ret;
	unsigned int in_h, out_h;
	unsigned int w = img->out_width;
	unsigned int h = img->out_height;
	
	if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
	{
		while ((INREG32(A_IPU_STATUS) & OUT_END) == 0);
		CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);
	}

	unsigned int size = img->in_width * img->in_height;

	if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
	{
		if (size > MAX_SIZE)
		{
			in_h = img->in_height;
			out_h = img->out_height;
			img->out_height /= 2;
			img->in_height /= 2;
			img->stride->y *=2 ;
			img->stride->u *=2 ;
			img->stride->v *=2 ;
		}
	}

	// IPU setting
	jz47_ipu_resize();

	if (type == TYPE_IPU_DIRECT)
	{
		if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
		{
			if (size > MAX_SIZE)
			{
				fg1.h = img->out_height * 2;
				img->out_height = out_h;
				img->in_height = in_h;
			}
			else
			{
				fg1.h = img->out_height;
			}

			fg1.w = img->out_width;
			img->out_x = fg1.x = ((plcd_mode->width - fg1.w) / 2) & ~1;
			img->out_y = fg1.y = ((plcd_mode->height - fg1.h) / 2) & ~1;

			fg1.y &= ~1;
			OUTREG32(A_LCD_LCDXYP1, (fg1.y << 16) | (fg1.x << 0));
			OUTREG32(A_LCD_LCDSIZE1, (fg1.h << 16) | (fg1.w << 0));
			OUTREG32(A_LCD_LCDDA0, PHYS(pCurDesc));
			OUTREG32(A_LCD_LCDSTATE, 0); // clear the lcd state register

			ipu_open();					// enable ipu_open
			lcd_open();
//			jz4755tve_open();
		}
		else
		{
			fg1.w = img->out_width;
			fg1.h = img->out_height;
			fg1.x = img->out_x + (w - img->out_width) /  2;
			fg1.y = img->out_y + (h - img->out_height) / 2;
			img->out_x = fg1.x;
			img->out_y = fg1.y;		

			lcd_close_backlight();

//			ret = spin_lock_irqsave();

			SETREG32(A_LCD_LCDCTRL, LCD_CTRL_DIS);
			while ((INREG8(A_LCD_LCDSTATE) & LCD_STATE_LDD) == 0);
			OUTREG32(A_LCD_LCDXYP1, (fg1.y << 16) | (fg1.x << 0));
			OUTREG32(A_LCD_LCDSIZE1, (fg1.h << 16) | (fg1.w << 0));
			OUTREG32(A_LCD_LCDSTATE, 0); // clear the lcd state register

			ipu_open();					// enable ipu_open

			CLRREG32(A_LCD_LCDCTRL, LCD_CTRL_DIS);
			SETREG32(A_LCD_LCDCTRL, LCD_CTRL_ENA);			// enable lcd

//			spin_unlock_irqrestore(ret);

			os_TimeDelay(30);

			if (lcd_get_backlight() != 0)
			{
				lcd_set_backlight(lcd_get_backlight());
			}

		}
	}
	else
	{
		ipu_open();
	}

}

// mode = 0; lcd
// mode = 1; Pal
// mode = 2; ntsc
// mode = 3; exception to lcd
void set_video_output(int mode)
{
	printf("%s, mode[%d], old mode[%d]\n", __FUNCTION__, mode, outMode);

	bOsdOpen = 0;
	bSysBusy = 1;

	switch(mode)
	{
		case 0:
			if (INREG32(A_LCD_LCDCFG) & LCDCFG_TVEN)
			{
					outMode = 0;
					lcd_close_backlight();
					lcd_preinit();
					tv_to_lcd();
			}
			break;

		case 1:
			if (outMode != IOCTL_LCD_TO_TVE_PAL)
			{
				outMode = IOCTL_LCD_TO_TVE_PAL;
			}
			break;
		case 2:
			if (outMode != IOCTL_LCD_TO_TVE_NTSC)
			{
				outMode = IOCTL_LCD_TO_TVE_NTSC;
			}
			break;

		case 3:
			outMode = 0;
			lcd_close_backlight();
			lcd_preinit();
			tv_to_lcd();

			#if (LCDTYPE == 5)
				os_TimeDelay(300);
			#else
				os_TimeDelay(5);
			#endif

			__gpio_as_lcd_24bit();
			os_TimeDelay(20);
			if (lcd_get_backlight() != 0)
			{
				lcd_set_backlight(lcd_get_backlight());
			}
		break;

		default:
			printf("%s, not support mode[%d]\n", __FUNCTION__, mode);
			outMode = 0;
			break;
	}
}

int lcd_ioctl(void *buff, unsigned int cmd)
{
	PLCD_DESCRIPTOR_NEW	pdesc0 = &lcd_frame_desc2[0];

	switch (cmd)
	{
		case IOCTL_GET_LCD_CFG:
			memcpy(buff, plcd_mode, sizeof(LCD_MODE));
			break;

		case IOCTL_SET_LCD_CFG:
			memcpy(plcd_mode, buff, sizeof(LCD_MODE));
			break;

		case IOCTL_DMA1_TO_IPU:
			dma1_to_ipu((img_param_t *)buff);
			break;

		case IOCTL_IPU_TO_DMA1:
			ipu_to_dma1();
			break;

		case IOCTL_LCD_RESIZE:
			image_resize((img_param_t *)buff, TYPE_IPU_NO_DIRECT);
			break;

		// for ipu direct to lcd
		case IOCTL_LCD_RESIZE_ALL:
			image_resize((img_param_t *)buff, TYPE_IPU_DIRECT);
			break;

		case IOCTL_LCD_TO_TVE_PAL:
		case IOCTL_LCD_TO_TVE_NTSC:
			lcd_close_backlight();
			lcd_to_tv(cmd, (img_param_t *)buff);
			break;

		case IOCTL_TVE_TO_LCD:
			lcd_close_backlight();
			lcd_preinit();
			tv_to_lcd();

			#if (LCDTYPE == 5)
				os_TimeDelay(300);
			#else
				os_TimeDelay(5);
			#endif

			__gpio_as_lcd_24bit();
			os_TimeDelay(20);
			if (lcd_get_backlight() != 0)
			{
				lcd_set_backlight(lcd_get_backlight());
			}
			break;

		case IOCTL_SET_IPU:
			*(unsigned int *)buff = 0;
			ipu_init((img_param_t *)buff);
			ipu_open();
			break;

		case IOCTL_PENDING_IPU_END:
			ipu_ioctl(0, IOCTL_IPU_PENDING_OUTEND);
			break;

		case IOCTL_PAL_TO_NTSC:
			lcd_to_tv(PANEL_MODE_TVE_NTSC, (img_param_t *)buff);
			break;

		case IOCTL_NTSC_TO_PAL:
			lcd_to_tv(PANEL_MODE_TVE_PAL, (img_param_t *)buff);
			break;

		case IOCTL_SVIDEO_TO_CVBS:
			jz4755tve_ioctl((void *)0, IOCTL_TV_SVIDEO_TO_CVBS);
			break;

		case IOCTL_CVBS_TO_SVIDEO:
			jz4755tve_ioctl((void *)0, IOCTL_TV_CVBS_TO_SVIDEO);
			break;

		case IOCTL_CLOSE_TVE:
			jz4755tve_close();
			break;

		case IOCTL_GET_OSD_CFG:
			memcpy(buff, posd_mode, sizeof(OSD_MODE));
			break;

		case IOCTL_SET_OSD_CFG:
			memcpy(posd_mode, buff, sizeof(OSD_MODE));
			break;

		case IOCTL_GET_FG0_INFO:
			memcpy(buff, &posd_mode->foreground0, sizeof(OSD_FG));
			break;

		case IOCTL_GET_FG1_INFO:
			memcpy(buff, &posd_mode->foreground1, sizeof(OSD_FG));
			break;

		default:
			printf("lcd not support ioctl cmd[%#x]\r\n", cmd);
			return (-1);
			break;
	}

	return (0);
}


//判断是否支持窗口模式
int lcd_is_support_window_mode()
{
	return 0;
}

//返回屏幕的显示更新范围
void lcd_get_update_rect(int* x, int* y, int* w, int* h)
{
	*x = posd_mode->foreground1.x;
	*y = posd_mode->foreground1.y;
	*w = posd_mode->foreground1.w;
	*h = posd_mode->foreground1.h;
}

unsigned int is_tv_show(void)
{
	return (bTvShowing || bSysBusy);
}
