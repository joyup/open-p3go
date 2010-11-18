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
 *  Maintained:   2009-01-06, by jgao
 *
 *******************************************************************************
 */

#ifndef __JZ4755SSI_H__
#define __JZ4755SSI_H__

//--------------------------------------------------------------------------
// SSI Controller(SSI) Address Definition
//--------------------------------------------------------------------------
#define SSI0_PHYS_ADDR			( 0x10043000 )
#define SSI0_BASE_U_VIRTUAL		( 0xB0043000 )

#define SSI1_PHYS_ADDR			( 0x10045000 )
#define SSI1_BASE_U_VIRTUAL		( 0xB0045000 )

//--------------------------------------------------------------------------
// SSI Registers Offset Definition
//--------------------------------------------------------------------------
#define SSI_DR_OFFSET		(0x00)	// RW, 32
#define SSI_CR0_OFFSET		(0x04)	// RW, 16
#define SSI_CR1_OFFSET		(0x08)	// RW, 32
#define SSI_SR_OFFSET		(0x0C)	// RW, 32
#define SSI_ITR_OFFSET		(0x10)	// RW, 16
#define SSI_ICR_OFFSET		(0x14)	// RW, 8
#define SSI_GR_OFFSET		(0x18)	// RW, 16


//--------------------------------------------------------------------------
// SSI Registers Address Definition
//--------------------------------------------------------------------------
#define A_SSI0_DR				(SSI0_BASE_U_VIRTUAL + SSI_DR_OFFSET)
#define A_SSI0_CR0				(SSI0_BASE_U_VIRTUAL + SSI_CR0_OFFSET)
#define A_SSI0_CR1				(SSI0_BASE_U_VIRTUAL + SSI_CR1_OFFSET)
#define A_SSI0_SR				(SSI0_BASE_U_VIRTUAL + SSI_SR_OFFSET)
#define A_SSI0_ITR				(SSI0_BASE_U_VIRTUAL + SSI_ITR_OFFSET)
#define A_SSI0_ICR				(SSI0_BASE_U_VIRTUAL + SSI_ICR_OFFSET)
#define A_SSI0_GR				(SSI0_BASE_U_VIRTUAL + SSI_GR_OFFSET)

#define A_SSI1_DR				(SSI1_BASE_U_VIRTUAL + SSI_DR_OFFSET)
#define A_SSI1_CR0				(SSI1_BASE_U_VIRTUAL + SSI_CR0_OFFSET)
#define A_SSI1_CR1				(SSI1_BASE_U_VIRTUAL + SSI_CR1_OFFSET)
#define A_SSI1_SR				(SSI1_BASE_U_VIRTUAL + SSI_SR_OFFSET)
#define A_SSI1_ITR				(SSI1_BASE_U_VIRTUAL + SSI_ITR_OFFSET)
#define A_SSI1_ICR				(SSI1_BASE_U_VIRTUAL + SSI_ICR_OFFSET)
#define A_SSI1_GR				(SSI1_BASE_U_VIRTUAL + SSI_GR_OFFSET)


//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------

// SSI Control Register0 (SSICR0)
#define SSI_SSIE		(1 << 15)
#define SSI_TIE			(1 << 14)
#define SSI_RIE			(1 << 13)
#define SSI_TEIE		(1 << 12)
#define SSI_REIE		(1 << 11)
#define SSI_LOOP		(1 << 10)
#define SSI_RFINE		(1 << 9)
#define SSI_RFINC		(1 << 8)
#define SSI_EACLRUN		(1 << 7)
#define SSI_FSEL		(1 << 6)
#define SSI_TFMODE		(1 << 3)
#define SSI_TFLUSH		(1 << 2)
#define SSI_RFLUSH		(1 << 1)
#define SSI_DISREV		(1 << 0)

// SSI Control Register1 (SSICR1)
#define SSI_TFVCK_MASK	(3 << 28)
#define SSI_TFVCK_3		(3 << 28)
#define SSI_TFVCK_2		(2 << 28)
#define SSI_TFVCK_1		(1 << 28)
#define SSI_TFVCK_0		(0 << 28)

