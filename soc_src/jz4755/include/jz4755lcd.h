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
 *  Create:   2008-09-23, by jbyu
 *
 *******************************************************************************
 */

#ifndef __JZ4755LCD_H__
#define __JZ4755LCD_H__

//--------------------------------------------------------------------------
// LCD Controller(LCD) Address Definition
//--------------------------------------------------------------------------
#define LCD_PHYS_ADDR			( 0x13050000 )
#define LCD_BASE_U_VIRTUAL		( 0xB3050000 )
//#define LCD_24BPP_COMPRESSED

//--------------------------------------------------------------------------
// LCD Registers Offset Definition
//--------------------------------------------------------------------------
#define LCD_LCDCFG_OFFSET		( 0x00 )	// RW, 32
#define LCD_LCDVSYNC_OFFSET		( 0x04 )	// RW, 32
#define LCD_LCDHSYNC_OFFSET		( 0x08 )	// RW, 32
#define LCD_LCDVAT_OFFSET		( 0x0C )	// RW, 32
#define LCD_LCDDAH_OFFSET		( 0x10 )	// RW, 32
#define LCD_LCDDAV_OFFSET		( 0x14 )	// RW, 32
#define LCD_LCDPS_OFFSET		( 0x18 )	// RW, 32
#define LCD_LCDCLS_OFFSET		( 0x1C )	// RW, 32
#define LCD_LCDSPL_OFFSET		( 0x20 )	// RW, 32
#define LCD_LCDREV_OFFSET		( 0x24 )	// RW, 32
#define LCD_LCDCTRL_OFFSET		( 0x30 )	// RW, 32
#define LCD_LCDSTATE_OFFSET		( 0x34 )	// RW, 8
#define LCD_LCDIID_OFFSET		( 0x38 )	// R, 32

#define LCD_LCDDA0_OFFSET		( 0x40 )	// RW, 32
#define LCD_LCDSA0_OFFSET		( 0x44 )	// R, 32
#define LCD_LCDFID0_OFFSET		( 0x48 )	// R, 32
#define LCD_LCDCMD0_OFFSET		( 0x4C )	// R, 32
#define LCD_LCDDA1_OFFSET		( 0x50 )	// RW, 32
#define LCD_LCDSA1_OFFSET		( 0x54 )	// R, 32
#define LCD_LCDFID1_OFFSET		( 0x58 )	// R, 32
#define LCD_LCDCMD1_OFFSET		( 0x5C )	// R, 32
#define LCD_LCDOFFS0_OFFSET		( 0x60 )	// R, 32
#define LCD_LCDPW0_OFFSET		( 0x64 )	// R, 32
#define LCD_LCDCNUM0_OFFSET		( 0x68 )	// R, 32
#define LCD_LCDDESSIZE0_OFFSET	( 0x6C )	// R, 32
#define LCD_LCDOFFS1_OFFSET		( 0x70 )	// R, 32
#define LCD_LCDPW1_OFFSET		( 0x74 )	// R, 32
#define LCD_LCDCNUM1_OFFSET		( 0x78 )	// R, 32
#define LCD_LCDDESSIZE1_OFFSET	( 0x7C )	// R, 32
#define LCD_LCDRGBC_OFFSET		( 0x90 )	// RW, 16

#define LCD_MCFG_OFFSET			( 0xA0 )	// RW, 32
#define LCD_MCTRL_OFFSET		( 0xA4 )	// RW, 8
#define LCD_MSTATE_OFFSET		( 0xA8 )	// RW, 8
#define LCD_MDATA_OFFSET		( 0xAC )	// RW, 32

