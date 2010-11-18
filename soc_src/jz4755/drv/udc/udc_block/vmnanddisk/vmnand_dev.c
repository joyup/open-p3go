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
 *  Author:  <xyzhang@ingenic.cn>  <jgao@ingenic.cn> 
 *
 *  Create:   2009-06-17, by xyzhang
 *
 *
 *******************************************************************************
 */

#if VMNAND_UDC_DISK

#include <mass_storage.h>
#include <os_api.h>
#include <jzfs_api.h>
#include <nand_api.h>
#include "function.h"

#undef printf

#define USEMBR	0

#if USEMBR
	#define HIDESECTOR	1
#else
	#define HIDESECTOR	0
#endif

static UDC_LUN udcLun;
extern int NAND_LB_Read(unsigned int Sector, void *pBuffer);
extern int NAND_LB_Write(unsigned int Sector, void *pBuffer);
//#undef printf

#define NAND_Write(x,y)	NAND_LB_Write(x,y)
#define NAND_Read(x,y)	NAND_LB_Read(x,y)

#define NAND_MultiWrite(x,y,z)	NAND_LB_MultiWrite(x,y,z)
#define NAND_MultiRead(x,y,z)	NAND_LB_MultiRead(x,y,z)

static int InitStall = 0;
static int FlushDataState = 0;
static int SectorSize;
static int totalsector = 0;
static int SectorCountStep = 2;

static unsigned int init_dev(unsigned int handle)
{
	if(InitStall == 0)
	{		
		printf("nand Disk unMount\r\n");
		jzfs_IoCtl("nfl:",JZFS_CMD_UNMOUNT,0,0);
		NAND_LB_Init();
		InitStall = 1;
		FlushDataState = 0;
		jzfs_IoCtl("nfl:",JZFS_CMD_CHECKMBR,0,&totalsector);
	}
}

static unsigned int read_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{
	int ret;
	unsigned int i;
	
	if(sectcount <= SectorCountStep)
	{	
		for(i = 0;i < sectcount; i++)
		{
			ret = NAND_Read(startsect + i,(void *)(buf));
			if (ret < 0)
				return ret;
			buf += SectorSize;
		}
	
	}else
	{
		ret = NAND_MultiRead(startsect,(void *)((unsigned int)buf),sectcount);
	}
	
	return ret;
	//return (NAND_MultiRead(startsect,(void *)((unsigned int)buf),sectcount));
}

static unsigned int write_dev_sector(unsigned int handle,unsigned char *buf,unsigned int startsect,unsigned int sectcount)
{
	int ret;
	unsigned int i;
	if(sectcount <= SectorCountStep)
	{
		for(i = 0;i<sectcount;i++)		
		{
			ret = NAND_Write(startsect + i,(void *)(buf));
			if (ret < 0)
				return ret;
			buf += SectorSize;
			FlushDataState = 1;
		}
	}
	else
	{
		//FlushDataState = 1;
		ret = NAND_MultiWrite(startsect,(void *)((unsigned int)buf),sectcount);
	}
	
	return ret;
	//return  (NAND_MultiWrite(startsect,(void *)((unsigned int)buf),sectcount));
}

static unsigned int check_dev_state(unsigned int handle)
{
	return 1;
}

static unsigned int flush_dev(unsigned int handle)
{
	NAND_LB_FLASHCACHE();	
}

static unsigned int get_dev_info(unsigned int handle,PDEVINFO pinfo)
{
	flash_info_t flashinfo;
	ssfdc_nftl_getInfo(0,&flashinfo);
	pinfo->hiddennum = 0;
	pinfo->headnum = 0;
	pinfo->sectnum = 1; // 4,8,16
	if (!totalsector)
		pinfo->partsize = flashinfo.dwFSTotalSectors - HIDESECTOR;
	else
		pinfo->partsize = totalsector;
//	printf("use mbr:%d\n",pinfo->partsize);
	pinfo->sectorsize = flashinfo.dwDataBytesPerSector;
	SectorSize = pinfo->sectorsize;
	SectorCountStep = (4 * 1024) / SectorSize;
}

static unsigned int deinit_dev(unsigned handle)
{
	if(InitStall == 1)
	{
		printf("udc mount nand fs!\n");
		NAND_LB_Deinit();
		jzfs_IoCtl("nfl:",JZFS_CMD_MOUNT,0,0);
		InitStall = 0;
	}		
}

void init_udc_nand()
{

//	printf("InitUdcNand\r\n");
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