#define SSI_TCKFI_MASK	(3 << 26)
#define SSI_TCKFI_3		(3 << 26)
#define SSI_TCKFI_2		(2 << 26)
#define SSI_TCKFI_1		(1 << 26)
#define SSI_TCKFI_0		(0 << 26)

#define SSI_LFST		(1 << 25)
#define SSI_ITFRM		(1 << 24)
#define SSI_UNFIN		(1 << 23)

#define SSI_FMAT_MASK	(3 << 20)
#define SSI_FMAT_MW2	(3 << 20)
#define SSI_FMAT_MW1	(2 << 20)
#define SSI_FMAT_SSP	(1 << 20)
#define SSI_FMAT_SPI	(0 << 20)

#define SSI_MCOM_MASK	(0x1f << 12)
#define SSI_MCOM_16		(15 << 12)
#define SSI_MCOM_15		(14 << 12)
#define SSI_MCOM_14		(13 << 12)
#define SSI_MCOM_13		(12 << 12)
#define SSI_MCOM_12		(11 << 12)
#define SSI_MCOM_11		(10 << 12)
#define SSI_MCOM_10		(9 << 12)
#define SSI_MCOM_9		(8 << 12)
#define SSI_MCOM_8		(7 << 12)
#define SSI_MCOM_7		(6 << 12)
#define SSI_MCOM_6		(5 << 12)
#define SSI_MCOM_5		(4 << 12)
#define SSI_MCOM_4		(3 << 12)
#define SSI_MCOM_3		(2 << 12)
#define SSI_MCOM_2		(1 << 12)
#define SSI_MCOM_1		(0 << 12)

#define SSI_CR1_TTRG_BIT	16
#define SSI_CR1_TTRG_MASK	(0xf << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_1	  (0 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_8	  (1 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_16	  (2 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_24	  (3 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_32	  (4 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_40	  (5 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_48	  (6 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_56	  (7 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_64	  (8 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_72	  (9 << SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_80	  (10<< SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_88	  (11<< SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_96	  (12<< SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_104	  (13<< SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_112	  (14<< SSI_CR1_TTRG_BIT)
#define SSI_CR1_TTRG_120	  (15<< SSI_CR1_TTRG_BIT)

#define SSI_CR1_RTRG_BIT	8
#define SSI_CR1_RTRG_MASK	(0xf << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_1	  (0 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_8	  (1 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_16	  (2 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_24	  (3 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_32	  (4 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_40	  (5 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_48	  (6 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_56	  (7 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_64	  (8 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_72	  (9 << SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_80	  (10<< SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_88	  (11<< SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_96	  (12<< SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_104	  (13<< SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_112	  (14<< SSI_CR1_RTRG_BIT)
#define SSI_CR1_RTRG_120	  (15<< SSI_CR1_RTRG_BIT)

#define SSI_RTRG_MASK	(0xf << 8)
#define SSI0_RTRG_14	(0x3 << 8)
#define SSI0_RTRG_8		(0x2 << 8)
#define SSI0_RTRG_4		(0x1 << 8)
#define SSI0_RTRG_1		(0x0 << 8)
#define SSI1_RTRG(n)	((n/8) << 8)

#define SSI_FLEN_MASK	(0x1f << 4)
#define SSI_FLEN_17 	(15 << 4) 
#define SSI_FLEN_16		(14 << 4) 
#define SSI_FLEN_15		(13 << 4) 
#define SSI_FLEN_14		(12 << 4) 
#define SSI_FLEN_13		(11 << 4) 
#define SSI_FLEN_12		(10 << 4) 
#define SSI_FLEN_11		( 9 << 4)  
#define SSI_FLEN_10		( 8 << 4)  
#define SSI_FLEN_9		( 7 << 4)  
#define SSI_FLEN_8		( 6 << 4)  
#define SSI_FLEN_7		( 5 << 4)  
#define SSI_FLEN_6		( 4 << 4)  
#define SSI_FLEN_5		( 3 << 4)  
#define SSI_FLEN_4		( 2 << 4)  
#define SSI_FLEN_3		( 1 << 4)  
#define SSI_FLEN_2		( 0 << 4)  
	