#define LCD_LCDOSDC_OFFSET		( 0x100 )	// RW, 16
#define LCD_LCDOSDCTRL_OFFSET	( 0x104 )	// RW, 16
#define LCD_LCDOSDS_OFFSET		( 0x108 )	// RW, 16
#define LCD_LCDBGC_OFFSET		( 0x10C )	// RW, 32
#define LCD_LCDKEY0_OFFSET		( 0x110 )	// RW, 32
#define LCD_LCDKEY1_OFFSET		( 0x114 )	// RW, 32
#define LCD_LCDALPHA_OFFSET		( 0x118 )	// RW, 8
#define LCD_LCDIPUR_OFFSET		( 0x11C )	// RW, 32
#define LCD_LCDXYP0_OFFSET		( 0x120 )	// RW, 32
#define LCD_LCDXYP1_OFFSET		( 0x124 )	// RW, 32
#define LCD_LCDSIZE0_OFFSET		( 0x128 )	// RW, 32
#define LCD_LCDSIZE1_OFFSET		( 0x12C )	// RW, 32

//--------------------------------------------------------------------------
// LCD Registers Address Definition
//--------------------------------------------------------------------------
#define A_LCD_LCDCFG			( LCD_BASE_U_VIRTUAL + LCD_LCDCFG_OFFSET )
#define A_LCD_LCDVSYNC			( LCD_BASE_U_VIRTUAL + LCD_LCDVSYNC_OFFSET )
#define A_LCD_LCDHSYNC			( LCD_BASE_U_VIRTUAL + LCD_LCDHSYNC_OFFSET )
#define A_LCD_LCDVAT			( LCD_BASE_U_VIRTUAL + LCD_LCDVAT_OFFSET )
#define A_LCD_LCDDAH			( LCD_BASE_U_VIRTUAL + LCD_LCDDAH_OFFSET )
#define A_LCD_LCDDAV			( LCD_BASE_U_VIRTUAL + LCD_LCDDAV_OFFSET )
#define A_LCD_LCDPS				( LCD_BASE_U_VIRTUAL + LCD_LCDPS_OFFSET )
#define A_LCD_LCDCLS			( LCD_BASE_U_VIRTUAL + LCD_LCDCLS_OFFSET )
#define A_LCD_LCDSPL			( LCD_BASE_U_VIRTUAL + LCD_LCDSPL_OFFSET )
#define A_LCD_LCDREV			( LCD_BASE_U_VIRTUAL + LCD_LCDREV_OFFSET )
#define A_LCD_LCDCTRL			( LCD_BASE_U_VIRTUAL + LCD_LCDCTRL_OFFSET )
#define A_LCD_LCDSTATE			( LCD_BASE_U_VIRTUAL + LCD_LCDSTATE_OFFSET )
#define A_LCD_LCDIID			( LCD_BASE_U_VIRTUAL + LCD_LCDIID_OFFSET )
#define A_LCD_LCDDA0			( LCD_BASE_U_VIRTUAL + LCD_LCDDA0_OFFSET )
#define A_LCD_LCDSA0			( LCD_BASE_U_VIRTUAL + LCD_LCDSA0_OFFSET )
#define A_LCD_LCDFID0			( LCD_BASE_U_VIRTUAL + LCD_LCDFID0_OFFSET )
#define A_LCD_LCDCMD0			( LCD_BASE_U_VIRTUAL + LCD_LCDCMD0_OFFSET )
#define A_LCD_LCDDA1			( LCD_BASE_U_VIRTUAL + LCD_LCDDA1_OFFSET )
#define A_LCD_LCDSA1			( LCD_BASE_U_VIRTUAL + LCD_LCDSA1_OFFSET )
#define A_LCD_LCDFID1			( LCD_BASE_U_VIRTUAL + LCD_LCDFID1_OFFSET )
#define A_LCD_LCDCMD1			( LCD_BASE_U_VIRTUAL + LCD_LCDCMD1_OFFSET )
#define A_LCD_LCDOFFS0			( LCD_BASE_U_VIRTUAL + LCD_LCDOFFS0_OFFSET )
#define A_LCD_LCDPW0			( LCD_BASE_U_VIRTUAL + LCD_LCDPW0_OFFSET )
#define A_LCD_LCDCNUM0			( LCD_BASE_U_VIRTUAL + LCD_LCDCNUM0_OFFSET )
#define A_LCD_LCDDESSIZE0		( LCD_BASE_U_VIRTUAL + LCD_LCDDESSIZE0_OFFSET )
#define A_LCD_LCDOFFS1			( LCD_BASE_U_VIRTUAL + LCD_LCDOFFS1_OFFSET )
#define A_LCD_LCDPW1			( LCD_BASE_U_VIRTUAL + LCD_LCDPW1_OFFSET )
#define A_LCD_LCDCNUM1			( LCD_BASE_U_VIRTUAL + LCD_LCDCNUM1_OFFSET )
#define A_LCD_LCDDESSIZE1		( LCD_BASE_U_VIRTUAL + LCD_LCDDESSIZE1_OFFSET )
#define A_LCD_LCDRGBC			( LCD_BASE_U_VIRTUAL + LCD_LCDRGBC_OFFSET )
#define A_LCD_MCFG				( LCD_BASE_U_VIRTUAL + LCD_MCFG_OFFSET )
#define A_LCD_MCTRL				( LCD_BASE_U_VIRTUAL + LCD_MCTRL_OFFSET )
#define A_LCD_MSTATE			( LCD_BASE_U_VIRTUAL + LCD_MSTATE_OFFSET )
#define A_LCD_MDATA				( LCD_BASE_U_VIRTUAL + LCD_MDATA_OFFSET )
#define A_LCD_LCDOSDC			( LCD_BASE_U_VIRTUAL + LCD_LCDOSDC_OFFSET )
#define A_LCD_LCDOSDCTRL		( LCD_BASE_U_VIRTUAL + LCD_LCDOSDCTRL_OFFSET )
#define A_LCD_LCDOSDS			( LCD_BASE_U_VIRTUAL + LCD_LCDOSDS_OFFSET )
#define A_LCD_LCDBGC			( LCD_BASE_U_VIRTUAL + LCD_LCDBGC_OFFSET )
#define A_LCD_LCDKEY0			( LCD_BASE_U_VIRTUAL + LCD_LCDKEY0_OFFSET )
#define A_LCD_LCDKEY1			( LCD_BASE_U_VIRTUAL + LCD_LCDKEY1_OFFSET )
#define A_LCD_LCDALPHA			( LCD_BASE_U_VIRTUAL + LCD_LCDALPHA_OFFSET )
#define A_LCD_LCDIPUR			( LCD_BASE_U_VIRTUAL + LCD_LCDIPUR_OFFSET )
#define A_LCD_LCDXYP0			( LCD_BASE_U_VIRTUAL + LCD_LCDXYP0_OFFSET )
#define A_LCD_LCDXYP1			( LCD_BASE_U_VIRTUAL + LCD_LCDXYP1_OFFSET )
#define A_LCD_LCDSIZE0			( LCD_BASE_U_VIRTUAL + LCD_LCDSIZE0_OFFSET )
#define A_LCD_LCDSIZE1			( LCD_BASE_U_VIRTUAL + LCD_LCDSIZE1_OFFSET )

