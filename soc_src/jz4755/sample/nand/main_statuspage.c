/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/
#include <os_api.h>
#include <nand_api.h>

/* File local definitions */
#define  TASK_START_PRIO 40
#define  TASK_STK_SIZE   1024 *128                      /* Size of each task's stacks (# of WORDs) */

#define NAND_STATUSPAGE_TEST 	1
//#undef printf

OS_TASK_STACK   TaskStartStk[TASK_STK_SIZE];
unsigned char os_malloc_completed = 0;
unsigned char os_init_completed = 0;
/* ********************************************************************* */
/* Local functions */
void __umoddi3()
{;}

void __udivdi3(){;}

#if NAND_STATUSPAGE_TEST
#define UNCACHE(x) ((unsigned int)x | 0xa0000000)
unsigned int data_buf[1 * 1024 * 1024 / 4] __attribute__((aligned(32)));
#define BLOCK_SIZE	512 * 1024
#define CONFIG_SSFDC_NAND_PAGE_PER_BLOCK 128

void jz_nand_hardware_test_info (void)
{
	nand_page_info_t	*info;
	unsigned short *pucinfo;
	unsigned int page,blockid;
	int k ,i;

	jz_nand_init();
	
	pucinfo = (unsigned short *)alignAlloc(32,256*10);
		
	
	//pucinfo = (unsigned short *)(&info);
	
	#if 0
	blockid = 512;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK ;
	memset(pucinfo, 0x3a3a , 256*10);
	info = (nand_page_info_t *)pucinfo ;
	

	jz_nand_erase_block (0,blockid);
	
	for (i =0  ; i < 1 ;i++)	
	{
		jz_nand_write_page_info(0, page, info);
	
	}
	#endif
	page = 128256;
	for(i = 0;i < 1;i++,page++)
	{
		memset(pucinfo, 0x3a3a , 256*10);
		info = (nand_page_info_t *)pucinfo ;
		jz_nand_read_page_info(0, page, info);
		
		printf("nand read successful!\r\n");
		
		unsigned char *tempbuf = (unsigned char *)pucinfo;
		for(k = 0;k < 218/8; k++)
			printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++);
		printf("%02x %02x\n",*tempbuf++,*tempbuf++);
	}
	//while(1);
}
void jz_nand_hardware_test_write_page (void)
{
	unsigned int page,blockid;
	unsigned char *data_ptr;
	unsigned int *mybuffer;
	int k,i;
	
	//data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	data_ptr = (unsigned char *)UNCACHE(data_buf);
	mybuffer = (unsigned int *)data_ptr;

	jz_nand_init();
	//jz_nand_get_info(&flashinfo);
	blockid = 1000;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	
	jz_nand_erase_block(0,blockid);
	
	for(i = 0;i < 1;i++)
	{
		for(k = 0;k < BLOCK_SIZE/4 ;k++)
			mybuffer[k] = k;

		jz_nand_write_page(0, page ,data_ptr,0);
				
		page++;
	}
		
	printf("page test ok!!\n");
	//while(1);
}

#if 0

