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
#ifndef __IPU_APP_H__
#define __IPU_APP_H__

// Data Format Register
#define RGB_888_OUT_FMT				( 1 << 24 )

#define RGB_OUT_OFT_BIT				( 21 )
#define RGB_OUT_OFT_MASK			( 7 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_RGB				( 0 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_RBG				( 1 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_GBR				( 2 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_GRB				( 3 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_BRG				( 4 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_BGR				( 5 << RGB_OUT_OFT_BIT )

#define OUT_FMT_BIT					( 19 )
#define OUT_FMT_MASK				( 3 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB555				( 0 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB565				( 1 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB888				( 2 <<  OUT_FMT_BIT )
#define OUT_FMT_YUV422				( 3 <<  OUT_FMT_BIT )

#define YUV_PKG_OUT_OFT_BIT			( 16 )
#define YUV_PKG_OUT_OFT_MASK		( 7 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y1UY0V		( 0 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y1VY0U		( 1 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_UY1VY0		( 2 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_VY1UY0		( 3 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y0UY1V		( 4 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y0VY1U		( 5 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_UY0VY1		( 6 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_VY0UY1		( 7 << YUV_PKG_OUT_OFT_BIT )

#define IN_OFT_BIT					( 2 )
#define IN_OFT_MASK					( 3 << IN_OFT_BIT )
#define IN_OFT_Y1UY0V				( 0 << IN_OFT_BIT )
#define IN_OFT_Y1VY0U				( 1 << IN_OFT_BIT )
#define IN_OFT_UY1VY0				( 2 << IN_OFT_BIT )
#define IN_OFT_VY1UY0				( 3 << IN_OFT_BIT )

#define IN_FMT_BIT					( 0 )
#define IN_FMT_MASK					( 3 << IN_FMT_BIT )
#define IN_FMT_YUV420				( 0 << IN_FMT_BIT )
#define IN_FMT_YUV422				( 1 << IN_FMT_BIT )
#define IN_FMT_YUV444				( 2 << IN_FMT_BIT )
#define IN_FMT_YUV411				( 3 << IN_FMT_BIT )

struct Ration2m
{
	unsigned int ratio;
	int n, m;
};


// parameter
// R = 1.164 * (Y - 16) + 1.596 * (cr - 128)    {C0, C1}
// G = 1.164 * (Y - 16) - 0.392 * (cb -128) - 0.813 * (cr - 128)  {C0, C2, C3}
// B = 1.164 * (Y - 16) + 2.017 * (cb - 128)    {C0, C4}
#define USE_CRCB_MODE 		1
#if USE_CRCB_MODE
#define YUV_CSC_C0					0x4A8        /* 1.164 * 1024 */
#define YUV_CSC_C1					0x662        /* 1.596 * 1024 */
#define YUV_CSC_C2					0x191        /* 0.392 * 1024 */
#define YUV_CSC_C3					0x341        /* 0.813 * 1024 */
#define YUV_CSC_C4					0x811        /* 2.017 * 1024 */
#define YUV_CSC_CHROM				128
#define YUV_CSC_LUMA				16
#else
#define YUV_CSC_C0					0x400
#define YUV_CSC_C1					0x59C
#define YUV_CSC_C2					0x161
#define YUV_CSC_C3					0x2DC
#define YUV_CSC_C4					0x718
#define YUV_CSC_CHROM				128
#define YUV_CSC_LUMA				0
#endif

struct YuvCsc
{									// YUV(default)	or	YCbCr
	unsigned int csc0;				//	0x400			0x4A8
	unsigned int csc1;              //	0x59C   		0x662
	unsigned int csc2;              //	0x161   		0x191
	unsigned int csc3;              //	0x2DC   		0x341
	unsigned int csc4;              //	0x718   		0x811
	unsigned int chrom;             //	128				128
	unsigned int luma;              //	0				16
};

struct YuvStride
{
	unsigned int y;
	unsigned int u;
	unsigned int v;
	unsigned int out;
};

typedef struct
{
	unsigned int		ipu_ctrl;				// IPU Control Register
	unsigned int		ipu_d_fmt;				// IPU Data Format Register
	unsigned int		in_width;
	unsigned int		in_height;
	unsigned int		in_bpp;
	unsigned int		out_x;
	unsigned int		out_y;
//	unsigned int		in_fmt;
//	unsigned int		out_fmt;
	unsigned int		out_width;
	unsigned int		out_height;
	unsigned char*		y_buf;
	unsigned char*		u_buf;
	unsigned char*		v_buf;
	unsigned char*		out_buf;
	unsigned char*		y_t_addr;				// table address
	unsigned char*		u_t_addr;
	unsigned char*		v_t_addr;
	unsigned char*		out_t_addr;
	struct Ration2m*	ratio_table;
	struct YuvCsc*		csc;
	struct YuvStride*	stride;
} img_param_t;

// Function prototype
int ipu_open(void);
int ipu_close(void);
int ipu_init(img_param_t *pimg);
int ipu_deinit(void);
int ipu_poweroff(void);
int ipu_poweron(void);
int ipu_ioctl(void *buff, unsigned int cmd);

#define IOCTL_IPU_SET_BUFF					0x0010
#define IOCTL_IPU_CHANGE_BUFF				0x0011
#define IOCTL_IPU_START						0x0012
#define IOCTL_IPU_STOP						0x0013
#define IOCTL_IPU_FB_SIZE					0x0014
#define IOCTL_IPU_SET_CTRL_REG				0x0015
#define IOCTL_IPU_SET_FMT_REG				0x0016
#define IOCTL_IPU_SET_CSC					0x0017
#define IOCTL_IPU_SET_STRIDE				0x0018
#define IOCTL_IPU_SET_OUTSIZE				0x0019
#define IOCTL_IPU_PENDING_OUTEND			0x0020

#endif
