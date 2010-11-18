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

#if SD_UDC_DISK
#include <jzfs_api.h>
#include "function.h"
#include <mass_storage.h>
#include "mmc_api.h"

static UDC_LUN udcLun;
static int isinited = 0;


unsigned int init_dev(unsigned int handle)
{

}

unsigned int read_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{

}

unsigned int write_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{

}

unsigned int check_dev_state(unsigned int handle)
{



}

unsigned int get_dev_info(unsigned int handle,PDEVINFO pinfo)
{
	pinfo->hiddennum = 0;
	pinfo->headnum = 2;
	pinfo->sectnum = 4;
	if (mmc_check_status())
		pinfo->partsize = MMC_GetSize();
	else
		pinfo->partsize = 1;
	pinfo->sectorsize = 512;
}

unsigned int deinit_dev(unsigned handle)
{
	
}

void  init_udc_mmc()
{

	

	udcLun.InitDev = init_dev;
	udcLun.ReadDevSector = read_dev_sector;
	udcLun.WriteDevSector = write_dev_sector;
	udcLun.CheckDevState = check_dev_state;
	udcLun.GetDevInfo = get_dev_info;
	udcLun.DeinitDev = deinit_dev;
	udcLun.FlushDev = 0;
	//udcLun.DevName = (unsigned int)'MMC1';
	udcLun.DevName = ('M' << 24) | ('M' << 16) | ('C' << 8) | '1';

	if(RegisterDev(&udcLun))
		printf("UDC Register Fail!!!\r\n");
	
}
#endif