//move jz_nand_write_page_withoutecc() to jz_nand.c
int jz_nand_write_page_withoutecc(int page, unsigned char *data)
{
	int ret;
	
	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
	#endif
	
	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif
	
	unsigned int startpage = page;
	OP_NAND_LOCK();	
		//printf("parsize =%d\n",parsize);	
	page = SelectChip(startpage);

	ret = nand_write_page_withoutecc(page,data);
	
	DeSelectChip(startpage);
	OP_NAND_UNLOCK();
	
	#if BACHMARK
	if (dev_id == 0)
	{
		ulTimeEnd = Get_PerformanceCounter();
		g_ulJZTotalWrite += (ulTimeEnd - ulTimeStart) / 6;	
	}
	#endif
	
	return ret;
}
//move nand_write_page_withoutecc() to nandchipdma.c
int nand_write_page_withoutecc(int page, unsigned char *data)
{
	int i,j;
	unsigned char *ptr;
	unsigned int dwDescrIndex = 0;
	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif
	printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	
	nand_read_oob(page,info);
	
	nand_erase_block(0, page);
	
	NandSendWriteAddr(page,0);
	NandWriteDesSet(dwDescrIndex,(void *)&NAND_IO_ADDR, data, g_dwbytesperpage);
	dwDescrIndex++;
	ptr = (unsigned char *) info;
	NandWriteOObDesSet(dwDescrIndex ,(void *)&NAND_IO_ADDR, ptr, g_dwoobsize);//write info data	
	#if DMA_INTERRUPT_ENABLE
		pnandWdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN; 
		pnandWdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
	#else
		pnandWdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN); 
	#endif
	dwDescrIndex++;
	dma_cache_wback_inv((unsigned long)pnandWdesc[0], (dwDescrIndex << 5));
		
	if ( NandWriteMemcpyEnableTEST())
	{
		printf("NandWriteMemcpyEnableTEST timeout\n");
		return -EIO;
	}
	
	CLEAR_RB();
	
	REG_NAND_CMD = NAND_CMD_PAGE_PROGRAM_STOP;
	
	#if NAND_INTERRUPT
		unsigned char err;
	
		os_SemaphorePend(rb_sem,PAGE_TIMEOUT,&err);
		if(err)
		{
			printf("!!!!!!!error!!! %s() page: 0x%x \n", __FUNCTION__, page);
			goto write_error;
		}
	#else
		nand_wait_ready();
	#endif
	
	REG_NAND_CMD = NAND_CMD_READ_STATUS;
	i = g_dwoobsize * JZ_WAIT_STAUTS_DELAY * 1000;
	while((NAND_IO_ADDR & 0x01) &&(i--)) ;
	
	if(i > 0)
	{	
		return 0;
	}
write_error:
	printf("write page: 0x%x failed\n", page);
	return -EIO;
}
#endif

void jz_nand_write_page_withoutecc_test(void)
{
	unsigned int page,blockid;
	unsigned char *data_ptr;
	unsigned int *mybuffer;
	int k,i;
	nand_page_info_t info;
	
	//data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	data_ptr = (unsigned char *)UNCACHE(data_buf);
	mybuffer = (unsigned int *)data_ptr;

	jz_nand_init();
	//jz_nand_get_info(&flashinfo);
	blockid = 1000;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	
	for(i = 0;i < 1;i++)
	{
		//mybuffer[0] = 0x1111;
		//mybuffer[1] = 0x1110;
		//mybuffer[63] = 0x111E;//0x3fh = 60
		
		for(k = 0;k < 4096/4 ;k++)
			mybuffer[k] = k;
		
		mybuffer[0] = 0x1111;//0x3E
		mybuffer[63] = 0x111E;//0x3F
		
		//mybuffer[1024] = 0x8c88;
		
		jz_nand_write_page_withoutecc(page, data_ptr);
		
		page++;
	}
		
	printf("page test ok!!\n");
	
}

void jz_nand_hardware_test_read_page (void)
{
	unsigned int page,blockid;
	unsigned char *readbuff;
	unsigned int *mybuffer;
	int k,i;
	
	readbuff = (unsigned char *)alloc(BLOCK_SIZE);	
	mybuffer =(unsigned int *)readbuff;
	
	jz_nand_init();
	//jz_nand_get_info(&flashinfo);
	blockid = 1000;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;

	for(i = 0;i < 1;i++)
	{		
		for(k = 0;k < BLOCK_SIZE/4;k++)
			mybuffer[k] = 0x0;
			
		jz_nand_read_page(0, page ,readbuff, NULL);
		
		page++;
	}

	#if 1
	for(k = 0;k < 4096 / 4; k++)
	{
		if (k != mybuffer[k] )//*data_ptr++ != *dataread_ptr++ )
		{
			printf("read sector data error, k = %d,data_ptr[0x%x]\r\n",k,mybuffer[k]);
			break;
		}
	}
	
	
	printf("page test ok!!\n");
	while(1);
	#endif
	unsigned char *tempbuf = readbuff;//pucinfo;
	for(k = 0;k < 512/8; k++)
		printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++);
	//while(1);
	
}

