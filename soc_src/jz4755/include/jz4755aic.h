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
 *  Maintain: 2008-09-23, by hyzhang
 *            
 *
 *******************************************************************************
 */

#ifndef __JZ4755AIC_H__
#define __JZ4755AIC_H__

//--------------------------------------------------------------------------
// AC97 and I2S Controller(AIC) Address Definition
//--------------------------------------------------------------------------
#define AIC_PHYS_ADDR			( 0x10020000 )
#define AIC_BASE_U_VIRTUAL		( 0xB0020000 )

//--------------------------------------------------------------------------
// AIC Registers Offset Definition
//--------------------------------------------------------------------------
#define AIC_AICFR_OFFSET		( 0x00 )        // 32, R/W, AIC Controller Configuration Register
#define AIC_AICCR_OFFSET		( 0x04 )        // 32, R/W, AIC Controller Common Control Register
#define AIC_ACCR1_OFFSET		( 0x08 )        // 32, R/W, AIC Controller AC-link Control Register 1
#define AIC_ACCR2_OFFSET		( 0x0C )        // 32, R/W, AIC Controller AC-link Control Register 2
#define AIC_I2SCR_OFFSET		( 0x10 )        // 32, R/W, AIC Controller I2S/MSB-justified Controller Register
#define AIC_AICSR_OFFSET		( 0x14 )        // 32, R/W, AIC Controller FIFO Status Register
#define AIC_ACSR_OFFSET			( 0x18 )        // 32, R/W, AIC Controller AC-link Status Register
#define AIC_I2SSR_OFFSET		( 0x1C )        // 32, R/W, AIC Controller I2S/MSB-justified Status Register

#define AIC_ACCAR_OFFSET		( 0x20 )        // 32, R/W, AIC Controller AC97 codec command Address Register
#define AIC_ACCDR_OFFSET		( 0x24 )        // 32, R/W, AIC Controller AC97 codec command Data Register
#define AIC_ACSAR_OFFSET		( 0x28 )        // 32, R,   AIC Controller AC97 codec Status Address Register
#define AIC_ACSDR_OFFSET		( 0x2c )        // 32, R,   AIC Controller AC97 codec Status Data Register
#define AIC_I2SDIV_OFFSET		( 0x30 )        // 32, R/W, AIC Controller I2S/MSB-justified Clock Divider Register
#define AIC_AICDR_OFFSET		( 0x34 )        // 32, R/W, AIC Controller FIFO Data Port Register

#define AIC_CKCFG_OFFSET		( 0xA0 )		// 32, R/W, Clock Configure for the embedded CODEC to AIC
#define AIC_RGADW_OFFSET		( 0xA4 )		// 32, R/W, Address, data in and write command for accessing to 
												//internal registers of embedded CODEC
#define AIC_RGDAT_OFFSET		( 0xA8 )		// 32, R, The read out data and interrupt request status of 
												//Internal registers data in the embedded CODEC.

//--------------------------------------------------------------------------
// AIC Registers Address Definition
//--------------------------------------------------------------------------
#define A_AIC_AICFR			( AIC_BASE_U_VIRTUAL + AIC_AICFR_OFFSET )
#define A_AIC_AICCR			( AIC_BASE_U_VIRTUAL + AIC_AICCR_OFFSET )
#define A_AIC_ACCR1			( AIC_BASE_U_VIRTUAL + AIC_ACCR1_OFFSET )
#define A_AIC_ACCR2			( AIC_BASE_U_VIRTUAL + AIC_ACCR2_OFFSET )
#define A_AIC_I2SCR			( AIC_BASE_U_VIRTUAL + AIC_I2SCR_OFFSET )
#define A_AIC_AICSR			( AIC_BASE_U_VIRTUAL + AIC_AICSR_OFFSET )
#define A_AIC_ACSR			( AIC_BASE_U_VIRTUAL + AIC_ACSR_OFFSET )
#define A_AIC_I2SSR			( AIC_BASE_U_VIRTUAL + AIC_I2SSR_OFFSET )

#define A_AIC_ACCAR			( AIC_BASE_U_VIRTUAL + AIC_ACCAR_OFFSET )
#define A_AIC_ACCDR			( AIC_BASE_U_VIRTUAL + AIC_ACCDR_OFFSET )
#define A_AIC_ACSAR			( AIC_BASE_U_VIRTUAL + AIC_ACSAR_OFFSET )
#define A_AIC_ACSDR			( AIC_BASE_U_VIRTUAL + AIC_ACSDR_OFFSET )
#define A_AIC_I2SDIV		( AIC_BASE_U_VIRTUAL + AIC_I2SDIV_OFFSET )
#define A_AIC_AICDR			( AIC_BASE_U_VIRTUAL + AIC_AICDR_OFFSET )

