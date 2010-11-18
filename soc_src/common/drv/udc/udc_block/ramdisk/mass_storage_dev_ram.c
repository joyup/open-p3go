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
 

#if RAM_UDC_DISK
#include <mass_storage.h>
#include "function.h"
#define BLOCKNUM	16384*7
#define SECTSIZE	512
static UDC_LUN udcLun;
static unsigned int disk_emu[BLOCKNUM * SECTSIZE / 4];
static unsigned char *disk = 0;
static unsigned int init_dev(unsigned int handle)
{
	printf("Ram Disk Mount\r\n");
	disk = (unsigned char *)((unsigned int)disk_emu | 0xa0000000);
}
static unsigned int read_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{
//	printf("read dev = ia:%x,oa:%x s:%d c:%d\r\n",buf,disk,startsect,sectcount);
	
	memcpy(buf,(void *)(disk + startsect * SECTSIZE),sectcount*SECTSIZE);
	return 1;
	
}
static unsigned int write_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{
	memcpy((void *)(disk + startsect * SECTSIZE),buf,sectcount*SECTSIZE);
	return 1;
	
}
static unsigned int check_dev_state(unsigned int handle)
{
	return 1;
}
static unsigned int get_dev_info(unsigned int handle,PDEVINFO pinfo)
{
	pinfo->hiddennum = 0;
	pinfo->headnum = 2;
	pinfo->sectnum = 4;
	pinfo->partsize = BLOCKNUM;
	pinfo->sectorsize = 512;
}
static unsigned int deinit_dev(unsigned handle)
{
	disk = 0;
}
void init_udc_ram()
{

//	printf("init_udc_ram\r\n");
	
	Init_Mass_Storage();
	udcLun.InitDev = init_dev;
	udcLun.ReadDevSector = read_dev_sector;
	udcLun.WriteDevSector = write_dev_sector;
	udcLun.CheckDevState = check_dev_state;
	udcLun.GetDevInfo = get_dev_info;
	udcLun.DeinitDev = deinit_dev;
	udcLun.DevName = (unsigned int)'RAM1';

	if(RegisterDev(&udcLun))
		printf("UDC Register Fail!!!\r\n");
	//printf("init_udc_ram finish = %08x\r\n",&udcLun);
	
}
#endif