#define SSI_PHA			(1 << 1)
#define SSI_POL			(1 << 0)

// SSI Status Register1 (SSISR)
#define SSI_TFIFONUM_BIT	16
#define SSI_RFIFONUM_BIT	8
#define SSI_TFIFONUM_MASK	(0xff << 16)
#define SSI_RFIFONUM_MASK	(0xff << 8)
#define SSI_END				(1 << 7)
#define SSI_BUSY			(1 << 6)
#define SSI_TFF				(1 << 5)
#define SSI_RFE				(1 << 4)
#define SSI_TFHE			(1 << 3)
#define SSI_RFHF			(1 << 2)
#define SSI_UNDR			(1 << 1)
#define SSI_OVER			(1 << 0)

// SSI Interval Time Control Register (SSIITR)
#define SSI_SSSIITR_CNTCLK		(1 << 15)

#define SSI_TYPE_MOTOROLA     	0
#define SSI_TYPE_TI           	1
#define SSI_TYPE_NATIONAL1    	2
#define SSI_TYPE_NATIONAL2    	3

// @GPIO B
#define	SSI0_GPIO_CONFIG	(GPIO_26 | GPIO_27 | GPIO_28 | GPIO_29 | GPIO_31)
#define SSI0_GPIO_CE0_CFG	(GPIO_26 | GPIO_27 | GPIO_28 | GPIO_29)
#define SSI0_GPIO_CE1_CFG	(GPIO_26 | GPIO_27 | GPIO_28 | GPIO_31)
#define SSI0_GPIO_GPC		(GPIO_30)

// @GPIO D
#define	SSI1_GPIO_CONFIG	(GPIO_26 | GPIO_27 | GPIO_28 | GPIO_29 | GPIO_30)
#define SSI1_GPIO_CE0_CFG	(GPIO_26 | GPIO_27 | GPIO_28 | GPIO_29)
#define SSI1_GPIO_CE1_CFG	(GPIO_26 | GPIO_27 | GPIO_28 | GPIO_30)


#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int		SSIDR;		// 0x00
	unsigned short		SSICR0;		// 0x04
	unsigned short   	SSIRSV0[1];
	unsigned int		SSICR1;		// 0x08
	unsigned int		SSISR;		// 0x0C
	unsigned short		SSIITR;		// 0x10
	unsigned short		SSIRSV1[1];
	unsigned char		SSIICR;		// 0x14
	unsigned char		SSIRSV2[3];
	unsigned short		SSIGR;		// 0x18
	unsigned short		SSIRSV3[1];

}JZ4755_SSI, *PJZ4755_SSI;

//---------------------------------------------------------------------------------------------------
//SSI Data Register (SSIDR)
//---------------------------------------------------------------------------------------------------
typedef volatile union _SSIDR
{
	struct
	{
		unsigned int  RTDATA:16;
		unsigned int  GPC_D16:1;
		unsigned int  RESERVE0:15;
	} bit;
	unsigned int value;
} SSIDR, *PSSIDR;

//-----------------------------------------------------------------------------------------------------
//SSI Control Register0 (SSICR0)
//---------------------------------------------------------------------------------------------------------
typedef union _SSICR0
{
	struct
	{
		unsigned short   DISREV:1;
		unsigned short   RFLUSH:1;
		unsigned short   TFLUSH:1;
		unsigned short   TFMODE:1;
		unsigned short	 RESERVE0:2;
		unsigned short   FSEL:1;
		unsigned short   EACLRUN:1;
		unsigned short   RFINC:1;
		unsigned short   RFINE:1;
		unsigned short   LOOP:1;
		unsigned short   REIE:1;
		unsigned short   TEIE:1;
		unsigned short   RIE:1;
		unsigned short   TIE:1;
		unsigned short   SSIE:1;
	} bit;
	unsigned short value;
} SSICR0, *PSSICR0;