//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------

// LCD Config Register(LCDCFG) Common Define
#define LCDCFG_SLCD_PIN			( 1 << 31 )
#define LCDCFG_TVEPEH			( 1 << 30 )
#define LCDCFG_FUHOLD			( 1 << 29 )
#define LCDCFG_NEWDES			( 1 << 28 )
#define LCDCFG_PALBP			( 1 << 27 )
#define LCDCFG_TVEN				( 1 << 26 )
#define LCDCFG_RECOVER			( 1 << 25 )
#define LCDCFG_DITHER			( 1 << 24 )
#define LCDCFG_PSM_DISABLE		( 1 << 23 )
#define LCDCFG_CLSM_DISABLE		( 1 << 22 )
#define LCDCFG_SPLM_DISABLE		( 1 << 21 )
#define LCDCFG_REVM_DISABLE		( 1 << 20 )
#define LCDCFG_HSYNM_DISABLE	( 1 << 19 )
#define LCDCFG_PCLKM_DISABLE	( 1 << 18 )
#define LCDCFG_INVDAT			( 1 << 17 )
#define LCDCFG_SYNDIR_IN		( 1 << 16 )
#define LCDCFG_PSP				( 1 << 15 )
#define LCDCFG_CLSP				( 1 << 14 )
#define LCDCFG_SPLP				( 1 << 13 )
#define LCDCFG_REVP				( 1 << 12 )
#define LCDCFG_HSYNCP_N			( 1 << 11 )
#define LCDCFG_PCLKP_N			( 1 << 10 )
#define LCDCFG_DEP_N			( 1 << 9 )
#define LCDCFG_VSYNCP_N			( 1 << 8 )
#define LCDCFG_TFT_18BIT		( 1 << 7 )
#define LCDCFG_TFT_24BIT		( 1 << 6 )

