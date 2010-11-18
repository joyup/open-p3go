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

#ifndef __MASS_STOTAGE_H__
#define __MASS_STOTAGE_H__
typedef struct{
	unsigned int hiddennum;
	unsigned int headnum;
	unsigned int sectnum;
	unsigned int partsize;
	unsigned int sectorsize;
} DEVINFO,*PDEVINFO;
typedef struct
{
	unsigned int (*InitDev)(unsigned int handle);
	unsigned int (*ReadDevSector)(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount);
	unsigned int (*WriteDevSector)(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount);
	unsigned int (*CheckDevState)(unsigned int handle);
	unsigned int (*FlushDev)(unsigned int handle);
	unsigned int (*GetDevInfo)(unsigned int handle,PDEVINFO pinfo);
	unsigned int (*DeinitDev)(unsigned handle);
	unsigned int sectorsize;
	unsigned int DevName;
}UDC_LUN,*PUDC_LUN;
#endif //__MASS_STOTAGE_H__
