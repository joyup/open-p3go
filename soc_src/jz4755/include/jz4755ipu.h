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
 *  Author:   <jbyu@ingenic.cn>
 *
 *  Create:   2008-10-15, by jbyu
 *  Maintain: 2009-03-23, by jbyu
 *            ADDED IPU_ADDR_CTRL register and its bit definition
 *
 *******************************************************************************
 */

#ifndef __JZ4755IPU_H__
#define __JZ4755IPU_H__

//--------------------------------------------------------------------------
// IPU Controller(IPU) Address Definition
//--------------------------------------------------------------------------
#define IPU_PHYS_ADDR				( 0x13080000 )
#define IPU_BASE_U_VIRTUAL			( 0xB3080000 )

//--------------------------------------------------------------------------
// IPU Registers Offset Definition
//--------------------------------------------------------------------------
#define IPU_CONTROL					( 0x00 )	// RW, 32, IPU Control Register
#define IPU_STATUS					( 0x04 )	// RW, 32, IPU Status Register
#define IPU_D_FMT					( 0x08 )	// RW, 32, Data Format Register
#define IPU_Y_ADDR					( 0x0C )	// RW, 32, Input Y Data Address Register
#define IPU_U_ADDR					( 0x10 )	// RW, 32, Input U Data Address Register
#define IPU_V_ADDR					( 0x14 )	// RW, 32, Input V Data Address Register
#define IPU_IN_FM_GS				( 0x18 )	// RW, 32, Input Geometric Size Register
#define IPU_Y_STRIDE				( 0x1C )	// RW, 32, Input Y Data Line Stride Register
#define IPU_UV_STRIDE				( 0x20 )	// RW, 32, Input UV Data Line Stride Register
#define IPU_OUT_ADDR				( 0x24 )	// RW, 32, Output Frame Start Address Register
#define IPU_OUT_GS					( 0x28 )	// RW, 32, Output Geometric Size Register
#define IPU_OUT_STRIDE				( 0x2C )	// RW, 32, Output Data Line Stride Register
#define IPU_RSZ_COEF_INDEX			( 0x30 )	// RW, 32, Resize Coefficients Table Index Register
#define IPU_CSC_C0_COEF				( 0x34 )	// RW, 32, CSC C0 Coefficient Register
#define IPU_CSC_C1_COEF				( 0x38 )	// RW, 32, CSC C1 Coefficient Register
#define IPU_CSC_C2_COEF				( 0x3C )	// RW, 32, CSC C2 Coefficient Register
#define IPU_CSC_C3_COEF				( 0x40 )	// RW, 32, CSC C3 Coefficient Register
#define IPU_CSC_C4_COEF				( 0x44 )	// RW, 32, CSC C4 Coefficient Register
#define IPU_HRSZ_COEF_LUT			( 0x48 )	// RW, 32, Horizontal Resize Coefficients Look Up Table Register group
#define IPU_VRSZ_COEF_LUT			( 0x4C )	// RW, 32, Vertical Resize Coefficients Look Up Table Register group
#define IPU_CSC_OFSET_PARA			( 0x50 )	// RW, 32, CSC Offset Parameter Register
#define IPU_Y_PHY_T_ADDR			( 0x54 )	// RW, 32, Input Y physics table address
#define IPU_U_PHY_T_ADDR			( 0x58 )	// RW, 32, Input U physics table address
#define IPU_V_PHY_T_ADDR			( 0x5C )	// RW, 32, Input V physics table address
#define IPU_OUT_PHY_T_ADDR			( 0x60 )	// RW, 32, OUT physics table address
#define IPU_ADDR_CTRL               ( 0x64 )    // RW, 32, IPU address control register

