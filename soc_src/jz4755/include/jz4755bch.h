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
 *  Author:   <zzhang@ingenic.cn> <xyzhang@ingenic.cn> 
 *
 *  Create:   2008-09-18, by zzhang
 *
 *  Maintain: 2008-09-24, by xyzhang
 *
 *******************************************************************************
 */

#ifndef __JZ4755BCH_H__
#define __JZ4755BCH_H__


//--------------------------------------------------------------------------
// BCH Controller (BCH) Address Definition
//--------------------------------------------------------------------------
#define BCH_PHYS_ADDR				( 0x130D0000 )
#define BCH_BASE_U_VIRTUAL			( 0xB30D0000 )


//--------------------------------------------------------------------------
// BCH Registers Offset Definition
//--------------------------------------------------------------------------
#define BCH_BHCR_OFFSET				( 0x00 )	//  R, 32, 0x00000000, BCH Control register
#define BCH_BHCSR_OFFSET			( 0x04 )	//  W, 32, 0x????????, BCH Control Set register
#define BCH_BHCCR_OFFSET			( 0x08 )	//  W, 32, 0x????????, BCH Control Clear register
#define BCH_BHCNT_OFFSET			( 0x0C )	// RW, 32/16, 0x00000000, BCH ENC/DEC Count register
#define BCH_BHDR_OFFSET				( 0x10 )	//  W, 8, 0x????????, BCH data register
#define BCH_BHPAR0_OFFSET			( 0x14 )	// RW, 32/18/8, 0x00000000, BCH Parity 0 register
#define BCH_BHPAR1_OFFSET			( 0x18 )	// RW, 32/18/8, 0x00000000, BCH Parity 1 register
#define BCH_BHPAR2_OFFSET			( 0x1C )	// RW, 32/18/8, 0x00000000, BCH Parity 2 register
#define BCH_BHPAR3_OFFSET			( 0x20 )	// RW, 32/18/8, 0x00000000, BCH Parity 3 register
#define BCH_BHINTS_OFFSET			( 0x24 )	//  R, 32, 0x00000000, BCH Interrupt Status register
#define BCH_BHERR0_OFFSET			( 0x28 )	//  R, 32/16, 0x00000000, BCH Error Report 0 register
#define BCH_BHERR1_OFFSET			( 0x2C )	//  R, 32/16, 0x00000000, BCH Error Report 1 register
#define BCH_BHERR2_OFFSET			( 0x30 )	//  R, 32/16, 0x00000000, BCH Error Report 2 register
#define BCH_BHERR3_OFFSET			( 0x34 )	//  R, 32/16, 0x00000000, BCH Error Report 3 register
#define BCH_BHINTE_OFFSET			( 0x38 )	// RW, 32, 0x00000000, BCH Interrupt Enable register
#define BCH_BHINTES_OFFSET			( 0x3c )	//  W, 32, 0x00000000, BCH Interrupt Set register
#define BCH_BHINTEC_OFFSET			( 0x40 )	//  W, 32, 0x????????, BCH Interrupt Clear register



//--------------------------------------------------------------------------
// BCH Registers Address Definition
//--------------------------------------------------------------------------
 
 
#define A_BCH_BHCR				( BCH_BASE_U_VIRTUAL + BCH_BHCR_OFFSET    )
#define A_BCH_BHCSR				( BCH_BASE_U_VIRTUAL + BCH_BHCSR_OFFSET	  )
#define A_BCH_BHCCR				( BCH_BASE_U_VIRTUAL + BCH_BHCCR_OFFSET	  )
#define A_BCH_BHCNT				( BCH_BASE_U_VIRTUAL + BCH_BHCNT_OFFSET	  )
#define A_BCH_BHDR				( BCH_BASE_U_VIRTUAL + BCH_BHDR_OFFSET	  )
#define A_BCH_BHPAR0			( BCH_BASE_U_VIRTUAL + BCH_BHPAR0_OFFSET  )
#define A_BCH_BHPAR1			( BCH_BASE_U_VIRTUAL + BCH_BHPAR1_OFFSET  )
#define A_BCH_BHPAR2			( BCH_BASE_U_VIRTUAL + BCH_BHPAR2_OFFSET  )
#define A_BCH_BHPAR3			( BCH_BASE_U_VIRTUAL + BCH_BHPAR3_OFFSET  )
#define A_BCH_BHINTS			( BCH_BASE_U_VIRTUAL + BCH_BHINTS_OFFSET  )
#define A_BCH_BHERR0			( BCH_BASE_U_VIRTUAL + BCH_BHERR0_OFFSET  )
#define A_BCH_BHERR1			( BCH_BASE_U_VIRTUAL + BCH_BHERR1_OFFSET  )
#define A_BCH_BHERR2			( BCH_BASE_U_VIRTUAL + BCH_BHERR2_OFFSET  )
#define A_BCH_BHERR3			( BCH_BASE_U_VIRTUAL + BCH_BHERR3_OFFSET  )
#define A_BCH_BHINTE			( BCH_BASE_U_VIRTUAL + BCH_BHINTE_OFFSET  )
#define A_BCH_BHINTES			( BCH_BASE_U_VIRTUAL + BCH_BHINTES_OFFSET )
#define A_BCH_BHINTEC			( BCH_BASE_U_VIRTUAL + BCH_BHINTEC_OFFSET )

