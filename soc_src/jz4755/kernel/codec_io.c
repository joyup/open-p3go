/********************** BEGIN LICENSE BLOCK **********************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright Ingenic Semiconductor Co. Ltd 2008. All rights reserved.
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
 *  Create:   2009-3-4, by whxu
 *
 *****************************************************************************
 */

#include <bsp.h>
#include <jz4755.h>

//----------------------------------------------------------------------------

unsigned char codec_reg_read(unsigned char addr)
{
	AIC_RW_CODEC_START();
	OUTREG16(A_AIC_RGADW, addr << RGADW_ADDR_BIT);

	return (INREG8(A_AIC_RGDAT));
}

//----------------------------------------------------------------------------

void codec_reg_write(unsigned char addr, unsigned char data)
{
	unsigned int	temp;

	temp = (addr << RGADW_ADDR_BIT) | (data << RGADW_DATA_BIT);

	AIC_RW_CODEC_START();
	OUTREG32(A_AIC_RGADW, temp);
	OUTREG32(A_AIC_RGADW, temp | AIC_RGADW_RGWR);

	AIC_RW_CODEC_STOP();
}

//----------------------------------------------------------------------------

void codec_reg_set(unsigned char addr, unsigned char data)
{
	unsigned int	temp = addr << RGADW_ADDR_BIT;

	AIC_RW_CODEC_START();
	OUTREG16(A_AIC_RGADW, temp);
	temp |= (INREG8(A_AIC_RGDAT) | data);
	OUTREG32(A_AIC_RGADW, temp);
	OUTREG32(A_AIC_RGADW, temp | AIC_RGADW_RGWR);

	AIC_RW_CODEC_STOP();
}

//----------------------------------------------------------------------------

void codec_reg_clear(unsigned char addr, unsigned char data)
{
	unsigned int	temp = addr << RGADW_ADDR_BIT;
	unsigned char t;

	AIC_RW_CODEC_START();
	OUTREG16(A_AIC_RGADW, temp);
	temp |= (INREG8(A_AIC_RGDAT) & (~data));
	OUTREG32(A_AIC_RGADW, temp);
	OUTREG32(A_AIC_RGADW, temp | AIC_RGADW_RGWR);

	AIC_RW_CODEC_STOP();
}

//----------------------------------------------------------------------------