#define LCDCFG_STN_DAT_PIN1		( 0x0 << 4 )
#define LCDCFG_STN_DAT_PIN2		( 0x1 << 4 )
#define LCDCFG_STN_DAT_PIN4		( 0x2 << 4 )
#define LCDCFG_STN_DAT_PIN8		( 0x3 << 4 )
#define LCDCFG_STN_DAT_PIN_MASK ( LCDCFG_STN_DAT_PIN8 )

#define STN_DAT_PIN1			(0x0 << 4)
#define STN_DAT_PIN2			(0x1 << 4)
#define STN_DAT_PIN4			(0x2 << 4)
#define STN_DAT_PIN8			(0x3 << 4)
#define STN_DAT_PINMASK	        STN_DAT_PIN8

#define MODE_TFT_GEN			0x00
#define MODE_TFT_SHARP			0x01
#define MODE_TFT_CASIO			0x02
#define MODE_TFT_SAMSUNG		0x03
#define MODE_CCIR656_NONINT		0x04
#define MODE_CCIR656_INT		0x06
#define MODE_STN_COLOR_SINGLE	0x08
#define MODE_STN_MONO_SINGLE	0x09
#define MODE_STN_COLOR_DUAL	    0x0A
#define MODE_STN_MONO_DUAL	    0x0B
#define MODE_8BIT_SERIAL_TFT	0x0C
#define MODE_8BIT_LCM		    0x0D
#define MODE_MASK		        0x0F
#define MODE_TFT_18BIT			(1 << 7)
#define MODE_TFT_24BIT			(1 << 6)
#define LCDCFG_MODE_SLCD		0x0D

#define HSYNC_N                 (LCDCFG_HSYNCP_N)
#define PCLK_N		            (LCDCFG_PCLKP_N)
#define DEP_N		            (LCDCFG_DEP_N)
#define VSYNC_N                 (LCDCFG_VSYNCP_N)

// LCD Control Register(LCDCTRL) Common Define
#define LCD_CTRL_FRC_2          (2 << 24)
#define LCD_CTRL_FRC_4          (1 << 24)
#define LCD_CTRL_FRC_16         (0 << 24)
#define LCD_CTRL_VGA            (1 << 15)
#define LCD_CTRL_DACTE          (1 << 14) 

// interrupt mask define 0-disable, 1 enable
#define	LCD_CTRL_EOFM 			(1 <<13)	// Mask end of frame interrupt
#define LCD_CTRL_SOFM 			(1 << 12)	//Mask start of frame interrupt
#define LCD_CTRL_OFUM 			(1 << 11)	//Mask out FIFO under run interrupt
#define LCD_CTRL_IFUM0 			(1 << 10)	//Mask in FIFO 0 under run interrupt
#define LCD_CTRL_IFUM1 			(1 << 9)	//Mask in FIFO 1 under run interrupt
#define LCD_CTRL_LDDM 			(1 << 8)	//Mask LCD disable done interrupt
#define LCD_CTRL_QDM 			(1 << 7)	//Mask LCD quick disable done interrupt