//--------------------------------------------------------------------------
// BCH Control register(BHCR) Common Define
//--------------------------------------------------------------------------
#define BCH_BDMA_BIT		4
#define BCH_ENCE_BIT		3
#define BCH_BSEL_BIT		2
#define BCH_BRST_BIT		1
#define BCH_BCHE_BIT		0

#define BCH_BDMA			( 1 << BCH_BDMA_BIT )
#define BCH_ENCE			( 1 << BCH_ENCE_BIT )
#define BCH_DNCE			( 1 << BCH_ENCE_BIT )
#define BCH_BSEL			( 1 << BCH_BSEL_BIT )
#define BCH_BRST			( 1 << BCH_BRST_BIT )
#define BCH_BCHE			( 1 << BCH_BCHE_BIT )
//--------------------------------------------------------------------------
// BCH ENC/DEC Count Register (BHCNT) Common Define
//--------------------------------------------------------------------------

#define BCH_DEC_BIT			( 16 )
#define BCH_ENC_BIT			( 0 )

//--------------------------------------------------------------------------
// BCH Interrupt Status Register (BHINT) Common Define
//--------------------------------------------------------------------------
#define BCH_INTS_ERRC_BIT	( 28 )
#define BCH_INTS_ERRC_MASK	( 0xF << BCH_INTS_ERRC_BIT)
#define BCH_INTS_ALL_0		( 1 << 5 )
#define BCH_INTS_ALL_F		( 1 << 4 )
#define BCH_INTS_DECF		( 1 << 3 )
#define BCH_INTS_ENCF		( 1 << 2 )
#define BCH_INTS_UNCOR		( 1 << 1 )
#define BCH_INTS_ERR		( 1 << 0 )

//--------------------------------------------------------------------------
// BCH Error Report Register (BCHERRn, n=0,1,2,3) Common Define
//--------------------------------------------------------------------------
#define BCH_INDEX_BIT			( 16 )
#define BCH_INDEX_MASK			( 0x1FF << BCH_INDEX_BIT )

#define BCH_MASK_BIT			( 0 )
#define BCH_MASK_MASK			( 0x1FF << BCH_MASK_BIT)

#define BCH_ERR_INDEX_ODD_BIT	16
#define BCH_ERR_INDEX_ODD_MASK	(0x1fff << BCH_ERR_INDEX_ODD_BIT)
#define BCH_ERR_INDEX_EVEN_BIT	0
#define BCH_ERR_INDEX_EVEN_MASK	(0x1fff << BCH_ERR_INDEX_EVEN_BIT)


#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{   
	
	unsigned int	BHCR;		//0x00
	unsigned int	BHCSR;	    //0x04
	unsigned int	BHCCR;      //0x08
	unsigned int	BHCNT;	    //0x0C
	unsigned char	BHDR;	    //0x10
	unsigned char	TCURSV[3];
	unsigned int	BHPAR0;	    //0x14
	unsigned int	BHPAR1;	    //0x18
	unsigned int	BHPAR2;	    //0x1C
	unsigned int	BHPAR3;	    //0x20
	unsigned int	BHINTS;	    //0x24,32,R
	unsigned int	BHERR0;	    //0x28
	unsigned int	BHERR1;     //0x2C
	unsigned int	BHERR2;	    //0x30
	unsigned int	BHERR3;	    //0x34
	unsigned int	BHINTE;	    //0x38
	unsigned int	BHINTES;    //0x3c
	unsigned int	BHINTEC;    //0x40

} JZ4755_BCH, *PJZ4755_BCH;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755BCH_H__
