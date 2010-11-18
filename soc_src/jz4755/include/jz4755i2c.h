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


#ifndef __JZ4755I2C_H__
#define __JZ4755I2C_H__

//--------------------------------------------------------------------------
// I2C Interface Module(I2C) Address Definition
//--------------------------------------------------------------------------
#define I2C_PHYS_ADDR       0x10042000
#define I2C_BASE_U_VIRTUAL  0xB0042000


//--------------------------------------------------------------------------
// I2C Registers Offset Definition
//--------------------------------------------------------------------------
#define I2CDR_OFFSET			0x00	// RW, 8 Bit
#define I2CCR_OFFSET			0x04	// RW, 8 Bit
#define I2CSR_OFFSET			0x08	// RW, 8 Bit
#define I2CGR_OFFSET			0x0C	// RW, 16 Bit


//--------------------------------------------------------------------------
// I2C Registers Address Definition
//--------------------------------------------------------------------------
#define A_I2C_I2CDR		( I2C_BASE_U_VIRTUAL + I2CDR_OFFSET )
#define A_I2C_I2CCR		( I2C_BASE_U_VIRTUAL + I2CCR_OFFSET )
#define A_I2C_I2CSR		( I2C_BASE_U_VIRTUAL + I2CSR_OFFSET )
#define A_I2C_I2CGR		( I2C_BASE_U_VIRTUAL + I2CGR_OFFSET )


//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------
// I2C Control Register(I2CCR) Common Define
#define	I2CCR_INT_EN			( 1 << 4 )
#define	I2CCR_START				( 1 << 3 )
#define	I2CCR_STOP				( 1 << 2 )
#define	I2CCR_ACK_HIGH			( 1 << 1 )
#define	I2CCR_ENABLE			( 1 << 0 )

// I2C Control Register(I2CSR) Common Define
#define	I2CSR_STX				( 1 << 4 )
#define	I2CSR_BUSY				( 1 << 3 )
#define	I2CSR_TEND				( 1 << 2 )
#define	I2CSR_DATA_VALID		( 1 << 1 )
#define	I2CSR_ACK_HIGH			( 1 << 0 )

// I2C Control Register(I2CGR) Common Define

// I2C CONST
#define	I2C_SCLK				( 100000 )
#define	I2C_RETRY_TIMES			0x3000
#define	READ					TRUE
#define	WRITE					FALSE

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned char	I2CDR;
	unsigned char	RESERVED0[3];
	unsigned char	I2CCR;
	unsigned char	RESERVED1[3];
	unsigned char	I2CSR;
	unsigned char	RESERVED2[3];
	unsigned short	I2CGR;
	unsigned short	RESERVED3[1];
} JZ4755_I2C, *PJZ4755_I2C;

/* I2C_SDA, I2C_SCK */
#define __gpio_as_i2c()									\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(4), 0x00003000);				\
	OUTREG32(A_GPIO_PXSELC(4), 0x00003000);				\
} while (0)

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755I2C_H__