typedef union _SSICR1
{
	struct
	{
		unsigned int   POL:1;
		unsigned int   PHA:1;
		unsigned int   RESERVE0:2;
		unsigned int   FLEN:4;
		unsigned int   RTRG:4;
		unsigned int   MCOM:4;
		unsigned int   TTRG:4;
		unsigned int   FMAT:2;
		unsigned int   RESERVE1:1;
		unsigned int   UNFIN:1;
		unsigned int   ITFRM:1;
		unsigned int   LFST:1;
		unsigned int   TCKFI:2;
		unsigned int   TFVCK:2;
		unsigned int   FRMHL:2;
	} bit;
	unsigned int value;
} SSICR1, *PSSICR1;

typedef union _SSISR
{
	struct
	{
		unsigned int   OVER:1;
		unsigned int   UNDR:1;
		unsigned int   RFHF:1;
		unsigned int   TFHE:1;
		unsigned int   RFE:1;
		unsigned int   TFF:1;
		unsigned int   BUSY:1;
		unsigned int   END:1;
		unsigned int   RFIFO_NUM:8;
		unsigned int   TFIFO_NUM:8;
		unsigned int   RESERVE0:8;
	} bit;
	unsigned int value;
} SSISR, *PSSISR;

typedef union _SSIITR
{
	struct
	{
		unsigned short   IVLTM:15;
		unsigned short   CNTCLK:1;
	} bit;
	unsigned short value;
} SSIITR, *PSSIITR;

//--------------------------------------------------------------------------------------------------------
//SSI Interval Character-per-frame Control Register (SSIICR)
//--------------------------------------------------------------------------------------------------
typedef volatile union _SSIICR
{
	struct
	{
		unsigned char ICC:3;
		unsigned char RESERVE0:5;
	}bit;
	unsigned char value;
} SSIICR, *PSSIICR;

//---------------------------------------------------------------------------------------------------------------------------
//SSI Clock Generator Register (SSIGR)
//---------------------------------------------------------------------------------------------------------
typedef volatile union _SSIGR
{
	struct
	{
		unsigned short CGV:8;
		unsigned short RESERVE0:8;
	}bit;
	unsigned short value;
} SSIGR, *PSSIGR;


#define _ssi_reset()           \
do{				     \
	OUTREG16(A_SSI0_CR0, 0x0000);        \
	OUTREG32(A_SSI0_CR1, 0x00007960);    \
	OUTREG32(A_SSI0_SR, 0x00000098);    \
	OUTREG16(A_SSI0_ITR, 0x0000);        \
	OUTREG8(A_SSI0_ICR, 0x00);          \
	OUTREG16(A_SSI0_GR, 0x0000);        \
    SETREG16(A_SSI0_CR0,SSI_TFLUSH);	\
    SETREG16(A_SSI0_CR0,SSI_RFLUSH);	\
}while(0)
/* SSI0_CLK, SSI0_DT, SSI0_DR SSI0_CE0_ SSI0_CE2_  SSI0_CE1_GPC*/
#define GPIO_AS_SSI0_CE0		(1 << 29)
#define GPIO_AS_SSI0_GPC		(1 << 30)
#define GPIO_AS_SSI0_CE2		(1 << 31)
#define GPIO_AS_SSI0_MAIN		(7 << 26)
#define GPIO_SSI0				(GPIO_AS_SSI0_CE0	|	\
								 GPIO_AS_SSI0_GPC	|	\
								 GPIO_AS_SSI0_CE2	|	\
								 GPIO_AS_SSI0_MAIN		\
								 )

#define __gpio_as_ssi0(GPIO_SSI0)								\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(1), GPIO_SSI0);			\
	OUTREG32(A_GPIO_PXSELC(1), GPIO_SSI0);			\
	OUTREG32(A_GPIO_PXPES(1), GPIO_SSI0);			\
} while (0)