#define LCD_CTRL_BEDN			(1 << 6)	//Endian selection
#define LCD_CTRL_PEDN			(1 << 5)	//Endian in byte: 0 ¨C msb first, 1 ¨C lsb first
#define LCD_CTRL_DIS			(1 << 4)
#define LCD_CTRL_ENA            (1 << 3)

#define LCD_CTRL_BPP_1          (0 << 0)
#define LCD_CTRL_BPP_2          (1 << 0)
#define LCD_CTRL_BPP_4          (2 << 0)
#define LCD_CTRL_BPP_8          (3 << 0)
#define LCD_CTRL_BPP_16         (4 << 0)
#define LCD_CTRL_BPP_18_24      (5 << 0)
#define LCD_CTRL_BPP_24_ZIP     (6 << 0)    //24bpp compressed
#define LCD_BPP_MASK			(0x07)
#define LCD_CTRL_RGB555			(1 << 27)
#define LCD_CTRL_BST_4			(0 << 28)
#define LCD_CTRL_BST_8			(1 << 28)
#define LCD_CTRL_BST_16			(2 << 28)

// lcdstate define
#define LCD_STATE_LDD           (1 << 0)
#define LCD_STATE_IFU1          (1 << 1)
#define LCD_STATE_IFU0          (1 << 2)
#define LCD_STATE_OUF           (1 << 3)
#define LCD_STATE_SOF           (1 << 4)
#define LCD_STATE_EOF           (1 << 5)
#define LCD_STATE_QD            (1 << 7)

// lcd DMA Register field define
#define LCD_CMD_SOFINT          (1 << 31)
#define LCD_CMD_EOFINT          (1 << 30)
#define LCD_CMD_CMD		        (1 << 29)
#define LCD_CMD_PAL             (1 << 28)

// OSD Configure Register (LCDOSDC)
#define LCD_OSDC_SOFM1			(1 << 15)
#define LCD_OSDC_EOFM1			(1 << 14)
#define LCD_OSDC_SOFM0			(1 << 11)
#define LCD_OSDC_EOFM0			(1 << 10)
#define LCD_OSDC_F1EN			(1 << 4)
#define LCD_OSDC_F0EN			(1 << 3)
#define LCD_OSDC_ALPHAEN		(1 << 2)
#define LCD_OSDC_ALPHAMD		(1 << 1)
#define LCD_OSDC_OSDEN			(1 << 0)

// OSD Control Register (LCDOSDCTRL)
#define LCD_OSDCTRL_IPU			(1 << 15)
#define LCD_OSDCTRL_OSDRGB		(1 << 4)
#define LCD_OSDCTRL_CHANGE      (1 << 3)

// OSD State Register (LCDOSDS)
#define LCD_OSDS_SOF1           (1 << 15)
#define LCD_OSDS_EOF1           (1 << 14)
#define LCD_OSDS_SOF0           (1 << 11)
#define LCD_OSDS_EOF0           (1 << 10)
#define LCD_OSDS_READY          (1 << 0)

// Foreground Color Key Register 0/1 (LCDKEY0/1)
#define LCD_LCDKEYX_KEYEN		(1 << 31)
#define LCD_LCDKEYX_KEYMD		(1 << 30)

// RGB Control (LCDRGBC)
#define LCD_RGBC_RGBDM			(1 << 15)	// RGB with dummy data enable.
#define LCD_RGBC_DMM 			(1 << 14)	// RGB dummy mode
#define LCD_RGBC_YCC 			(1 << 8) 	// Change RGB to YCbCr

#define LCD_LCDRGBC_OddRGB_BIT	(4)
#define LCD_LCDRGBC_EvenRGB_BIT	(0)

#define RGB_ORDER_RGB			(0)			//RGB mode
#define RGB_ORDER_RBG			(1)
#define RGB_ORDER_GRB			(2)
#define RGB_ORDER_GBR			(3)
#define RGB_ORDER_BRG			(4)
#define RGB_ORDER_BGR			(4)

