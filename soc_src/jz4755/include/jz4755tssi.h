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
 *  Create:   2008-09-23, by jbyu
 *
 *  Maintain: 2008-12-19, by jgao
 *
 *******************************************************************************
 */

#ifndef __JZ4755TSSI_H__
#define __JZ4755TSSI_H__

//--------------------------------------------------------------------------
// TSSI Controller(TSSI) Address Definition
//--------------------------------------------------------------------------
#define TSSI_PHYS_ADDR			( 0x10073000 )
#define TSSI_BASE_U_VIRTUAL		( 0xB0073000 )

//--------------------------------------------------------------------------
// TSSI Registers Offset Definition
//--------------------------------------------------------------------------
#define TVSSI_TSENA_OFFSET		(0x00)	// RW, 8
#define TVSSI_TSCFG_OFFSET		(0x04)	// RW, 16
#define TVSSI_TSCTRL_OFFSET		(0x08)	// RW, 8
#define TVSSI_TSSTAT_OFFSET		(0x0C)	// RW, 8
#define TVSSI_TSFIFO_OFFSET		(0x10)	// R, 32
#define TVSSI_TSPEN_OFFSET		(0x14)	// RW, 32
#define TVSSI_TSPID0_OFFSET		(0x20)	// RW, 32
#define TVSSI_TSPID1_OFFSET		(0x24)	// RW, 32
#define TVSSI_TSPID2_OFFSET		(0x28)	// RW, 32
#define TVSSI_TSPID3_OFFSET		(0x2C)	// RW, 32
#define TVSSI_TSPID4_OFFSET		(0x30)	// RW, 32
#define TVSSI_TSPID5_OFFSET		(0x34)	// RW, 32
#define TVSSI_TSPID6_OFFSET		(0x38)	// RW, 32
#define TVSSI_TSPID7_OFFSET		(0x3C)	// RW, 32

//--------------------------------------------------------------------------
// TSSI Registers Address Definition
//--------------------------------------------------------------------------
#define A_TSSI_TSENA			(TSSI_BASE_U_VIRTUAL + TVSSI_TSENA_OFFSET)
#define A_TSSI_TSCFG			(TSSI_BASE_U_VIRTUAL + TVSSI_TSCFG_OFFSET)
#define A_TSSI_TSCTRL			(TSSI_BASE_U_VIRTUAL + TVSSI_TSCTRL_OFFSET)
#define A_TSSI_TSSTAT			(TSSI_BASE_U_VIRTUAL + TVSSI_TSSTAT_OFFSET)
#define A_TSSI_TSFIFO			(TSSI_BASE_U_VIRTUAL + TVSSI_TSFIFO_OFFSET)
#define A_TSSI_TSPEN			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPEN_OFFSET)
#define A_TSSI_TSPID(n)         (TSSI_BASE_U_VIRTUAL + TVSSI_TSPID0_OFFSET + 4*(n))  
#define A_TSSI_TSPID0			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID0_OFFSET)
#define A_TSSI_TSPID1			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID1_OFFSET)
#define A_TSSI_TSPID2			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID2_OFFSET)
#define A_TSSI_TSPID3			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID3_OFFSET)
#define A_TSSI_TSPID4			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID4_OFFSET)
#define A_TSSI_TSPID5			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID5_OFFSET)
#define A_TSSI_TSPID6			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID6_OFFSET)
#define A_TSSI_TSPID7			(TSSI_BASE_U_VIRTUAL + TVSSI_TSPID7_OFFSET)

//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------

// TSSI Enable Register (TSENA)
#define TSSI_TSENA_SFT_RST		(1 << 7)
#define TSSI_TSENA_PID_EN		(1 << 2)
#define TSSI_TSENA_DMA_EN		(1 << 1)
#define TSSI_TSENA_ENA			(1 << 0)