/* SSI1_CLK, SSI1_DT, SSI1_DR SSI1_CE0_ SSI0_CE1_*/
#define GPIO_AS_SSI1_CE0		(1 << 29)
#define GPIO_AS_SSI1_CE1		(1 << 30)
#define GPIO_AS_SSI1_MAIN		(7 << 26)
#define GPIO_SSI1				(GPIO_AS_SSI1_CE0	|	\
								 GPIO_AS_SSI1_CE1	|	\
								 GPIO_AS_SSI1_MAIN		\
								 )

#define __gpio_as_ssi1(GPIO_SSI1)								\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(3), GPIO_SSI1);			\
	OUTREG32(A_GPIO_PXSELC(3), GPIO_SSI1);			\
	OUTREG32(A_GPIO_PXPES(3),  GPIO_SSI1);			\
} while (0)

/***************************************************************************
 * SSI
 ***************************************************************************/

#define __ssi_enable()     SETREG16(A_SSI0_CR0, SSI_SSIE )
#define __ssi_disable()    CLRREG16(A_SSI0_CR0, SSI_SSIE )
#define __ssi_select_ce()  CLRREG16(A_SSI0_CR0, SSI_FSEL )

#define __ssi_disable_tx_intr() \
	CLRREG16(A_SSI0_CR0,(SSI_TIE | SSI_TEIE))

#define __ssi_disable_rx_intr() \
	CLRREG16(A_SSI0_CR0,(SSI_RIE | SSI_REIE))
#define __ssi_flush_fifo() \
	SETREG16(A_SSI0_CR0,(SSI_TFLUSH | SSI_RFLUSH))

#define __ssi_enable_receive()   CLRREG16(A_SSI0_CR0,SSI_DISREV)

#define __ssi_clear_errors() \
    CLRREG32(A_SSI0_SR,(SSI_UNDR | SSI_OVER) )