// SLCD Configure Register(MCFG) Common Define
#define LCD_MCFG_DWIDTH_18BIT   	(0x0 << 10)
#define LCD_MCFG_DWIDTH_16BIT   	(0x1 << 10)
#define LCD_MCFG_DWIDTH_8BIT_THIRD	(0x2 <<10)
#define LCD_MCFG_DWIDTH_8BIT_TWICE	(0x3 << 10)
#define LCD_MCFG_DWIDTH_8BIT_ONCE	(0x4 << 10)
#define LCD_MCFG_CWIDTH_16BIT		(0x0 << 8)
#define LCD_MCFG_CWIDTH_8BIT 		(0x1 << 8)
#define LCD_MCFG_CWIDTH_18BIT   	(0x2 << 8)
#define LCD_MCFG_CWIDTH_24BIT		(0x3 << 8)
#define LCD_MCFG_CS_ACTIVE_LOW  	(0 << 4)
#define LCD_MCFG_CS_ACTIVE_HIGH  	(1 << 4)
#define LCD_MCFG_RS_CMD_LOW     	(0 << 3)
#define LCD_MCFG_RS_CMD_HIGH     	(1 << 3)
#define LCD_MCFG_CLK_ACTIVE_FALLING (0 << 1)
#define LCD_MCFG_CLK_ACTIVE_RASING  (1 << 1)
#define LCD_MCFG_TYPE_PARALLEL  	(0 << 0)
#define LCD_MCFG_TYPE_SERIAL  		(1 << 0)

// SLCD Control Register(MCTRL) Common Define
#define LCD_MCTRL_DMAMODE           (1 << 2)
#define LCD_MCTRL_DMASTART          (1 << 1)
#define LCD_MCTRL_DMA_EN		    (1 << 0)
#define LCD_MCTRL_DMA_NO_EN 	    (0 << 0)

// SLCD Status Register (MSTATE) Common Define
#define LCD_MSTATE_BUSY		        (1 << 0)
#define LCD_MSTATE_NO_BUSY	        (0 << 0)


// The Clock Limitation
#define	MAX_LCD_CLOCK			150000000	// The MAX LCD device clock
#define	MAX_LCD_CLOCK_DIV		32			// The MAX LCD device clock division

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	LCDCFG;			// 0x00, RW
	unsigned int	LCDVSYNC;		// 0x04, RW
	unsigned int	LCDHSYNC;		// 0x08, RW
	unsigned int	LCDVAT;			// 0x0C, RW
	unsigned int	LCDDAH;			// 0x10, RW
	unsigned int	LCDDAV;			// 0x14, RW
	unsigned int	LCDPS;			// 0x18, RW
	unsigned int	LCDCLS;			// 0x1C, RW
	unsigned int	LCDSPL;			// 0x20, RW
	unsigned int	LCDREV;			// 0x24, RW

	unsigned int	LCDRSV00[2];	// 0x28, 0x2C

	unsigned int	LCDCTRL;		// 0x30, RW
	unsigned int	LCDSTATE;		// 0x34, RW
	unsigned int	LCDIID;			// 0x38, R

	unsigned int	LCDRSV01[1];	// 0x3C

	unsigned int	LCDDA0;			// 0x40, RW
	unsigned int	LCDSA0;			// 0x44, R
	unsigned int	LCDFID0;		// 0x48, RW
	unsigned int	LCDCMD0;		// 0x4C, RW
	unsigned int	LCDDA1;			// 0x50, RW
	unsigned int	LCDSA1;			// 0x54, R
	unsigned int	LCDFID1;		// 0x58, RW
	unsigned int	LCDCMD1;		// 0x5C, RW

	unsigned int	LCDOFFS0;		// 0x60, R
	unsigned int	LCDPW0;			// 0x64, R
	unsigned int	LCDCNUM0;		// 0x68, R
	unsigned int	LCDDESSIZE0;	// 0x5C, R
	unsigned int	LCDOFFS1;		// 0x70, R
	unsigned int	LCDPW1;			// 0x74, R
	unsigned int	LCDCNUM1;		// 0x78, R
	unsigned int	LCDDESSIZE1;	// 0x7C, R

	unsigned int	LCDRSV02[4];	// 0x80, 0x84, 0x88, 0x8C

	unsigned short	LCDRGBC;		// 0x90, RW
	unsigned short	LCDRSV03[1];

	unsigned int	LCDRSV04[3];	// 0x94, 0x98, 0x9C

	unsigned int	MCFG;			// 0xA0
	unsigned char	MCTRL;			// 0xA4
	unsigned char	LCDRSV05[3];
	unsigned char	MSTATE;			// 0xA8
	unsigned char	LCDRSV06[3];
	unsigned int	MDATA;			// 0xAC

	unsigned int	LCDRSV07[20];	// 0xB0, -0xFC

	unsigned short	LCDOSDC;		// 0x0100, RW
	unsigned short	LCDRSV08[1];
	unsigned short	LCDOSDCTRL;		// 0x0104, RW
	unsigned short	LCDRSV09[1];
	unsigned short	LCDOSDS;		// 0x0108, RW
	unsigned short	LCDRSV10[1];
	unsigned int	LCDBGC;			// 0x010C, RW
	unsigned int	LCDKEY0;		// 0x0110, RW
	unsigned int	LCDKEY1;		// 0x0114, RW
	unsigned char	LCDALPHA;		// 0x0118, RW
	unsigned char	LCDRSV11[3];
	unsigned int	LCDIPUR;		// 0x011C, RW
	unsigned int	LCDXYP0;		// 0x0120, RW
	unsigned int	LCDXYP1;		// 0x0124, RW
	unsigned int	LCDSIZE0;		// 0x0128, RW
	unsigned int	LCDSIZE1;		// 0x012C, RW

} JZ4755_LCD, *PJZ4755_LCD;

