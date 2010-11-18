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

#ifndef __STRUCTURE_H__
#define __STRUCTURE_H__

#include <nand_api.h>


typedef struct __BLOCK_CACHE__
{
	int	in_use;
	int	virt_block; 
	int	old_phys_block;
	int	new_phys_block;

	// unsigned char 8bit because max pagesize = 4(2048 / 512) bit
	unsigned char	*dirty_page;

	//unsigned short dirty_sector[CONFIG_SSFDC_NAND_PAGE_PER_BLOCK][SECTORS_PER_PAGE];
	unsigned char	*data;
	unsigned char	*ext_data;
} block_cache_t;

typedef struct __PAGE_CACHE__
{
	unsigned long	page_num;
	unsigned char	*data;
} page_cache_t;

typedef struct __BLOCK_INFO__
{
	unsigned int	lifetime;
	unsigned int	tag;
} block_info_t;




typedef struct __NAND_ZONE__
{
	unsigned int	dwSignature;
	unsigned int	dwStartBlock;
	unsigned int	dwTotalBlocks;
	unsigned int	dwTotalSectors;
	unsigned int	dwMaxBadBlocks;
} NAND_ZONE, *PNAND_ZONE;


typedef struct __VENUS_NAND_INFO__
{
	unsigned int	dwBytesPerBlock;
	unsigned int	dwSectorsPerBlock;
	unsigned int	dwDataBytesPerSector;
	unsigned int	dwTotalPhyBlocks;

	unsigned int	dwNumOfZone;
	NAND_ZONE		NandZoneList[1];
} VENUS_NAND_INFO, *PVENUS_NAND_INFO;


typedef struct __NAND_CHIP_INFO__
{
	int id;
	unsigned int chipcs;
	unsigned int start_pages;
	unsigned int end_pages;
	
}NandChipInfo,*PNandChipInfo;
#endif	// __STRUCTURE_H__