/* n = ix8 */
#define __ssi_set_tx_trigger(n) 		\
do { 						\
	CLRREG32(A_SSI0_CR1,SSI_CR1_TTRG_MASK); 	\
	SETREG32(A_SSI0_CR1,SSI_CR1_TTRG_##n);		\
} while (0)

/* n = ix8 */
#define __ssi_set_rx_trigger(n) 		\
do { 						\
	CLRREG32(A_SSI0_CR1,SSI_CR1_RTRG_MASK);		\
	SETREG32(A_SSI0_CR1,SSI_CR1_RTRG_##n);		\
} while (0)

/* Set the clock phase for SPI */
#define __ssi_set_spi_clock_phase(n) \
    OUTREG32(A_SSI0_CR1, ((INREG32(A_SSI0_CR1) & ~SSI_PHA) | (n&0x1)) )

/* Set the clock polarity for SPI */
#define __ssi_set_spi_clock_polarity(n) \
	OUTREG32(A_SSI0_CR1, ((INREG32(A_SSI0_CR1) & ~SSI_POL) | (n&0x1)) )

#define __ssi_get_txfifo_count() \
    ( (INREG32(A_SSI0_SR) & SSI_TFIFONUM_MASK) >> SSI_TFIFONUM_BIT )

#define __ssi_get_rxfifo_count() \
    ( (INREG32(A_SSI0_SR) & SSI_RFIFONUM_MASK) >> SSI_RFIFONUM_BIT )

/* Transfer with MSB or LSB first */
#define __ssi_set_msb() CLRREG32(A_SSI0_CR1,SSI_LFST)
#define __ssi_set_lsb() SETREG32(A_SSI0_CR1,SSI_LFST)

#define __ssi_set_frame_length(n) \
    OUTREG32(A_SSI0_CR1, (INREG32(A_SSI0_CR1) & ~SSI_FLEN_MASK) | (((n) - 2) << 4))

#define __ssi_enable_loopback()  SETREG16(A_SSI0_CR0,SSI_LOOP)
#define __ssi_disable_loopback() CLRREG16(A_SSI0_CR0,SSI_LOOP)

#define __ssi_spi_format() 					\
do { 								\
	CLRREG32(A_SSI0_CR1,SSI_FMAT_MASK);			\
	SETREG32(A_SSI0_CR1,SSI_FMAT_SPI);							\
	CLRREG32(A_SSI0_CR1,(SSI_TFVCK_MASK|SSI_TCKFI_MASK));	\
	SETREG32(A_SSI0_CR1,(SSI_TFVCK_1 | SSI_TCKFI_1));		\
} while (0)




#if 0
#define __ssi_enable_tx_intr() 	\
  ( REG_SSI_CR0 |= SSI_CR0_TIE | SSI_CR0_TEIE )


#define __ssi_enable_rx_intr() 	\
  ( REG_SSI_CR0 |= SSI_CR0_RIE | SSI_CR0_REIE )



#define __ssi_disable_receive()  ( REG_SSI_CR0 |= SSI_CR0_DISREV )

#define __ssi_finish_receive() 	\
  ( REG_SSI_CR0 |= (SSI_CR0_RFINE | SSI_CR0_RFINC) )

#define __ssi_disable_recvfinish() \
  ( REG_SSI_CR0 &= ~(SSI_CR0_RFINE | SSI_CR0_RFINC) )

#define __ssi_flush_txfifo()   ( REG_SSI_CR0 |= SSI_CR0_TFLUSH )
#define __ssi_flush_rxfifo()   ( REG_SSI_CR0 |= SSI_CR0_RFLUSH )



#define __ssi_finish_transmit() ( REG_SSI_CR1 &= ~SSI_CR1_UNFIN )




/* CE# level (FRMHL), CE# in interval time (ITFRM),
   clock phase and polarity (PHA POL),
   interval time (SSIITR), interval characters/frame (SSIICR) */

 /* frmhl,endian,mcom,flen,pha,pol MASK */
#define SSICR1_MISC_MASK 					\
	( SSI_CR1_FRMHL_MASK | SSI_CR1_LFST | SSI_CR1_MCOM_MASK	\
	| SSI_CR1_FLEN_MASK | SSI_CR1_PHA | SSI_CR1_POL )	\

#define __ssi_spi_set_misc(frmhl,endian,flen,mcom,pha,pol)	\
do { 								\
	REG_SSI_CR1 &= ~SSICR1_MISC_MASK; 			\
	REG_SSI_CR1 |= ((frmhl) << 30) | ((endian) << 25) | 	\
		 (((mcom) - 1) << 12) | (((flen) - 2) << 4) | 	\
	         ((pha) << 1) | (pol); 				\
} while(0)



/* n = 1 - 16 */
#define __ssi_set_microwire_command_length(n) \
    ( REG_SSI_CR1 = ((REG_SSI_CR1 & ~SSI_CR1_MCOM_MASK) | SSI_CR1_MCOM_##n##BIT) )




#define __ssi_transfer_end()	( REG_SSI_SR & SSI_SR_END )
#define __ssi_is_busy()		( REG_SSI_SR & SSI_SR_BUSY )

#define __ssi_txfifo_full()	( REG_SSI_SR & SSI_SR_TFF )
#define __ssi_rxfifo_empty()	( REG_SSI_SR & SSI_SR_RFE )
#define __ssi_rxfifo_noempty()	( REG_SSI_SR & SSI_SR_RFHF )

#define __ssi_set_clk(dev_clk, ssi_clk) \
  ( REG_SSI_GR = (dev_clk) / (2*(ssi_clk)) - 1 )

#define __ssi_receive_data()    REG_SSI_DR
#define __ssi_transmit_data(v)  ( REG_SSI_DR = (v) )

#endif //if 0
/*
#define __gpio_as_ssi0()				\
do {						\
	A_GPIO_PXFUNS(1) = 0x3c000000;	\
	A_GPIO_PXSELC(1) = 0x3c000000;	\
	A_GPIO_PXPES(1) = 0x3c000000;		\
} while (0)
*/
#endif //__MIPS_ASSEMBLER

#endif // __JZ4755SSI_H__
