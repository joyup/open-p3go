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
 *  Create:   2008-11-14, by whxu, create
 *
 ****************************************************************************/

#ifndef __JZ4755AHB_H__
#define __JZ4755AHB_H__

#define AHB_PHYS_ADDR				( 0x13000000 )
#define AHB_BASE_U_VIRTUAL			( 0xB3000000 )

//--------------------------------------------------------------------------
// AHB Registers Offset Definition
//--------------------------------------------------------------------------
#define AHB_RPIOR_OFFSET			( 0x00 )	//	RW, 32, 0x00000000
#define AHB_CTRL_OFFSET				( 0x04 )	//	RW, 32, 0x00000000
#define AHB_CLKL_OFFSET				( 0x08 )	//	RW, 32, 0x00000000
#define AHB_EVENT0L_OFFSET			( 0x0C )	//	RW, 32, 0x00000000
#define AHB_EVENT1L_OFFSET			( 0x10 )	//	RW, 32, 0x00000000
#define AHB_EVENTH_OFFSET			( 0x14 )	//	RW, 32, 0x00000000


//--------------------------------------------------------------------------
// AHB Registers Address Definition
//--------------------------------------------------------------------------
 
#define A_AHB_RPIOR			( AHB_BASE_U_VIRTUAL + AHB_RPIOR_OFFSET )
#define A_AHB_CTRL			( AHB_BASE_U_VIRTUAL + AHB_CTRL_OFFSET )
#define A_AHB_CLKL			( AHB_BASE_U_VIRTUAL + AHB_CLKL_OFFSET )
#define A_AHB_EVENT0L		( AHB_BASE_U_VIRTUAL + AHB_EVENT0L_OFFSET )
#define A_AHB_EVENT1L		( AHB_BASE_U_VIRTUAL + AHB_EVENT1L_OFFSET )
#define A_AHB_EVENTTH		( AHB_BASE_U_VIRTUAL + AHB_EVENTH_OFFSET )


//--------------------------------------------------------------------------
// AHB Control Register field descriptions
//--------------------------------------------------------------------------
#define AHB_CTRL_M1_BIT		( 20 )
#define AHB_CTRL_M0_BIT		( 16 )

#define AHB_CTRL_EV1_BIT	( 12 )
#define AHB_CTRL_EV0_BIT	( 8 )

#define AHB_CTRL_EV1_EN		( 1 << 2 )
#define AHB_CTRL_EV0_EN		( 1 << 1 )
#define AHB_CTRL_CLOCK_EN	( 1 << 0 )

#define	MEVENT_BUS_TRANS_CYCLE		( 0 )
#define	MEVENT_BUS_TRANS_TIMES		( 1 )
#define	MEVENT_GRANT_LATENCY		( 2 )


#define	MASTER_CPU		( 0 )
#define	MASTER_CIM		( 1 )
#define	MASTER_LCD		( 2 )
#define	MASTER_DMA		( 3 )
#define	MASTER_IPU		( 4 )
#define	MASTER_USB		( 5 )
#define	MASTER_AHB1_0BRIDGE		( 6 )
#define	MASTER_MC_BRIDGE		( 7 )

#define	MASTER_ALL		( 15 )

#ifndef __MIPS_ASSEMBLER

//----------------------------------------------------------------------------
// Common macro
//----------------------------------------------------------------------------
#define	MONITOR_CONTROL(m0, ev0, m1, ev1)	\
	OUTREG32(A_AHB_CTRL, (((m0)<<16) | ((ev0)<<8) | ((m1)<<20) | ((ev1)<<12)))

#define	MONITOR_START(m0, ev0, m1, ev1)				\
do {												\
	MONITOR_CONTROL(m0, ev0, m1, ev1);				\
	OUTREG32(A_AHB_CLKL, 0x00000000);				\
	OUTREG32(A_AHB_EVENT0L, 0x00000000);			\
	OUTREG32(A_AHB_EVENT1L, 0x00000000);			\
	OUTREG32(A_AHB_EVENTTH, 0x00000000);			\
	SETREG32(A_AHB_CTRL, AHB_CTRL_EV1_EN | AHB_CTRL_EV0_EN | AHB_CTRL_CLOCK_EN);\
} while (0)

#define	MONITOR_LOG()									\
do {													\
	unsigned int ev0 = INREG32(A_AHB_EVENT0L);			\
	unsigned int ev1 = INREG32(A_AHB_EVENT1L);			\
	unsigned int clk = INREG32(A_AHB_CLKL);				\
	unsigned int high = INREG32(A_AHB_EVENTTH);			\
	OUTREG32(A_AHB_CTRL, 0x00000000);					\
	printf("%d %d %d  %d %d %d\n", ev0, ev1, clk, high & 0xFF, (high >> 8) & 0xFF, high >> 16);		\
} while (0)


#endif	//	__MIPS_ASSEMBLER

#endif	//__JZ4755AHB_H__