typedef struct __BUFF_INFO__
{
	unsigned int	dwPhyAddr;
	unsigned int	dwVirAddr;
	unsigned int	dwLength;
} BUFF_INFO, *PBUFF_INFO;

typedef struct __LCD_INFO__
{
	unsigned short		usLCDType;
	unsigned int		dwMainClock;

	BUFF_INFO		FrameBuff;
	BUFF_INFO		DMADesc;
	BUFF_INFO		PaletteDesc;
} LCD_INFO, *PLCD_INFO;

typedef struct __LCD_DESC__
{
	unsigned int	NEXTDESC_PA;	// LCDDA
	unsigned int	DATA_PA;		// LCDSA
	unsigned int	FRAME_ID;		// LCDFID
	unsigned int	COMMAND;		// LCDCMD
} LCD_DESCRIPTOR, *PLCD_DESCRIPTOR;

typedef struct __LCD_DESC_NEW
{
	unsigned int	NEXTDESC_PA;	// LCDDA
	unsigned int	DATA_PA;		// LCDSA
	unsigned int	FRAME_ID;		// LCDFID
	unsigned int	COMMAND;		// LCDCMD
	unsigned int	OFFSET;       	// Stride Offsize(in word)
	unsigned int	PAGE_WIDTH; 	// Stride Pagewidth(in word)
	unsigned int	CMD_NUM; 		// Command Number(for SLCD)
	unsigned int	DESC_SIZE; 		// Foreground Size
} LCD_DESCRIPTOR_NEW, *PLCD_DESCRIPTOR_NEW;

typedef struct __OSD_FG__
{
	int bpp;
	int x;
	int y;
	int w;
	int h;
}OSD_FG, *POSD_FG;


typedef struct __LCD_MODE__
{
	unsigned char	*vendor;
	unsigned char	*type;
	int				width;
	int				height;
//	int				bpp;
	int				freq;
	int				hsync;
	int				vsync;
	unsigned int	elw;		// End of Line, in PCLK
	unsigned int	blw;		// Begin of Line, in PCLK
	unsigned int	efw;		// End of Frame, in line count
	unsigned int	bfw;		// Begin of Frame, in line count
	unsigned int	*mode_bitmask;
	unsigned int	cfg;		// lcd configure register
	unsigned int	ctrl;		// lcd control register
	unsigned int	slcd_cfg;	// slcd configure reister

} LCD_MODE, *PLCD_MODE;

