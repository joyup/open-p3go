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
 *  Author:  <hyzhang@ingenic.cn>
 *
 *  Create:   2008-09-23, by hyzhang
 *            
 *  Maintain: 2008-10-23, by zhang zhi
 *            
 *
 *******************************************************************************
 */

#ifndef __JZ4755SADC_H__
#define __JZ4755SADC_H__

//--------------------------------------------------------------------------
// SADC Address Definition
//--------------------------------------------------------------------------
#define SADC_PHYS_ADDR			( 0x10070000 )
#define SADC_BASE_U_VIRTUAL		( 0xB0070000 )

//--------------------------------------------------------------------------
// SADC Registers Offset Definition
//--------------------------------------------------------------------------
#define SADC_ADENA_OFFSET		( 0x00 )	// RW,  8, 0x00
#define SADC_ADCFG_OFFSET		( 0x04 )	// RW, 32, 0x0002002C
#define SADC_ADCTRL_OFFSET		( 0x08 )	// RW,  8, 0x00
#define SADC_ADSTATE_OFFSET		( 0x0C )	// RW,  8, 0x00
#define SADC_ADSAME_OFFSET		( 0x10 )	// RW, 16, 0x0000
#define SADC_ADWAIT_OFFSET		( 0x14 )	// RW, 16, 0x0000
#define SADC_ADTCH_OFFSET		( 0x18 )	// RW, 32, 0x00000000
#define SADC_ADBDAT_OFFSET		( 0x1C )	// RW, 16, 0x0000
#define SADC_ADSDAT_OFFSET		( 0x20 )	// RW, 16, 0x0000
#define SADC_ADDPIN_OFFSET		( 0x24 )	// R , 16, 0x0000
#define SADC_ADCLK_OFFSET		( 0x28 )	// RW, 16, 0x0000

//--------------------------------------------------------------------------
// SADC Registers Address Definition
//--------------------------------------------------------------------------
#define A_SADC_ADENA			( SADC_BASE_U_VIRTUAL + SADC_ADENA_OFFSET )
#define A_SADC_ADCFG			( SADC_BASE_U_VIRTUAL + SADC_ADCFG_OFFSET )
#define A_SADC_ADCTRL			( SADC_BASE_U_VIRTUAL + SADC_ADCTRL_OFFSET )
#define A_SADC_ADSTATE			( SADC_BASE_U_VIRTUAL + SADC_ADSTATE_OFFSET )
#define A_SADC_ADSAME			( SADC_BASE_U_VIRTUAL + SADC_ADSAME_OFFSET )
#define A_SADC_ADWAIT			( SADC_BASE_U_VIRTUAL + SADC_ADWAIT_OFFSET )
#define A_SADC_ADTCH			( SADC_BASE_U_VIRTUAL + SADC_ADTCH_OFFSET )
#define A_SADC_ADBDAT			( SADC_BASE_U_VIRTUAL + SADC_ADBDAT_OFFSET )
#define A_SADC_ADSDAT			( SADC_BASE_U_VIRTUAL + SADC_ADSDAT_OFFSET )
#define A_SADC_ADDPIN			( SADC_BASE_U_VIRTUAL + SADC_ADDPIN_OFFSET )
#define A_SADC_ADCLK			( SADC_BASE_U_VIRTUAL + SADC_ADCLK_OFFSET )

//--------------------------------------------------------------------------
// ADENA Common Define
//--------------------------------------------------------------------------
#define ADENA_ADEN			( 1 << 7 )
#define ADENA_ENTR_SLP		( 1 << 6 )
#define ADENA_EXIT_SLP		( 1 << 5 )
#define ADENA_TCHEN			( 1 << 2 )
#define ADENA_PBATEN		( 1 << 1 )
#define ADENA_SADCINEN		( 1 << 0 )

#define	ADENA_ALL_EN		(ADENA_TCHEN | ADENA_PBATEN | ADENA_SADCINEN)

//--------------------------------------------------------------------------
// ADCFG Common Define
//--------------------------------------------------------------------------
#define ADCFG_SPZZ				( 1 << 31 )
#define ADCFG_EXIN				( 1 << 30 )
#define ADCFG_SDELAY_MASK		( 0x7 << 16 )
#define ADCFG_SDELAY(x)			( ((x) - 1) << 16 )
#define ADCFG_DMA_EN			( 1 << 15 )

