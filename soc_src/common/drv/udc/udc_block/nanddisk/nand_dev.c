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

#if NAND_UDC_DISK

#include "header.h"
#include <mass_storage.h>
#include <os_api.h>
#include <jzfs_api.h>
#include "function.h"

static UDC_LUN udcLun;


static int InitStall = 0;
static int FlushDataState = 0;
static unsigned int init_dev(unsigned int handle)
{

	return 1;
}
static unsigned int read_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{
	
	
	return 1;
	
}
static unsigned int write_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{

	
}
static unsigned int check_dev_state(unsigned int handle)
{
	return 1;
}

static unsigned int flush_dev(unsigned int handle)
{

		
}
static unsigned int get_dev_info(unsigned int handle,PDEVINFO pinfo)
{

}
static unsigned int deinit_dev(unsigned handle)
{
	

		
}
void init_udc_nand()
{

//	printf("InitUdcRam\r\n");
	
	Init_Mass_Storage();
	udcLun.InitDev = init_dev;
	udcLun.ReadDevSector = read_dev_sector;
	udcLun.WriteDevSector = write_dev_sector;
	udcLun.CheckDevState = check_dev_state;
	udcLun.GetDevInfo = get_dev_info;
	udcLun.FlushDev = flush_dev;
	udcLun.DeinitDev = deinit_dev;
	//udcLun.DevName = (unsigned int)'NAND';
	udcLun.DevName = ('N' << 24) | ('A' << 16) | ('N' << 8) | 'D';

	if(RegisterDev(&udcLun))
		printf("UDC Register Fail!!!\r\n");
	//printf("InitUdcRam finish = %08x\r\n",&udcLun);
	
}
#endif
