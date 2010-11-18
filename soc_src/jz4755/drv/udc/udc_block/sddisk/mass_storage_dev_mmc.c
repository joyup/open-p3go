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



#define UCFS_MMC_NAME "mmc:"
static UDC_LUN udcLun;
static int isinited = 0;
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

unsigned int init_dev(unsigned int handle)
{
//	printf("mmc init \n");
	//针对Win7不能弹出优盘所做的临时做法
	//jzfs_IoCtl("mmc:",JZFS_CMD_UNMOUNT,0,0);		//移动到sysboot中

}

unsigned int read_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{
	if (mmc_check_status())
		MMC_ReadMultiBlock( startsect , sectcount , buf);
}

unsigned int write_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{
	if (mmc_check_status())
		MMC_WriteMultiBlock( startsect , sectcount , buf);
}

unsigned int check_dev_state(unsigned int handle)
{
	return (mmc_check_status());
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
	jzfs_IoCtl("mmc:",JZFS_CMD_MOUNT,0,0);
	return 1;
}

void  init_udc_mmc()
{
	Init_Mass_Storage();

	udcLun.InitDev = init_dev;
	udcLun.ReadDevSector = read_dev_sector;
	udcLun.WriteDevSector = write_dev_sector;
	udcLun.CheckDevState = check_dev_state;
	udcLun.GetDevInfo = get_dev_info;
	udcLun.DeinitDev = deinit_dev;
	udcLun.FlushDev = 0;
	//udcLun.DevName = (unsigned int)'MMC1';
	udcLun.DevName = ('M' << 24) | ('M' << 16) | ('C' << 8) | '1';

#if (MMC_PRESENT == 0xff)
#else
	if(RegisterDev(&udcLun))
		printf("UDC Register Fail!!!\r\n");
#endif	
}
#endif