//--------------------------------------------------------------------------
// IPU Registers Address Definition
//--------------------------------------------------------------------------
#define A_IPU_CONTROL				( IPU_BASE_U_VIRTUAL + IPU_CONTROL )
#define A_IPU_STATUS				( IPU_BASE_U_VIRTUAL + IPU_STATUS )
#define A_IPU_D_FMT					( IPU_BASE_U_VIRTUAL + IPU_D_FMT	)
#define A_IPU_Y_ADDR				( IPU_BASE_U_VIRTUAL + IPU_Y_ADDR )
#define A_IPU_U_ADDR				( IPU_BASE_U_VIRTUAL + IPU_U_ADDR )
#define A_IPU_V_ADDR				( IPU_BASE_U_VIRTUAL + IPU_V_ADDR )
#define A_IPU_IN_FM_GS				( IPU_BASE_U_VIRTUAL + IPU_IN_FM_GS )
#define A_IPU_Y_STRIDE				( IPU_BASE_U_VIRTUAL + IPU_Y_STRIDE )
#define A_IPU_UV_STRIDE				( IPU_BASE_U_VIRTUAL + IPU_UV_STRIDE )
#define A_IPU_OUT_ADDR				( IPU_BASE_U_VIRTUAL + IPU_OUT_ADDR )
#define A_IPU_OUT_GS				( IPU_BASE_U_VIRTUAL + IPU_OUT_GS )
#define A_IPU_OUT_STRIDE			( IPU_BASE_U_VIRTUAL + IPU_OUT_STRIDE )
#define A_IPU_RSZ_COEF_INDEX		( IPU_BASE_U_VIRTUAL + IPU_RSZ_COEF_INDEX )
#define A_IPU_CSC_C0_COEF			( IPU_BASE_U_VIRTUAL + IPU_CSC_C0_COEF )
#define A_IPU_CSC_C1_COEF			( IPU_BASE_U_VIRTUAL + IPU_CSC_C1_COEF )
#define A_IPU_CSC_C2_COEF			( IPU_BASE_U_VIRTUAL + IPU_CSC_C2_COEF )
#define A_IPU_CSC_C3_COEF			( IPU_BASE_U_VIRTUAL + IPU_CSC_C3_COEF )
#define A_IPU_CSC_C4_COEF			( IPU_BASE_U_VIRTUAL + IPU_CSC_C4_COEF )
#define A_IPU_HRSZ_COEF_LUT			( IPU_BASE_U_VIRTUAL + IPU_HRSZ_COEF_LUT )
#define A_IPU_VRSZ_COEF_LUT			( IPU_BASE_U_VIRTUAL + IPU_VRSZ_COEF_LUT )
#define A_IPU_CSC_OFSET_PARA		( IPU_BASE_U_VIRTUAL + IPU_CSC_OFSET_PARA )
#define A_IPU_Y_PHY_T_ADDR			( IPU_BASE_U_VIRTUAL + IPU_Y_PHY_T_ADDR )
#define A_IPU_U_PHY_T_ADDR			( IPU_BASE_U_VIRTUAL + IPU_U_PHY_T_ADDR )
#define A_IPU_V_PHY_T_ADDR			( IPU_BASE_U_VIRTUAL + IPU_V_PHY_T_ADDR )
#define A_IPU_OUT_PHY_T_ADDR		( IPU_BASE_U_VIRTUAL + IPU_OUT_PHY_T_ADDR )
#define A_IPU_ADDR_CTRL             ( IPU_BASE_U_VIRTUAL + IPU_ADDR_CTRL)

// IPU Control Register
#define ADDR_SEL                    ( 1 << 20 )
#define DFIX_SEL					( 1 << 17 )
#define FIELD_SEL                   ( 1 << 16 )
#define FIELD_CONF_EN               ( 1 << 15 )
#define DISP_SEL   	                ( 1 << 14 )
#define DPAGE_MAP                   ( 1 << 13 )
#define SPAGE_MAP                   ( 1 << 12 )
#define LCDC_SEL                    ( 1 << 11 )
#define SPKG_SEL                    ( 1 << 10 )
#define V_SCALE                     ( 1 << 9 )
#define H_SCALE                     ( 1 << 8 )
#define IPU_RST                     ( 1 << 6 )
#define FM_IRQ_EN                   ( 1 << 5 )
#define CSC_EN                      ( 1 << 4 )
#define VRSZ_EN                     ( 1 << 3 )
#define HRSZ_EN                     ( 1 << 2 )
#define IPU_RUN                     ( 1 << 1 )
#define CHIP_EN                     ( 1 << 0 )