typedef struct __OSD_MODE__
{
	unsigned int osd_cfg;		// OSDEN, ALHPAEN, F0EN, F1EN, etc
	unsigned int osd_ctrl;		// IPUEN, OSDBPP, etc
	unsigned int rgb_ctrl;		// RGB Dummy, RGB sequence, RGB to YUV
	unsigned int bgcolor;		// background color(RGB888)
	unsigned int colorkey0;		// foreground0's Colorkey enable, Colorkey value
	unsigned int colorkey1;		// foreground1's Colorkey enable, Colorkey value
	unsigned int alpha;			// ALPHAEN, alpha value
	unsigned int ipu_restart;	// IPU Restart enable, ipu restart interval time

#define FG_NOCHANGE 			0x0000
#define FG0_CHANGE_SIZE 		0x0001
#define FG0_CHANGE_POSITION 	0x0002
#define FG1_CHANGE_SIZE 		0x0010
#define FG1_CHANGE_POSITION 	0x0020
#define FG_CHANGE_ALL 		( FG0_CHANGE_SIZE | FG0_CHANGE_POSITION | \
				  FG1_CHANGE_SIZE | FG1_CHANGE_POSITION )
	int 	fg_change;
	OSD_FG	foreground0;					// foreground 0
	OSD_FG	foreground1;					// foreground 1
}OSD_MODE, *POSD_MODE;

typedef struct __LCD_PARAM__
{
	LCD_MODE lcd_mode;
	OSD_MODE osd_mode;
}LCD_PARAM, *PLCD_PARAM;

/*
 * LCD_D0~LCD_D15, LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_16bit()							\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), 0x003cffff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x003cffff);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x003cffff);				\
	OUTREG32(A_GPIO_PXPES(3), 0x003cffff);				\
} while (0)

/*
 * LCD_D0~LCD_D17, LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_18bit()							\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), 0x003fffff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x003fffff);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x003fffff);				\
	OUTREG32(A_GPIO_PXPES(3), 0x003fffff);				\
} while (0)

/*
 * LCD_D0~LCD_D17, LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 * LCD_CLS UART3_CTS_ LCD_PS LCD_SPL LCD_REV UART3_RTS_
 */
#define __gpio_as_lcd_24bit()							\
do 														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), 0x0fffffff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x0c3fffff);				\
	OUTREG32(A_GPIO_PXSELS(3), 0x03c00000);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x0fffffff);				\
	OUTREG32(A_GPIO_PXPES(3), 0x0fffffff);				\
} while (0)

/* LCD_D0~LCD_D7, SLCD_RS, SLCD_CS */
#define __gpio_as_slcd_8bit()							\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), 0x001800ff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x001800ff);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x001800ff);				\
} while (0)

/* LCD_D0~LCD_D7, SLCD_RS, SLCD_CS */
#define __gpio_as_slcd_9bit()							\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), 0x001801ff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x001801ff);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x001801ff);				\
} while (0)

/* LCD_D0~LCD_D15, SLCD_RS, SLCD_CS */
#define __gpio_as_slcd_16bit()							\
do 														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), 0x0018ffff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x0018ffff);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x0018ffff);				\
} while (0)

/* LCD_D0~LCD_D17, SLCD_RS, SLCD_CS */
#define __gpio_as_slcd_18bit()							\
do 														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), 0x001bffff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x001bffff);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x001bffff);				\
} while (0)

#define __lcd_as_gpio()									\
do 														\
{														\
	OUTREG32(A_GPIO_PXFUNC(3), 0x0fffffff);				\
	OUTREG32(A_GPIO_PXSELC(3), 0x0fffffff);				\
	OUTREG32(A_GPIO_PXTRGC(3), 0x0fffffff);				\
} while (0)

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755LCD_H__
