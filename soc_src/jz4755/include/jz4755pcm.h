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

#ifndef __JZ4755PCM_H__
#define __JZ4755PCM_H__

//--------------------------------------------------------------------------
// PCM Address Definition
//--------------------------------------------------------------------------
#define PCM_PHYS_ADDR			( 0x10071000 )
#define PCM_BASE_U_VIRTUAL		( 0xB0071000 )

//--------------------------------------------------------------------------
// PCM Registers Offset Definition
//--------------------------------------------------------------------------
#define PCM_PCMCTL_OFFSET		( 0x00 )        // 32, R/W, PCM Control Register
#define PCM_PCMCFG_OFFSET		( 0x04 )        // 32, R/W, PCM Configure Register
#define PCM_PCMDP_OFFSET		( 0x08 )        // 32, R/W, PCM FIFO Data Port Register
#define PCM_PCMINTC_OFFSET		( 0x0C )        // 32, R/W, PCM Interrupt Control Register
#define PCM_PCMINTS_OFFSET		( 0x10 )        // 32, R/W, PCM Interrupt Status Register
#define PCM_PCMDIV_OFFSET		( 0x14 )        // 32, R/W, PCM Clock Divide Register

//--------------------------------------------------------------------------
// PCM Registers Address Definition
//--------------------------------------------------------------------------
#define A_PCM_PCMCTL			( AIC_BASE_U_VIRTUAL + PCM_PCMCTL_OFFSET )
#define A_PCM_PCMCFG			( AIC_BASE_U_VIRTUAL + PCM_PCMCFG_OFFSET )
#define A_PCM_PCMDP				( AIC_BASE_U_VIRTUAL + PCM_PCMDP_OFFSET )
#define A_PCM_PCMINTC			( AIC_BASE_U_VIRTUAL + PCM_PCMINTC_OFFSET )
#define A_PCM_PCMINTS			( AIC_BASE_U_VIRTUAL + PCM_PCMINTS_OFFSET )
#define A_PCM_PCMDIVM			( AIC_BASE_U_VIRTUAL + PCM_PCMDIV_OFFSET )

//--------------------------------------------------------------------------
// PCMCTL Common Define
//--------------------------------------------------------------------------
#define PCMCTL_ERDMA			( 1 << 9 )
#define PCMCTL_ETDMA			( 1 << 8 )
#define PCMCTL_LSMP				( 1 << 7 )
#define PCMCTL_ERPL				( 1 << 6 )
#define PCMCTL_EREC			( 1 << 5 )
#define PCMCTL_FLUSH		( 1 << 4 )
#define PCMCTL_RST			( 1 << 3 )
#define PCMCTL_CLKEN		( 1 << 1 )
#define PCMCTL_PCMEN		( 1 << 0 )

//--------------------------------------------------------------------------
// PCMCFG Common Define
//--------------------------------------------------------------------------
#define PCMCFG_SLOT_MASK		( 3 << 13 )
#define PCMCFG_SLOT(x)			( x << 13 )
#define PCMCFG_ISS				( 1 << 12 )
#define PCMCFG_OSS				( 1 << 11 )
#define PCMCFG_IMSBPOS				( 1 << 10 )
#define PCMCFG_RFTH_MASK			( 0xf << 5 )
#define PCMCFG_RFTH(x)				( x << 5 )
#define PCMCFG_TFTH_MASK			( 0xf << 1 )
#define PCMCFG_TFTH(x)			( x << 1 )
#define PCMCFG_PCMMOD			( 1 << 0 )

//--------------------------------------------------------------------------
// PCMDP Common Define
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// PCMINTC Common Define
//--------------------------------------------------------------------------
#define PCMINTC_ETFS			( 1 << 3 )
#define PCMINTC_ETUR			( 1 << 2 )
#define PCMINTC_ERFS			( 1 << 1 )
#define PCMINTC_EROR			( 1 << 0 )

//--------------------------------------------------------------------------
// PCMINTS Common Define
//--------------------------------------------------------------------------
#define PCMINTS_TFS			( 1 << 8 )
#define PCMINTS_TUR			( 1 << 7 )
#define PCMINTS_RFS			( 1 << 1 )
#define PCMINTS_ROR			( 1 << 0 )

//--------------------------------------------------------------------------
// PCMDIV Common Define
//--------------------------------------------------------------------------
#define PCMDIV_SYNL(x)			( x << 11 )
#define PCMDIV_SYNDIV(x)		( x << 6 )
#define PCMDIV_CLKDIV(x)		( x << 1 )

//--------------------------------------------------------------------------
// GPIO Common Define
//--------------------------------------------------------------------------
#define PCM_GPIO_PIN			(GPIO_18 | GPIO_19 | GPIO_20 | GPIO_21)

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int 	PCMCTL;		//00, R/W, 32
	unsigned int 	PCMCFG;		//04, R/W, 32
	unsigned int	PCMDP;		//08, R/W, 32
	unsigned int 	PCMINTC;	//0C, R/W, 32
	unsigned int 	PCMINTS;	//10, R/W, 32
	unsigned int	PCMDIV;		//14, R/W, 32	
}J4755_PCM, *PJ4755_PCM;
	

#endif //__MIPS_ASSEMBLER

#endif //__JZ4755AIC_H__