#define A_AIC_CKCFG			( AIC_BASE_U_VIRTUAL + AIC_CKCFG_OFFSET )
#define A_AIC_RGADW		 	( AIC_BASE_U_VIRTUAL + AIC_RGADW_OFFSET )
#define A_AIC_RGDAT			( AIC_BASE_U_VIRTUAL + AIC_RGDAT_OFFSET )

//--------------------------------------------------------------------------
// AICFR Common Define
//--------------------------------------------------------------------------
#define AICFR_RFTH_MASK			( 0xF << 12 )
#define AICFR_RFTH(x)			( ( (x)/2 - 1 ) << 12 )		// 2, 4, ..., 32

#define AICFR_TFTH_MASK			( 0xF << 8 )
#define AICFR_TFTH(x)			( ( (x)/2 ) << 8 )		// 2, 4, ..., 32

#define	AICFR_LSMP_LAST_SAMPLE	( 1 << 6 )
#define	AICFR_LSMP_ZERO_SAMPLE	( 0 << 6 )
#define	AICFR_CODEC_INT			( 1 << 5 )
#define	AICFR_CODEC_EXT			( 0 << 5 )
#define	AICFR_I2S				( 1 << 4 )
#define	AICFR_ACLINK			( 0 << 4 )
#define	AICFR_RESET				( 1 << 3 )
#define	AICFR_BCKD_OUT			( 1 << 2 )
#define	AICFR_BCKD_IN			( 0 << 2 )
#define	AICFR_SYNCD_OUT			( 1 << 1 )
#define	AICFR_SYNCD_IN			( 0 << 1 )
#define	AICFR_ENABLE			( 1 << 0 )

//--------------------------------------------------------------------------
// AICCR Common Define
//--------------------------------------------------------------------------
#define	AICCR_OSS_MASK			( 0x7 << 19 )
#define	AICCR_OSS_8BIT			( 0x0 << 19 )
#define	AICCR_OSS_16BIT			( 0x1 << 19 )
#define	AICCR_OSS_18BIT			( 0x2 << 19 )
#define	AICCR_OSS_20BIT			( 0x3 << 19 )
#define	AICCR_OSS_24BIT			( 0x4 << 19 )
#define	AICCR_ISS_MASK				( 0x7 << 16 )
#define	AICCR_ISS_8BIT				( 0x0 << 16 )
#define	AICCR_ISS_16BIT				( 0x1 << 16 )
#define	AICCR_ISS_18BIT				( 0x2 << 16 )
#define	AICCR_ISS_20BIT				( 0x3 << 16 )
#define	AICCR_ISS_24BIT				( 0x4 << 16 )
#define	AICCR_RDMS				( 1 << 15 )
#define	AICCR_TDMS				( 1 << 14 )
#define	AICCR_M2S				( 1 << 11 )
#define	AICCR_ENDSW				( 1 << 10 )
#define	AICCR_ASVTSU				( 1 << 9 )
#define	AICCR_FLUSH_FIFO			( 0x3 << 7 )
#define	AICCR_FLUSH_RFIFO			( 1 << 8 )
#define	AICCR_FLUSH_TFIFO			( 1 << 7 )
#define	AICCR_EROR				( 1 << 6 )
#define	AICCR_ETUR				( 1 << 5 )
#define	AICCR_ERFS				( 1 << 4 )
#define	AICCR_ETFS				( 1 << 3 )
#define	AICCR_LOOPBACK			( 1 << 2 )
#define	AICCR_PLAYBACK			( 1 << 1 )
#define	AICCR_RECORD			( 1 << 0 )

//--------------------------------------------------------------------------
// ACCR1 Common Define
//--------------------------------------------------------------------------
#define	ACCR1_RS_SLOT(x)		( 1 << ((x) + 13) )  // Receive Slot n has valid PCM data(3 < x)
#define	ACCR1_XS_SLOT(x)		( 1 << ((x) - 3) )  // Transmit Slot n has valid PCM data(3 < x)

//--------------------------------------------------------------------------
// ACCR2 Common Define
//--------------------------------------------------------------------------
#define ACCR2_ERSTO			( 1 << 18 )  // Enable RSTO interrupt function
#define ACCR2_ESADR			( 1 << 17 )  // Enable SADR interrupt function
#define ACCR2_ECADT			( 1 << 16 )  // Enable ECADT interrupt function
#define ACCR2_SO			( 1 << 3 )   // SDATA_OUT output value
#define ACCR2_SR			( 1 << 2 )   // Specify RESET# signal, drive RESET# pin of AIC 
#define ACCR2_SS			( 1 << 1 )   // When Read, It describes the actual value of SYNC
                                             // When Write, this bit control the value of SYNC signal
