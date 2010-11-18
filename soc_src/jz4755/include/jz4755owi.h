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
 *******************************************************************************
 */

#ifndef __JZ4755OWI_H__
#define __JZ4755OWI_H__

//--------------------------------------------------------------------------
// TSSI Controller(TSSI) Address Definition
//--------------------------------------------------------------------------
#define OWI_PHYS_ADDR			( 0x10072000 )
#define OWI_BASE_U_VIRTUAL		( 0xB0072000 )

//--------------------------------------------------------------------------
// TSSI Registers Offset Definition
//--------------------------------------------------------------------------
#define OWI_OWCFG_OFFSET		(0x00)	// RW, 8
#define OWI_OWCTL_OFFSET		(0x04)	// RW, 8
#define OWI_OWSTS_OFFSET		(0x08)	// RW, 8
#define OWI_OWDAT_OFFSET		(0x0C)	// RW, 8
#define OWI_OWDIV_OFFSET		(0x10)	// RW, 8

//--------------------------------------------------------------------------
// TSSI Registers Address Definition
//--------------------------------------------------------------------------
#define A_OWI_OWCFG				(OWI_BASE_U_VIRTUAL + OWI_OWCFG_OFFSET)
#define A_OWI_OWCTL				(OWI_BASE_U_VIRTUAL + OWI_OWCTL_OFFSET)
#define A_OWI_OWSTS				(OWI_BASE_U_VIRTUAL + OWI_OWSTS_OFFSET)
#define A_OWI_OWDAT				(OWI_BASE_U_VIRTUAL + OWI_OWDAT_OFFSET)
#define A_OWI_OWDIV				(OWI_BASE_U_VIRTUAL + OWI_OWDIV_OFFSET)

//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------

// One-Wire Configure Register (OWCFG)
#define OWI_OWCFG_MODE			(1 << 7)
#define OWI_OWCFG_RDDATA		(1 << 6)
#define OWI_OWCFG_WRDATA		(1 << 5)
#define OWI_OWCFG_RDST			(1 << 4)
#define OWI_OWCFG_WR1			(1 << 3)
#define OWI_OWCFG_WR0			(1 << 2)
#define OWI_OWCFG_RST			(1 << 1)
#define OWI_OWCFG_ENA			(1 << 0)

// One-Wire Control Register (OWCTL)
#define OWI_OWCTL_EBYTE			(1 << 2)
#define OWI_OWCTL_EBIT			(1 << 1)
#define OWI_OWCTL_ERST			(1 << 0)

// One-Wire Status Register (OWSTS)
#define OWI_OWSTS_PST			(1 << 7)
#define OWI_OWSTS_BYTE_RDY		(1 << 2)
#define OWI_OWSTS_BIT_RDY		(1 << 1)
#define OWI_OWSTS_PST_RDY		(1 << 0)

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned char	OWCFG;
	unsigned char	OWIRSV0[3];
	unsigned char	OWCTL;
	unsigned char	OWIRSV1[3];
	unsigned char	OWSTS;
	unsigned char	OWIRSV2[3];
	unsigned char	OWDAT;
	unsigned char	OWIRSV3[3];
	unsigned char	OWDIV;
	unsigned char	OWIRSV4[3];

} JZ4755_OWI, *PJZ4755_OWI;

/* owi */
#define __gpio_as_owi()									\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(4), 0x02000000);				\
	OUTREG32(A_GPIO_PXSELS(4), 0x02000000);				\
} while (0)

#endif //__MIPS_ASSEMBLER

#endif // __JZ4755OWI_H__
