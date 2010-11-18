/*
 *  XBurst  mobile_tv  Project  V1.0.0
 *  Copyright (C) 2006 - 2007 Ingenic Semiconductor Inc.
 *  Author: <xliu@ingenic.cn>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  2007-12-27, xliu
 *  Created.
 */

#include <bsp.h>
#include <os_api.h>
#include "nand_api.h"
#include "function.h"

// 配置信息在Nand中的位置
#define CONFIG_ADDR     256 // 配置信息
#define CONFIG_LENGTH   1
#define BLOCK_MAX					(1024 * 1)
#define	IMG_AND_DEF_BIN_PAGE_SIZE	(2048)

unsigned int	g_page_size = IMG_AND_DEF_BIN_PAGE_SIZE;
unsigned int	g_page_per_block = 128;

// 配置信息：位置(页号) 长度(页)
static int config_buf[IMG_AND_DEF_BIN_PAGE_SIZE / 4 * 4] __attribute__ ((aligned (32)));   // 配置信息
static int *config = (int *)config_buf;
// 应用程序入口函数
static void (*minios_entry)();

unsigned int load_data(	unsigned int	page_addr,
						unsigned int	page_count,
						unsigned char	*buffer)
{
	unsigned int dev_id = 0, ret, i;
	unsigned int cur_page, cur_block, first_page, last_page;
	unsigned int read_page_count = 0;
	nand_page_info_t	info, info2;

	cur_page = page_addr;
	cur_block = cur_page / g_page_per_block;
	for (i = 0; i < BLOCK_MAX; i++) {
		first_page = cur_block * g_page_per_block;
		last_page = first_page + g_page_per_block - 1;

		jz_nand_read_page_info(&dev_id, first_page, &info);
		jz_nand_read_page_info(&dev_id, last_page, &info2);
		if ((0xFFFFFFFF != info.block_status) || (0xFFFFFFFF != info2.block_status)) {
			cur_block++;
			printf("bad block.");
			continue;
		}

		cur_page = first_page;
		while (cur_page <= last_page) {
			ret = jz_nand_read_page(&dev_id, cur_page, buffer, &info);
			
			if (0 != ret) {
				printf("Read nand data error!\n");
				return (0);
			}
			buffer += g_page_size;
			read_page_count ++;
			cur_page++;
			if (read_page_count >= page_count) {
				printf("OK, read in %d pages.\n", read_page_count);
				return (1);
			}
		}
		cur_block++;
	}

	printf("Too many bad blocks!\n");
	return (0);
}

void LoaderInit(void)
{
	flash_info_t flashinfo;

	jz_nand_init();
	jz_nand_get_info(&flashinfo);
	__dcache_writeback_all();
	config = (int *) ((unsigned int) config | 0xa0000000);
	g_page_size = flashinfo.pnandinfo->dwPageSize;
	if (g_page_size == 0) {
		printf("jz_nand_get_info!\n");
		while (1);
	}

	g_page_per_block = flashinfo.pnandinfo->dwBlockSize / g_page_size;
}

void LoaderTask(void)
{
	unsigned int c;

    unsigned int page_addr  = CONFIG_ADDR;
    unsigned int page_count = CONFIG_LENGTH;

	//卸载USB
	disable_device(1);
	mdelay(100);

    // 初始化OS嘀嗒
    JZ_StartTicker(OS_TICKS_PER_SEC);

	// Loader Nand Init
    LoaderInit();
#if (!GLWX_LOGO)
    // 读入配置信息，内含应用程序信息
    printf("begin to read in information.");
    
    
    if(0 == load_data(page_addr, page_count, (unsigned char*)config)){
        printf("Read config info failure!\n");
        while(1);
    }
    
    //printfdata((unsigned char *)config,512);
    // 将应用程序读入内存
    printf("begin to read in application module.\n");
    page_addr  = config[2];
    page_count = config[3] / (g_page_size/IMG_AND_DEF_BIN_PAGE_SIZE);
    printf("page_addr=%d, page_count=%d, buffer=0x%08X\n",page_addr, page_count, config[4]);

	__dcache_writeback_all();
	unsigned int addr = config[4] | 0xa0000000;
	printf("page_addr=%d, page_count=%d, buffer=0x%08X\n",page_addr, page_count, addr);
#endif	
#if (!GLWX_LOGO)  
    if(0 == load_data(page_addr, page_count, (unsigned char*)addr))
#else    
    if(0 == load_data(2176, 510, (unsigned char*)0x80001000 ))  //xltao add, page count??????
#endif  
	{
        printf("Read application module failure!\n");
        while(1);
    }

    // 跳转至应用程序入口
    printf("go go go! goto 0x%08X\n", config[5]);

#if (!GLWX_LOGO) //xltao add 
    minios_entry = (void *)config[5];
#else
    minios_entry = (void *)0x80002000;
#endif
    cli();
	__dcache_writeback_all();
	__icache_invalidate_all();
    minios_entry();
}