#define V_SCALE_BIT					( 9 )
#define H_SCALE_BIT					( 8 )

// IPU Status Register
#define SIZE_ERR                    ( 1 << 2 )
#define FMT_ERR                     ( 1 << 1 )
#define OUT_END                     ( 1 << 0 )

// IPU address control register
#define PTD_READY                   (1 << 7)                              
#define PTV_READY                   (1 << 6) 
#define PTU_READY                   (1 << 5)
#define PTY_READY                   (1 << 4)
#define D_READY                     (1 << 3)
#define Y_READY                     (1 << 2)
#define U_READY                     (1 << 1)
#define V_READY                     (1 << 0)

#define YUV_READY                   (Y_READY | U_READY | V_READY)

// Input Geometric Size Register
#define IN_FM_W_BIT					( 16 )
#define IN_FM_W_MASK				( 0xfff << IN_FM_W_BIT )

#define IN_FM_H_BIT					( 0 )
#define IN_FM_H_MASK				( 0xfff << IN_FM_H_BIT )

#define IN_FM_W(val)				((val) << IN_FM_W_BIT)
#define IN_FM_H(val)				((val) << IN_FM_H_BIT)

// Input UV Data Line Stride Register
#define U_S_BIT						( 16 )
#define U_S_MASK					( 0xfff << U_S_BIT )

#define V_S_BIT						( 0 )
#define V_S_MASK					( 0xfff << V_S_BIT )

#define U_STRIDE(val)				((val) << U_S_BIT)
#define V_STRIDE(val)				((val) << V_S_BIT)

// Output Geometric Size Register
#define OUT_FM_W_BIT				( 16 )
#define OUT_FM_W_MASK				( 0xfff << OUT_FM_W_BIT )

#define OUT_FM_H_BIT				( 0 )
#define OUT_FM_H_MASK				( 0xfff << OUT_FM_H_BIT )

#define OUT_FM_W(val)				((val) << OUT_FM_W_BIT)
#define OUT_FM_H(val)				((val) << OUT_FM_H_BIT)

// Resize Coefficients Table Index Register
#define HE_IDX_W_BIT				( 16 )
#define HE_IDX_W_MASK				( 0x1f << HE_IDX_W_BIT )

#define VE_IDX_H_BIT				( 0 )
#define VE_IDX_H_MASK				( 0x1f << VE_IDX_H_BIT )

// common parameter Resize Coefficients Look Up Table Register group
#define OUT_EN_BIT					( 0 )
#define IN_EN_BIT					( 1 )
#define W_COEF_BIT					( 2 )
#define W_COEF_MASK					(0x3ff << W_COEF_BIT)
#define LUT_START					(1 << 12)

// CSC Offset Parameter Register
#define CHROM_OF_W_BIT				( 16 )
#define CHROM_OF_W_MASK				( 0x1f << CHROM_OF_W_BIT )
#define CHROM(x)					(x << CHROM_OF_W_BIT)

#define LUMA_OF_H_BIT				( 0 )
#define LUMA_OF_H_MASK				( 0x1f << LUMA_OF_H_BIT )
#define LUMA(x)						(x << LUMA_OF_H_BIT)

#ifndef __MIPS_ASSEMBLER

typedef struct
{
	unsigned int	coef;
	unsigned short	in_n;
	unsigned short	out_n;
} rsz_lut;

