/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
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
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn> 
 *
 *  Create:   2008-06-26, by dsqiu
 *            
 *  Maintain: 2008-06-26, by jgao
 *            
 *
 *******************************************************************************
 */


#ifndef __I2C_H__
#define __I2C_H__

extern int i2c_read(unsigned char device, unsigned char *buf,
		    unsigned char offset, int count); 	/* Read register though I2C */
extern int i2c_write(unsigned char device, unsigned char *buf,
	      unsigned char offset, int count);		/* Write register though I2C */
extern int i2c_read16(unsigned char device, unsigned char *buf,
		    unsigned short offset, int count); 	/* Read register though I2C */
extern int i2c_write16(unsigned char device, unsigned char *buf,
	      unsigned short offset, int count);		/* Write register though I2C */
extern void i2c_open(void);				/* Open and enable I2C */
extern void i2c_close(void);				/* Disable and close I2C */
extern void i2c_setclk(unsigned int i2cclk); 		/* Set I2C clock */
#endif