#define ADCFG_XYZ_MASK			( 0x3 << 13 )
#define ADCFG_XYZ_X_Y			( 0x0 << 13 )
#define ADCFG_XYZ_X_Y_Z			( 0x1 << 13 )
#define ADCFG_XYZ_X_Y_Z1_Z2		( 0x2 << 13 )

#define ADCFG_SNUM_MASK			( 0x7 << 10 )
#define ADCFG_SNUM(x)			( (((x) > 7) ? (x) - 2 : (x) - 1) << 10 )


#define ADCFG_BAT_MD			( 1 << 4 )

#define ADCFG_CMD_MASK			( 0xF << 0 )
#define ADCFG_CMD_1100			( 0xC << 0 )

//--------------------------------------------------------------------------
// ADCTRL Common Define
//--------------------------------------------------------------------------
#define ADCTRL_SLPENDM		( 1 << 5 )
#define ADCTRL_PENDM		( 1 << 4 )
#define ADCTRL_PENUM		( 1 << 3 )
#define ADCTRL_DTCHM		( 1 << 2 )
#define ADCTRL_DRDYM		( 1 << 1 )
#define ADCTRL_SRDYM		( 1 << 0 )

//--------------------------------------------------------------------------
// ADSTATE Common Define
//--------------------------------------------------------------------------
#define ADCTRL_SLPEND		( 1 << 5 )
#define ADSTATE_PEND		( 1 << 4 )
#define ADSTATE_PENU		( 1 << 3 )
#define ADSTATE_DTCH		( 1 << 2 )
#define ADSTATE_DRDY		( 1 << 1 )
#define ADSTATE_SRDY		( 1 << 0 )

//--------------------------------------------------------------------------
// ADSAME Common Define
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// ADWAIT Common Define
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// ADTCH Common Define
//--------------------------------------------------------------------------
#define ADTCH_TYPE1				( 1 << 31 )
#define ADTCH_TDATA1_MASK		( 0xfff << 16 )
#define ADTCH_TDATA1(x)			( x << 16 )
#define ADTCH_TYPE0				( 1 << 15 )
#define ADTCH_TDATA0_MASK		( 0xfff << 0 )
#define ADTCH_TDATA0(x)			( x << 0 )

//--------------------------------------------------------------------------
// ADBDAT Common Define
//--------------------------------------------------------------------------
#define ADBDAT_BDATA_MASK				( 0xfff << 0 )
#define ADBDAT_BDATAk(x)				( x << 0 )

//--------------------------------------------------------------------------
// ADCLK Common Define
//--------------------------------------------------------------------------
#define ADCFG_CLKDIV_BIT		( 0 )
#define ADCFG_CLKDIV_MASK		( 0x3F << ADCFG_CLKDIV_BIT )
#define ADCFG_CLKDIV_10_BIT		( 16 )
#define ADCFG_CLKDIV_10_MASK	( 0x7F << ADCFG_CLKDIV_10_BIT )
//--------------------------------------------------------------------------
// GPIO Common Define
//--------------------------------------------------------------------------

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned char	ADENA;			// 0x00,  8 R/W
	unsigned char	ADENARSV[3];

	unsigned int	ADCFG;			// 0x04,  32 R/W

	unsigned char	ADCTRL;				// 0x08,  8 R/W
	unsigned char	ADCTRLRSV[3];

	unsigned char	ADSTATE;			// 0x0C,  8 R/W
	unsigned char	ADSTATERSV[3];

	unsigned short	ADSAME;			// 0x10,  16 R/W
	unsigned short	ADSAMERSV;

	unsigned short	ADWAIT;			// 0x14,  16 R/W
	unsigned short	ADWAITRSV;

	unsigned int	ADTCH;			// 0x18,  32 R/W

	unsigned short	ADBDAT;			// 0x1C,  16 R/W
	unsigned short	ADBDATRSV;
	
	unsigned short	ADSDAT;			// 0x20,  16 R/W
	unsigned short	ADSDATRSV;
	
	unsigned short	ADDPIN;			// 0x24,  16 R
	unsigned short	ADDPINRSV;
} JZ4755_SADC, *PJZ4755_SADC;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755SADC_H__