typedef volatile union _IPU_CTRL
{
	struct
	{
		unsigned int chip_en		:1;
		unsigned int ipu_run		:1;
		unsigned int hrsz_en		:1;
		unsigned int vrsz_en		:1;
		unsigned int csc_en			:1;
		unsigned int fm_irq_en		:1;
		unsigned int ipu_rst		:1;
		unsigned int rsv0			:1;
		unsigned int h_scale		:1;
		unsigned int v_scale		:1;
		unsigned int spkg_sel		:1;
		unsigned int lcdc_sel		:1;
		unsigned int spage_map		:1;
		unsigned int dpage_map		:1;
		unsigned int disp_sel		:1;
		unsigned int field_conf_en	:1;
		unsigned int field_sel		:1;
		unsigned int dfix_sel		:1;
		unsigned int rsv1			:14;
	}BITS;

	unsigned int value;
}IPU_CTRL, *PIPU_CTRL;

typedef volatile struct _IPU_CTRL2
{
	unsigned int chip_en		:1;
	unsigned int ipu_run		:1;
	unsigned int hrsz_en		:1;
	unsigned int vrsz_en		:1;
	unsigned int csc_en			:1;
	unsigned int fm_irq_en		:1;
	unsigned int ipu_rst		:1;
	unsigned int rsv0			:1;
	unsigned int h_scale		:1;
	unsigned int v_scale		:1;
	unsigned int spkg_sel		:1;
	unsigned int lcdc_sel		:1;
	unsigned int spage_map		:1;
	unsigned int dpage_map		:1;
	unsigned int disp_sel		:1;
	unsigned int field_conf_en	:1;
	unsigned int field_sel		:1;
	unsigned int dfix_sel		:1;
	unsigned int rsv1			:14;
}IPU_CTRL2, *PIPU_CTRL2;


typedef volatile union _IPU_D_FMT
{
	struct
	{
		unsigned int in_fmt			:2;
		unsigned int in_oft			:2;
		unsigned int rsv0			:12;
		unsigned int pkg_out_oft	:3;
		unsigned int out_fmt		:2;
		unsigned int rgb_out_oft	:3;
		unsigned int out_ftm_24b	:1;
		unsigned int rsv1			:17;
	}BITS;

	unsigned int value;
}IPU_DFMT, *PIPU_DFMT;

typedef volatile struct _IPU_D_FMT2
{
	unsigned int in_fmt			:2;
	unsigned int in_oft			:2;
	unsigned int rsv0			:12;
	unsigned int pkg_out_oft	:3;
	unsigned int out_fmt		:2;
	unsigned int rgb_out_oft	:3;
	unsigned int out_ftm_24b	:1;
	unsigned int rsv1			:17;
}IPU_DFMT2, *PIPU_DFMT2;

typedef volatile struct
{
	unsigned int IPUCONTROL;				// 0x00
	unsigned int IPUSTATUS;
	unsigned int IPUD_FMT;
	unsigned int IPUY_ADDR;
	unsigned int IPUU_ADDR;
	unsigned int IPUV_ADDR;
	unsigned int IPUIN_FM_GS;
	unsigned int IPUY_STRIDE;
	unsigned int IPUUV_STRIDE;
	unsigned int IPUOUT_ADDR;
	unsigned int IPUOUT_GS;
	unsigned int IPUOUT_STRIDE;
	unsigned int IPURSZ_COEF_INDEX;
	unsigned int IPUCSC_C0_COEF;
	unsigned int IPUCSC_C1_COEF;
	unsigned int IPUCSC_C2_COEF;
	unsigned int IPUCSC_C3_COEF;
	unsigned int IPUCSC_C4_COEF;
	unsigned int IPUHRSZ_COEF_LUT;
	unsigned int IPUVRSZ_COEF_LUT;
	unsigned int IPUCSC_OFSET_PARA;
	unsigned int IPUY_PHY_T_ADDR;
	unsigned int IPUU_PHY_T_ADDR;
	unsigned int IPUV_PHY_T_ADDR;
	unsigned int IPUOUT_PHY_T_ADDR;			// 0x60

}JZ4755_IPU, *PJZ4755_IPU;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755IPU_H__

