/********************** BEGIN LICENSE BLOCK **********************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright@ Ingenic Semiconductor Co. Ltd 2008. All rights reserved.
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
 *	Create: 2008-11-18, by whxu
 *
 *****************************************************************************
 */

#include <bsp.h>
#include <jz4755.h>

#define	L3_EN		( GPIO_GROUP_F + 13 )		// GP-F13
#define	L3_CLK		( GPIO_GROUP_F + 10 )		// GP-F10
#define	L3_DATA		( GPIO_GROUP_F + 11 )		// GP-F11


#define	L3_EN_HIGH		OUTREG32(A_GPIO_PXDATS(L3_EN/32), 1 << (L3_EN%32))
#define	L3_EN_LOW		OUTREG32(A_GPIO_PXDATC(L3_EN/32), 1 << (L3_EN%32))

#define	L3_CLK_HIGH		OUTREG32(A_GPIO_PXDATS(L3_CLK/32), 1 << (L3_CLK%32))
#define	L3_CLK_LOW		OUTREG32(A_GPIO_PXDATC(L3_CLK/32), 1 << (L3_CLK%32))

#define	L3_DATA_HIGH	OUTREG32(A_GPIO_PXDATS(L3_DATA/32), 1 << (L3_DATA%32))
#define	L3_DATA_LOW		OUTREG32(A_GPIO_PXDATC(L3_DATA/32), 1 << (L3_DATA%32))

//----------------------------------------------------------------------------

void l3_gpio_init(void)
{
	gpio_set_dir(L3_EN, 1, 1);
	gpio_set_dir(L3_CLK, 1, 1);
	gpio_set_dir(L3_DATA, 1, 1);
}

//----------------------------------------------------------------------------
#define	DELAT_TIME	50

unsigned char l3_write_reg(unsigned char reg, unsigned char data)
{
	unsigned short i;
	unsigned short value = (reg << 10) | data;

	L3_EN_HIGH;
	L3_CLK_LOW;
	L3_DATA_LOW;

	L3_EN_LOW;
	udelay(DELAT_TIME);
	for (i = 0; i < 16; i++)
	{
		L3_CLK_LOW;
		if ((value & 0x8000) == 0x8000)
			L3_DATA_HIGH;
		else
			L3_DATA_LOW;
		udelay(DELAT_TIME);

		L3_CLK_HIGH;
		value <<= 1;
		udelay(DELAT_TIME);
	}
	L3_EN_HIGH;

	udelay(DELAT_TIME);

	return (0);
}

//----------------------------------------------------------------------------
