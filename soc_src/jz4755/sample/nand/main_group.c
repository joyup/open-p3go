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

#define NAND_GROUP_TEST 	0
//#undef printf

OS_TASK_STACK   TaskStartStk[TASK_STK_SIZE];
unsigned char os_malloc_completed = 0;
unsigned char os_init_completed = 0;
/* ********************************************************************* */
/* Local functions */
void __umoddi3()
{;}

void __udivdi3(){;}

#ifdef NAND_GROUP_TEST

#define BLOCK_SIZE	512 * 1024
#define CONFIG_SSFDC_NAND_PAGE_PER_BLOCK 256
void jz_nand_hardware_test_info (void)
{
	nand_page_info_t	info;
	unsigned char *pucinfo;
	unsigned int page,blockid;
	int k;
	jz_nand_init();
	
	blockid = 512;
	page = blockid * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	pucinfo = (unsigned char *)(&info);
	memset(pucinfo, 0xff, 64);
	//memset(pucinfo, 0, 16);
	
	unsigned char *tempbuf = pucinfo;
	*tempbuf++ = 0x7;
	*tempbuf++ = 0x0;
	*tempbuf++ = 0x0;
	*tempbuf++ = 0xFC;
	tempbuf = pucinfo;
	for(k = 0;k < 64/8; k++)
	{
		printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++);	
	}
	page = 101377;	
	blockid = page / CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	jz_nand_erase_block(0,blockid);
	
//	page = page + 1;

	jz_nand_write_page_groupinfo(0, page, &info);
	jz_nand_read_page_groupinfo(0, page, &info);
	
	printf("nand read successful!\r\n");
	
	tempbuf = pucinfo;
	for(k = 0;k < 64/8; k++)
		printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++,*tempbuf++);
	while(1);
}


void jz_nand_lb_SingleSec_test(void)
{
	unsigned int *mybuffer;	
	unsigned char *data_ptr;
	unsigned int i,j,Sector,StartSector = 512;
	nand_group NandGroup;
	
	data_ptr = (unsigned char *)alignAlloc(32,BLOCK_SIZE);
	mybuffer = (unsigned int *)data_ptr;
	
	NAND_LB_Init();
		
	printf("Nand data test\r\n");
#if 1
	for(i = 0;i < BLOCK_SIZE / 4;i++)
		mybuffer[i] = i;
	
	for(i = 0; i < 1; i++)
	{
		NAND_LB_Write(StartSector, data_ptr);
	}
#endif

#if 1
	printf("nand read data\r\n");
	for(i = 0;i < BLOCK_SIZE / 4;i++)
			mybuffer[i] = 0xffffffff;
	for(i = 0; i < 1; i++) 
	{
		NAND_LB_Read(StartSector, data_ptr);
				
		for(j = 0;j < 2048 / 4;j++)
		{
			if(mybuffer[j] != j)
			{
				printf("read sector data error %d word->0x%x\r\n",j,mybuffer[j]);
				printf("mybuffer: %x %x %x %x\n",mybuffer[j++],mybuffer[j++],mybuffer[j++],mybuffer[j++]);
				deAlloc(mybuffer);
				while(1);
			}
		}
	}
#endif
	printf("Test OK!xyzhang\r\n");
	deAlloc(mybuffer);
	//while(1);
}

void jz_nand_lb_MultiSec_test(void)
{
	unsigned int *mybuffer;	
	unsigned char *data_ptr;
	unsigned int i,j,Sector,StartSector = 512;
	nand_group NandGroup;
	
	data_ptr = (unsigned char *)alignAlloc(32,BLOCK_SIZE);
	mybuffer = (unsigned int *)data_ptr;
	
	NAND_LB_Init();
		
	printf("Nand data test\r\n");
#if 1
	for(i = 0;i < BLOCK_SIZE / 4;i++)
		mybuffer[i] = i;
	
	for(i = 0; i < 10; i++)
	{
		NAND_LB_MultiWrite(StartSector, data_ptr,CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
		StartSector += CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
	}
#endif

#if 1
	printf("nand read data\r\n");
	for(i = 0;i < BLOCK_SIZE / 4;i++)
			mybuffer[i] = 0xffffffff;
	for(i = 0; i < 1; i++) 
	{
		NAND_LB_MultiRead(StartSector, data_ptr,CONFIG_SSFDC_NAND_PAGE_PER_BLOCK);
				
		for(j = 0;j < BLOCK_SIZE / 4;j++)
		{
			if(mybuffer[j] != j)
			{
				printf("read sector data error %d word->0x%x\r\n",j,mybuffer[j]);
				printf("mybuffer: %x %x %x %x\n",mybuffer[j++],mybuffer[j++],mybuffer[j++],mybuffer[j++]);
				deAlloc(mybuffer);
				while(1);
			}
		}
	}
#endif
	printf("Test OK!xyzhang\r\n");
	deAlloc(mybuffer);
	while(1);
}

void jz_nand_ssfdc_test(void)
{
	unsigned int *mybuffer;	
	unsigned char *data_ptr;
	unsigned int i,j,Sector,StartSector = 512;
	nand_group NandGroup;
	
	data_ptr = (unsigned char *)alignAlloc(32,BLOCK_SIZE);
	mybuffer = (unsigned int *)data_ptr;
	
	ssfdc_nftl_init();
	ssfdc_nftl_open();
	
	ssfdc_nftl_format();
	for(i = 0;i < BLOCK_SIZE / 4;i++)
		mybuffer[i] = i;
#if 0 //Write without copyback
	for(i = 0; i < 1; i++)
	{
		NandGroup.page_addr = StartSector + i * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;
		NandGroup.page_count = CONFIG_SSFDC_NAND_PAGE_PER_BLOCK;// + 128;
		NandGroup.databuff = (unsigned int *)data_ptr;
		NandGroup.nextgroup = NULL;
		ssfdc_nftl_write(&NandGroup);
	}
#endif
#if 1	//Write with copyback
	for(i = 0; i < 1; i++)
	{
		NandGroup.page_addr = StartSector + i * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK + 8;
		NandGroup.page_count = 256 - 9;
		NandGroup.databuff = (unsigned int *)data_ptr;
		NandGroup.nextgroup = NULL;
		ssfdc_nftl_write(&NandGroup);
	}
#endif
#if 1 //Read
	printf("nand read data\r\n");
	for(i = 0;i < BLOCK_SIZE / 4;i++)
			mybuffer[i] = 0xffffffff;
	for(i = 0; i < 1; i++) 
	{
		NandGroup.page_addr = StartSector + i * CONFIG_SSFDC_NAND_PAGE_PER_BLOCK + 8;
		NandGroup.page_count = 256 - 9;
		NandGroup.databuff = (unsigned int *)data_ptr;

		NandGroup.nextgroup = NULL;
		ssfdc_nftl_read(&NandGroup);
				
		for(j = 0;j < NandGroup.page_count / 4;j++)
		{
			if(mybuffer[j] != j)
			{
				printf("read sector data error %d word->0x%x\r\n",j,mybuffer[j]);
				printf("mybuffer: %x %x %x %x\n",mybuffer[j++],mybuffer[j++],mybuffer[j++],mybuffer[j++]);
				deAlloc(mybuffer);
				while(1);
			}
		}
	}
#endif
	printf("Test OK!xyzhang\r\n");
	deAlloc(mybuffer);
	while(1);
}

void NandTest()
{
	printf("Nand data test\r\n");
	jz_nand_ssfdc_test();
	//jz_nand_lb_SingleSec_test();
	//jz_nand_lb_MultiSec_test();
	//jz_nand_hardware_test_info();
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