void jz_nand_hardware_test_multi_write (void)
{
	nand_statuspage_t *infopage;
	unsigned int page,blockid;
	unsigned char *data_ptr ,*readbuff;
	unsigned int *mybuffer ,*mybuffer1;
	unsigned char	*mask;
	int k ,i;
	
	jz_nand_init();

	data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	mask = (unsigned char *)alloc(CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	mybuffer = (unsigned int *)data_ptr;
	infopage = (nand_statuspage_t *)alloc(sizeof(nand_statuspage_t));
	
	memset(infopage, 0x77, sizeof(nand_statuspage_t));
	
	blockid = 1000;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	
	for(k = 0;k < BLOCK_SIZE/4;k++)
		mybuffer[k] = k;

	jz_nand_erase_block(0,blockid);
	
	memset(mask, 0xFF, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	
	printf("%s:page = %d\n",__FUNCTION__,page);
	#if 0
	for (i =0 ;i< 64 ;i++)
	{
		jz_nand_write_page(0, (page + i), data_ptr,0);
		data_ptr +=8192;
	}
	#endif

	jz_nand_multiwrite(0, page, 255, mask, data_ptr,infopage);
	printf("write over!\n");
}

void jz_nand_hardware_test_multi_read (void)
{
	nand_statuspage_t *infopage;
	unsigned int page,blockid;
	unsigned char *data_ptr;
	unsigned int *mybuffer;
	unsigned char	*mask;
	int k ,i;
	jz_nand_init();
	data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	mask = (unsigned char *)alloc(CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	mybuffer = (unsigned int *)data_ptr;
	infopage = (nand_statuspage_t *)alloc(sizeof(nand_statuspage_t));
	
	memset(infopage, 0, sizeof(nand_statuspage_t));
	
	blockid = 1000;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	//page = 0x1c480;
	for(k = 0;k < BLOCK_SIZE/4;k++)
		mybuffer[k] = 0;
	
	memset(mask, 0x0, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);


#if 0
	for (i =0 ; i<2 ;i++)
	{
		jz_nand_read_page(0, page + i, data_ptr,info);
		//data_ptr +=8192;
		//info ++ ;
	}
#endif 
	
	//jz_nand_read_page(0, page, data_ptr, info);
	jz_nand_multiread(0 , page, 255, mask, data_ptr, infopage);
	
	//jz_nand_read_page(0, page , data_ptr, info);
	//jz_nand_read_page_info(0,page ,info);
	
	
	for(k = 0;k < ( BLOCK_SIZE - 4096 )/ 4; k++)
	{
		if (k != mybuffer[k] )//*data_ptr++ != *dataread_ptr++ )
		{
			printf("read sector data error, k = %d,data_ptr[0x%x],dataread_ptr[0x%x]\r\n",k,mybuffer[k]);
			return;
		}
	}

	printf("nand read test successful!\r\n");
/*	
	unsigned char *tempbuf = (unsigned char *)data_ptr;//pucinfo;
	for(k = 0;k < 4096/8; k++)
		printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++);
*/	
}

void jz_nand_hardware_test(void)
{
	nand_page_info_t	*info;
	unsigned char *pucinfo;
	unsigned int page,blockid;
	unsigned char *data_ptr;
	unsigned char *dataread_ptr;
	unsigned char *puctemp;
	unsigned char *puctemp1;
	unsigned int *mybuffer;
	unsigned int *mybufferread;
	unsigned char	*mask;
	int k,i;
	flash_info_t	flashinfo;
	
	jz_nand_init();
	jz_nand_get_info(&flashinfo);
	data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	dataread_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	mask = (unsigned char *)alloc(CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	pucinfo = (unsigned char *)alloc(CONFIG_SSFDC_NAND_PAGE_PER_BLOCK * 64);
	mybuffer = (unsigned int *)data_ptr;
	mybufferread = (unsigned int *)dataread_ptr;
	info = (nand_page_info_t *)pucinfo;
	blockid = 512;
	#if 0

	info = (nand_page_info_t *)pucinfo;
	//memset(pucinfo, 0x0, 64 * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	
	for(k = 0;k < BLOCK_SIZE/4;k++)
	{
		mybuffer[k] = k+1;
		mybufferread[k] = 0;
	}
	
	//blockid = (flashinfo.dwFSTotalBlocks / 2 + 5000);
	for(i = 0;i < (flashinfo.dwFSTotalBlocks / 2 + 5000);i++)//flashinfo.dwFSTotalBlocks / 2
	{	
		puctemp = data_ptr;
		puctemp1 = dataread_ptr;
		page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
		jz_nand_erase_block(0,blockid);
		memset(mask, 0xf, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
		//jz_nand_multiwrite(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK, mask, data_ptr);
		//jz_nand_write_page(0, page,data_ptr, info);	
		memset(mask, 0x0, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
		//jz_nand_multiread(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK, mask, dataread_ptr);
		//jz_nand_read_page(0, page,dataread_ptr, info);
		//jz_nand_read_page_info(0, page, info);
			
		for(k = 0;k < BLOCK_SIZE; k++)
		{
			if (*puctemp++ != *puctemp1++ )
			{
				printf("read sector data error, k = %d,data_ptr[0x%x],dataread_ptr[0x%x]\r\n",k,*(--puctemp),*(--puctemp1));
				
				//while (1);
			}
		}
		
		printf("\r\nTest OK!xyzhang\r\n");
		blockid++;
	}
	#endif
	#if 0
	
	info = (nand_page_info_t *)pucinfo;
	for(k = 0;k < BLOCK_SIZE/4;k++)
	{
		mybuffer[k] = k+1;
		mybufferread[k] = 0;
	}
	for(i = 0;i < (flashinfo.dwFSTotalBlocks / 2);i++)
	{	
		page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
		jz_nand_erase_block(0,blockid);
		memset(mask, 0xf, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
		//jz_nand_multiwrite(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK, mask, data_ptr);
		blockid++;
	}
	#else
	for(k = 0;k < BLOCK_SIZE/4;k++)
	{
		mybuffer[k] = k+1;
		mybufferread[k] = 0;
	}
	blockid = 512;

	for(i = 0;i < (flashinfo.dwFSTotalBlocks / 2);i++)
	//while(1)
	{	
		puctemp = data_ptr;
		puctemp1 = dataread_ptr;
		page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
		memset(mask, 0x0, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
		//jz_nand_multiread(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK, mask, dataread_ptr);
					
		for(k = 0;k < BLOCK_SIZE; k++)
		{
			if (*puctemp++ != *puctemp1++ )
			{
				printf("read sector data error, k = %d,data_ptr[0x%x],dataread_ptr[0x%x]\r\n",k,*(--puctemp),*(--puctemp1));
				break;
			}
		}
		
		printf("\r\nTest OK!xyzhang\r\n");
		blockid++;
	}
	#endif
	printf("\r\nTest OK!xyzhang\r\n");
	while (1);
}
void jz_nand_hardware_test_mask (void)
{
	nand_page_info_t	*info;
	unsigned char *pucinfo;
	unsigned int page,blockid;
	unsigned char *data_ptr;
	unsigned char *dataread_ptr;
	unsigned int *mybuffer;
	unsigned int *mybufferread;
	unsigned char	*mask;
	int k,i;
	jz_nand_init();
	data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	dataread_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	mask = (unsigned char *)alloc(CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	pucinfo = (unsigned char *)alloc(CONFIG_SSFDC_NAND_PAGE_PER_BLOCK * 64);
	mybuffer = (unsigned int *)data_ptr;
	mybufferread = (unsigned int *)dataread_ptr;
	info = (nand_page_info_t *)pucinfo;
	blockid = 512;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	
	info = (nand_page_info_t *)pucinfo;
	//memset(pucinfo, 0x0, 64 * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	
	for(k = 0;k < BLOCK_SIZE/4;k++)
	{
		mybuffer[k] = k+1;
		mybufferread[k] = k+1;
	}	
	jz_nand_erase_block(0,blockid);
	
	memset(mask, 0xF, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	//jz_nand_multiwrite(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK, mask, data_ptr);
	
	memset(mask, 0x0, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	
	//jz_nand_multiread(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK, mask, dataread_ptr);

	//for (i = 0; i < CONFIG_SSFDC_NAND_PAGE_PER_BLOCK * 4;i++)
	//{
		for(k = 0;k < BLOCK_SIZE; k++)//BLOCK_SIZE
		{
			if (*data_ptr++ != *dataread_ptr++ )
			{
				printf("read sector data error, k = %d,data_ptr[0x%x],dataread_ptr[0x%x]\r\n",k,*(--data_ptr),*(--dataread_ptr));
				printf("i = %d\n",i);
				while (1);
			}
		}
		//data_ptr += 512;
		//dataread_ptr += 512;
	//}
		
	printf("\r\nTest OK!xyzhang\r\n");
	
	while (1);
}


void jz_nand_hardware_test_speed (void)
{
	nand_statuspage_t *infopage;
	unsigned int page,blockid;
	unsigned char *data_ptr;
	unsigned char *dataread_ptr;
	unsigned int *mybuffer;
	unsigned int *mybufferread;
	unsigned char	*mask;
	unsigned int 	k,i, j;
	unsigned long	ulTimeStart = 0,ulTimeEnd = 0,ulTime,ulSpeed;
	jz_nand_init();

	//data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	data_ptr = (unsigned char *)UNCACHE(data_buf);
	dataread_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	mask = (unsigned char *)alloc(CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
	mybuffer = (unsigned int *)data_ptr;
	mybufferread = (unsigned int *)dataread_ptr;
	
	infopage = (nand_statuspage_t *)alloc(sizeof(nand_statuspage_t));
	memset(infopage, 0x0, sizeof(nand_statuspage_t));
	
	for(k = 0;k < BLOCK_SIZE/4;k++)
		mybuffer[k] =k;
	//jz_nand_speed_mode(1);
/////////////////////test speed and time////////////////////////////
	
	for (j =0 ; j<1; j++)
	{	
		blockid = 512;
		for(k = 0;k< 2; k++)
		{	
			jz_nand_erase_block(0,(blockid + k));
		}
		
		page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
		memset(mask, 0xF, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
		#if 1
		Init_PerformanceCounter();
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
		#endif
		 
		for(k = 0;k< 2; k++)
		{
			jz_nand_multiwrite(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK - 1, mask, data_ptr, infopage);
			page += CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;	
		}
		
		#if 1
		ulTimeEnd = Get_PerformanceCounter();
		ulTime = (ulTimeEnd - ulTimeStart) / 6000;
		ulSpeed = (BLOCK_SIZE*k) / ulTime;// KB/S
		printf("%d %d\n", ulSpeed,ulTime);
		#endif 
	}	
	printf("write over!\n");
	//Init timer
	
	for (j =0 ; j<1; j++)
	{
		blockid = 512;
		page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
		
		memset(mask, 0x0, (CONFIG_SSFDC_NAND_PAGE_PER_BLOCK));
		#if 1
		Init_PerformanceCounter();
		ulTimeStart = ulTimeEnd = Get_PerformanceCounter();
		//ulTimeStart = ulTimeEnd;
		#endif 
		
		for(k = 0;k<2; k++)
		{
			jz_nand_multiread(0, page, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK - 1, mask, dataread_ptr, infopage);
			page += CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
			memset(mask, 0x0, CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
			//jz_nand_read_page(0, page + k, data_ptr, infopage);
			
			dataread_ptr += 4096;
		}
		#if 1
		ulTimeEnd = Get_PerformanceCounter();
		ulTime = (ulTimeEnd - ulTimeStart) / 6000;
		ulSpeed = (BLOCK_SIZE*k) / ulTime;// KB/S
		printf(" %d  %d\n", ulSpeed,ulTime);
	//	printf("%dus\n",ulTime);
		#endif 
	}	

	unsigned char *tempbuf = dataread_ptr;//pucinfo;
	for(k = 0;k < 512/8; k++)
		printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++);
	while(1);

	
	for(k = 0;k < (BLOCK_SIZE - 4096)/ 4; k++)
	{
		if (k != mybufferread[k] )//*data_ptr++ != *dataread_ptr++ )
		{
			printf("read sector data error, k = %d,data_ptr[0x%x],dataread_ptr[0x%x]\r\n",k,*(--data_ptr),*(--dataread_ptr));
			while (1);
		}
	}
	
	
	printf("\r\nTest OK!xyzhang\r\n");

	while (1);
}


void jz_nand_ssfdc_test(void)
{
	unsigned int *mybuffer;	
	unsigned char *data_ptr;
	unsigned int i,j,Sector;
	 
	data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	mybuffer = (unsigned int *)data_ptr;
	
	ssfdc_nftl_init();
		
	ssfdc_nftl_open(0);
	ssfdc_nftl_format();
	
	printf("Nand data test\r\n");
	
	#if 1
	for(i = 0;i < BLOCK_SIZE / 4;i++)
		mybuffer[i] = i;
	
	for(i = 0; i < 10; i++)
	{
		Sector = 0 + i * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
		ssfdc_nftl_write(Sector,data_ptr,250);
		printf("ssfdc_nftl_write: %d\r\n",i);
	}
	ssfdc_nftl_flush_cache();
	while(1);
	#endif
	printf("nand read data\r\n");
	for(i = 0; i < 1; i++) 
	{
		for(j = 0;j < BLOCK_SIZE / 4;j++)
			mybuffer[j] = -1;
		Sector = 0 + i * 127;
		ssfdc_nftl_read(Sector,data_ptr,50);
				
		for(j = 0;j < (4096 * 50) / 4;j++)
		{
			if(mybuffer[j] != j)
			{
				printf("read sector data error %d sector %d word->0x%x\r\n",i,j,mybuffer[j]);
				deAlloc(mybuffer);
				while(1);
			}
		}
	}
	
	printf("Test OK!xyzhang\r\n");
	deAlloc(mybuffer);
	while(1);
}

void jz_nand_lbnand_test(void)
{
	unsigned int *mybuffer;	
	unsigned char *data_ptr;
	unsigned int i,j,Sector;
	 
	data_ptr = (unsigned char *)alloc(BLOCK_SIZE);
	mybuffer = (unsigned int *)data_ptr;
	
	NAND_LB_Init();
		
	printf("Nand data test\r\n");
	
	#if 1
	for(i = 0;i < BLOCK_SIZE / 4;i++)
		mybuffer[i] = i;

	Sector = 1000;
	for(i = 0; i < 100; i++)
	{
		NAND_LB_Write(Sector + i,data_ptr);
		printf("NAND_LB_Write: %d\r\n",i);
	}
	printf("NAND_LB_Write Test ok!\r\n");
	Sector = 1000;
	for(i = 0; i < 100; i++)
	{
		NAND_LB_Write(Sector + i,data_ptr);
		printf("NAND_LB_Write: %d\r\n",i);
	}
	printf("NAND_LB_Write Test ok!\r\n");
	NAND_LB_FLASHCACHE();
	printf("NAND_LB_FLASHCACHE Test ok!\r\n");
	while(1);
	#endif
	printf("nand read data\r\n");
	for(i = 0; i < 1; i++) 
	{
		for(j = 0;j < BLOCK_SIZE / 4;j++)
			mybuffer[j] = -1;
		
		Sector = 1000;
		for(i = 0; i < 1000; i++)
		{
			NAND_LB_Read(Sector + i,data_ptr);
			printf("ssfdc_nftl_write: %d\r\n",i);
		}
				
		for(j = 0;j < (4096 * 50) / 4;j++)
		{
			if(mybuffer[j] != j)
			{
				printf("read sector data error %d sector %d word->0x%x\r\n",i,j,mybuffer[j]);
				deAlloc(mybuffer);
				while(1);
			}
		}
	}
	
	printf("Test OK!xyzhang\r\n");
	deAlloc(mybuffer);
	while(1);
}

void NandTest()
{
	printf("Nand data test\r\n");
	//jz_nand_ssfdc_test(); 
	//jz_nand_lbnand_test();
	//jz_nand_hardware_test();
	//jz_nand_hardware_test_speed();
	//jz_nand_hardware_test_mask();
	//jz_nand_hardware_test_write_page();
	//jz_nand_write_page_withoutecc_test();
	//jz_nand_hardware_test_read_page();
	//jz_nand_hardware_test_info(); 
	//jz_nand_hardware_test_multi_write();
	//jz_nand_hardware_test_multi_read();
	//jz_nand_hardware_test_speed();
	while(1);
}
#else
void NandTest()
{
	printf("Nand data test\r\n");
	while(1);
}
#endif

void TaskStart (void *data)
{
	unsigned char i;
	char key;
	data = data;                            /* Prevent compiler warning */
	
	JZ_StartTicker(OS_TICKS_PER_SEC);	/* os_cfg.h */
	printf("uC/OS-II, The Real-Time Kernel MIPS Ported version\n");
	printf("EXAMPLE #1 %s %s\n",__DATE__,__TIME__);
	while(0)
	{
		if(serial_tstc())
		{
			key = serial_getc();
			printf("you pressed: %c\n",key);
			if(key == 0x1B) 
			{        /* see if it's the ESCAPE key */
				printf(" Escape display of statistic\n");
				os_TaskDelete(TASK_START_PRIO);
			}
		}
		printf("time: %d\n",os_TimeGet());
		os_TimeDelayHMSM(0, 0, 1, 0);     /* Wait one second */
	}
	NandTest();
}


/* ********************************************************************* */
/* Global functions */

void APP_vMain (void)
{
	heapInit();
	os_malloc_completed = 1;
	os_init_completed = 1;
	printf("MINIOS:Start test----\n");	
	os_TaskCreate(TaskStart, 
				(void *)0, 
				(void *)&TaskStartStk[TASK_STK_SIZE - 1], 
				TASK_START_PRIO);
	
	os_SystemStart();                              /* Start multitasking */
	
	while(1);
}
/* ********************************************************************* */