#define ACCR2_SA			( 1 << 0 )   // SYNC Alternation.

//--------------------------------------------------------------------------
// I2SCR Common Define
//--------------------------------------------------------------------------
#define I2SCR_STPBK				( 1 << 12 )
#define I2SCR_ESCLK				( 1 << 4 )
#define I2SCR_MSBJUSTIFIED		( 1 << 0 )
#define I2SCR_I2S				( 0 << 0 )

//--------------------------------------------------------------------------
// AICSR Common Define
//--------------------------------------------------------------------------
#define AICSR_RFL_MASK			( 0x3F << 24 )
#define AICSR_TFL_MASK			( 0x3F << 8 )

#define AICSR_ROR			( 1 << 6 )
#define AICSR_TUR			( 1 << 5 )
#define AICSR_RFS			( 1 << 4 )
#define AICSR_TFS			( 1 << 3 )

//--------------------------------------------------------------------------
// ACSR Common Define
//--------------------------------------------------------------------------
#define ACSR_SLTERR			( 1 << 21 )
#define ACSR_CRDY			( 1 << 20 )
#define ACSR_CLPM			( 1 << 19 )
#define ACSR_RSTO			( 1 << 18 )
#define ACSR_SADR			( 1 << 17 )
#define ACSR_CADT			( 1 << 16 )

//--------------------------------------------------------------------------
// I2SSR Common Define
//--------------------------------------------------------------------------
#define I2SSR_BSY			( 1 << 2 )

//--------------------------------------------------------------------------
// ACCAR Common Define
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// ACCDR Common Define
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// ACSAR Common Define
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// ACSDR Common Define
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// I2SDIV Common Define
//--------------------------------------------------------------------------
#define AIC_I2SDIV_MASK		(0xf << 0)

//--------------------------------------------------------------------------
// CKCFG Common Define
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// RGADW Common Define
//--------------------------------------------------------------------------
#define AIC_RGADW_RGWR				(1 << 16)

#define RGADW_ADDR_BIT		( 8 )
#define RGADW_DATA_BIT		( 0 )

#define AIC_RGADW_RGADDR_MASK 		(0x7F << RGADW_ADDR_BIT)
#define AIC_RGADW_RGADDR(x)			((x) << RGADW_ADDR_BIT)
#define AIC_RGADW_RGDIN_MASK		(0xFF << RGADW_DATA_BIT)
#define AIC_RGADW_RGDIN(x)			((x) << RGADW_DATA_BIT)


#define	AIC_RW_CODEC_START()	while (INREG32(A_AIC_RGADW) & AIC_RGADW_RGWR)
#define	AIC_RW_CODEC_STOP()		while (INREG32(A_AIC_RGADW) & AIC_RGADW_RGWR)

//--------------------------------------------------------------------------
// RGDAT Common Define
//--------------------------------------------------------------------------
#define AIC_RGDAT_IRQ				(1    << 8)
#define AIC_RGDAT_RGDOUT_MASK		(0xff << 0)
#define AIC_RGDAT_RGDOUT(x)			((x) << 0)


//--------------------------------------------------------------------------
// GPIO Common Define
//--------------------------------------------------------------------------
#define AIC_GPIO_PIN			(GPIO_22 | GPIO_23 | GPIO_24)

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	AICFR;	// 0,  32 R/W
	unsigned int    AICCR;	// 4,  32 R/W
	unsigned int	ACCR1;	// 8,  32 R/W
	unsigned int	ACCR2;	// C,  32 R/W
	unsigned int	I2SCR;	// 10, 32 R/W
	unsigned int	AICSR;	// 14, 32 R/W
	unsigned int	ACSR;	// 18, 32 R/W
	unsigned int	I2SSR;	// 1C, 32 R/W
	unsigned int    ACCAR;	// 20, 32 R/W
	unsigned int	ACCDR;	// 24, 32 R/W
	unsigned int	ACSAR;	// 28, 32 R  
	unsigned int	ACSDR;	// 2C, 32 R  
	unsigned int	I2SDIV;	// 30, 32 R/W
	unsigned int	AICDR;	// 34, 32 R/W	
	unsigned int	AICRSV00[26];	// 38, 3C, 32 R/W

	unsigned int	CKCFG;		// 0xA0, 32 R/W
	unsigned int	RGADW;		// 0xA4, 32 R/W
	unsigned int	RGDAT;		// 0xA8, 32 R
} JZ4755_AIC, *PJZ4755_AIC;

#endif // __MIPS_ASSEMBLER

#endif //__JZ4755AIC_H__