// TSSI Configure Register (TSCFG)
#define TSSI_TSCFG_EDN_WD		(1 << 9)
#define TSSI_TSCFG_EDN_BT		(1 << 8)
#define TSSI_TSCFG_TSDI_H		(1 << 7)
#define TSSI_TSCFG_USE_0		(1 << 6)
#define TSSI_TSCFG_TSCLK_CH		(1 << 5)
#define TSSI_TSCFG_PARAL		(1 << 4)
#define TSSI_TSCFG_TSCLK_P		(1 << 3)
#define TSSI_TSCFG_TSFRM_H		(1 << 2)
#define TSSI_TSCFG_TSSTR_H		(1 << 1)
#define TSSI_TSCFG_TSFAIL_H		(1 << 0)

#define TSSI_TSCFG_F_TRIGV4		(0 << 14)	// Trigger Value is 4
#define TSSI_TSCFG_F_TRIGV8		(1 << 14)	// Trigger Value is 8
#define TSSI_TSCFG_F_TRIGV16 	(2 << 14)	// Trigger Value is 16
#define TSSI_TSCFG_F_TRIGVMASK	(3 << 14)	// Reserve

// TSSI Control Register (TSCTRL)
#define TSSI_TSCTRL_F_OVRNM		(1 << 1)
#define TSSI_TSCTRL_F_TRIGM		(1 << 0)

// TSSI State Register (TSSTAT)
#define TSSI_TSSTAT_F_OVRN		(1 << 1)
#define TSSI_TSSTAT_F_TRIG		(1 << 0)

// TSSI PID Enable Register (TSPEN)
#define TSSI_TSPEN_PEN_0		(1 << 31)

#define TSSI_TSPEN_EN_71		(1 << 23)
#define TSSI_TSPEN_EN_61		(1 << 22)
#define TSSI_TSPEN_EN_51		(1 << 21)
#define TSSI_TSPEN_EN_41		(1 << 20)
#define TSSI_TSPEN_EN_31		(1 << 19)
#define TSSI_TSPEN_EN_21		(1 << 18)
#define TSSI_TSPEN_EN_11		(1 << 17)
#define TSSI_TSPEN_EN_01		(1 << 16)

#define TSSI_TSPEN_EN_70		(1 << 7)
#define TSSI_TSPEN_EN_60		(1 << 6)
#define TSSI_TSPEN_EN_50		(1 << 5)
#define TSSI_TSPEN_EN_40		(1 << 4)
#define TSSI_TSPEN_EN_30		(1 << 3)
#define TSSI_TSPEN_EN_20		(1 << 2)
#define TSSI_TSPEN_EN_10		(1 << 1)
#define TSSI_TSPEN_EN_00		(1 << 0)

// TSSI PID Filter Registers (TSPID0~7)
#define TSSI_TSPID_PID1			(1 << 16)
#define TSSI_TSPID_PID0			(1 << 0)
#define TSSI_TSPID_PID1_MASK 	(0x1FFF<< 16)
#define TSSI_TSPID_PID0_MASK 	(0x1FFF<< 0)



#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned char	TSENA;			// 0x00
	unsigned char	TSSIRSV0[3];
	unsigned short	TSCFG;			// 0x04
	unsigned short	TSSIRSV1[1];
	unsigned char	TSCTRL;			// 0x08
	unsigned char	TSSIRSV2[3];
	unsigned char	TSSTAT;			// 0x0C
	unsigned char	TSSIRSV3[1];
	unsigned int	TSFIFO;			// 0x10
	unsigned int	TSPEN;			// 0x14
	unsigned int	TSSIRSV4[2];	// 0x18,0x1C
	unsigned int	TSPID0;			// 0x20
	unsigned int	TSPID1;			// 0x24
	unsigned int	TSPID2;			// 0x28
	unsigned int	TSPID3;			// 0x2C
	unsigned int	TSPID4;			// 0x30
	unsigned int	TSPID5;			// 0x34
	unsigned int	TSPID6;			// 0x38
	unsigned int	TSPID7;			// 0x3C

} JZ4755_TSSI, *PJZ4755_TSSI;

#endif //__MIPS_ASSEMBLER

#endif // __JZ4755TSSI_H__
