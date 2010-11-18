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


#include "udcbus.h"
#include "function.h"
//mem manger using DrvMngMem.c
#define malloc    alloc
#define free      deAlloc

PUDC_BUS curDevice = 0;
PUDC_BUS CreateDevice(PFN_BusNotify busNotify)
{
	if(curDevice == 0)
	{
		curDevice = (PUDC_BUS)malloc(sizeof(UDC_BUS));
		InitUDC(curDevice);
		//curDevice->DeviceName = (unsigned int)'UDC:';
		curDevice->DeviceName = ('U' << 24) | ('D' << 16) | ('C' << 8) | ':';
	}
	curDevice->BusNotify = busNotify;
	
	return curDevice;
}
void CloseDevice()
{
	if(curDevice)
		free(curDevice);
	curDevice = 0;
}
PUDC_BUS SwichDevice(PFN_BusNotify busNotify)
{
	if(curDevice)
		curDevice->BusNotify = busNotify;
	return curDevice;
	
}
unsigned int BusNotify(unsigned int handle,unsigned int stat,unsigned char *bufaddr,unsigned int len)
{
	if(handle != (unsigned int)curDevice)
		return 0;
    curDevice->BusNotify(handle,stat,bufaddr,len);
	return 1;
	
}
