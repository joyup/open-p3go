/********************** BEGIN LICENSE BLOCK ************************************
 *
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
 *  Author:  <xyzhang@ingenic.cn>
 *
 *  Create:   2008-09-24, by xyzhang
 *
 *
 *
 *******************************************************************************
 */
#include <os_api.h>
#include <header.h>
#include <jz_nand.h>
#include <nand_cfg.h>
#include <dm.h>

//#define TEST_FOR_POWER_ZZHANG
//#define JZ_DEBUG
//#undef printf

#if BACHMARK
	unsigned long	ulTimeStart = 0,ulTimeEnd = 0;
	unsigned long g_ulJZTotalRead  = 0;
	unsigned long g_ulJZTotalWrite = 0;
#endif

#define MAX_CHIP_NUM 4

unsigned int g_dwchipnum = 1;
unsigned int g_dwplanenum = 1;
unsigned int g_dwplanemask = 0;
unsigned int g_dwnandperblock = 0;
unsigned int g_dweccsteps = 4;
unsigned int g_dwbytesperpage = 2048;
unsigned int g_dwoobsize = 64;
unsigned int g_dweccblocksize = 256;

static unsigned int g_dwpmask[MAX_CHIP_NUM];


unsigned int NANDFLASH_BASE = 0xB8000000;

unsigned int g_dwTSZ = DES_TSZ_32BIT;
unsigned int g_dwUintSize = 4;

static NandChipInfo chip_cs[MAX_CHIP_NUM_RES] = {0};
static unsigned int base_chip_addr[MAX_CHIP_NUM_RES] = {0xB8000000,0xB4000000,0xAC000000,0xA8000000};
static unsigned int nand_chip[MAX_CHIP_NUM_RES] = {
	NFCSR_NFE1 | NFCSR_NFCE1,
	NFCSR_NFE2 | NFCSR_NFCE2,
	NFCSR_NFE3 | NFCSR_NFCE3,
	NFCSR_NFE4 | NFCSR_NFCE4
	};

static unsigned int emc_smcr[MAX_CHIP_NUM_RES] = {A_EMC_SMCR1,A_EMC_SMCR2,A_EMC_SMCR3,A_EMC_SMCR4};

/* global mutex between yaffs2 and ssfdc */

#define NAND_SIZE		2	//nand.bin size,1 block
#define LOADER_SIZE		4	//loader.bin size,1 block
#define DEF_SIZE		0	//def_boot.bin size,2 blocks
#define IMG_SIZE		0	//img_boot.bin size,60 blocks
#define MINIOS_SIZE	 	8	//minios.bin size,192 blocks
#define RES_SIZE		256	//res.bin size,512 blocks

#define NAND_RESERVE_SIZE  \
(NAND_SIZE + LOADER_SIZE + DEF_SIZE + IMG_SIZE + MINIOS_SIZE + RES_SIZE)
//block
//#define NAND_RESERVE_SIZE  \
//((NAND_SIZE + LOADER_SIZE + DEF_SIZE + IMG_SIZE + MINIOS_SIZE + RES_SIZE) / 128)

#define MULTI_TASK_SUPPORT 1
#if MULTI_TASK_SUPPORT
	static  OS_EVENT  *SemDeviceOps;
	#define OP_NAND_LOCK() do{ \
								unsigned char  err; \
								os_SemaphorePend(SemDeviceOps, 200, &err); \
							}while(0)
	#define OP_NAND_UNLOCK() do{ \
									unsigned char  err; \
									os_SemaphorePost(SemDeviceOps); \
								}while(0)
#else
	#define OP_NAND_LOCK()
	#define OP_NAND_UNLOCK()
#endif

#define NAND_RESET() while(1)

//-----------------------------------------------------------------------------
//
//	Functions
//
//-----------------------------------------------------------------------------
static inline unsigned int SelectChip(unsigned int page)
{
	unsigned int i;
	for(i = 0;i < g_dwchipnum;i++)
	{
		if((page >= chip_cs[i].start_pages) && (page < chip_cs[i].end_pages))
			break;
	}
	if(i == g_dwchipnum)
	{
		printf("page > totalpage error!");
		NAND_RESET();
	}
	unsigned int cs = chip_cs[i].chipcs;
	g_dwplanemask = g_dwpmask[i];
	//printf("chip %d,g_dwplanemask 0x%x!\n",i,g_dwplanemask);
	NANDFLASH_BASE = base_chip_addr[cs];
	if(cs == 1)
	{
		OUTREG32(A_GPIO_PXDATC(CS2_GROUP),CS2_BIT);
		OUTREG32(A_EMC_NFCSR,nand_chip[cs]);
	}
	else
		OUTREG32(A_EMC_NFCSR,nand_chip[cs]);

	return (page - chip_cs[i].start_pages);
}

static inline void DeSelectChip(unsigned int page)
{
		OUTREG32(A_GPIO_PXDATS(CS2_GROUP),CS2_BIT);
		OUTREG32(A_EMC_NFCSR,0);
}

static unsigned int IsNandSupport(unsigned int dwNandInfoID,unsigned int dwMClock)
{
	unsigned int dwData = 0, dwTime = 0, dwSMCRData;
	#define MCLOCK_BEAT (1000000000 / (dwMClock / 1000))
	#define	NAND_TIMING_MARGIN		0	// Percent

	NAND_INFO	*cur_NandInfo;
	cur_NandInfo=(NAND_INFO	*)&g_NandInfo[dwNandInfoID];
	dwSMCRData = 0;
	// Tas
	dwTime = (cur_NandInfo->Tals * 100 + NAND_TIMING_MARGIN) / 100;
	dwData = (dwTime * 1000 + MCLOCK_BEAT - 1) / MCLOCK_BEAT;
	dwData = dwData > 7 ? 7 : dwData;
	dwSMCRData |= dwData << EMC_SMCR_TAS_BIT;
	// Tah
	dwTime = (cur_NandInfo->Talh * 100 + NAND_TIMING_MARGIN) / 100;
	dwData = (dwTime * 1000 + MCLOCK_BEAT - 1) / MCLOCK_BEAT;
	dwData = dwData > 7 ? 7 : dwData;
	dwSMCRData |= dwData << EMC_SMCR_TAH_BIT;
	// Twp
	dwTime = (cur_NandInfo->Twp * 100 + NAND_TIMING_MARGIN) / 100;
	dwData = (dwTime * 1000 + MCLOCK_BEAT - 1) / MCLOCK_BEAT;
	dwData = dwData > 12 ? 0xF : dwData;
	dwSMCRData |= dwData << EMC_SMCR_TBP_BIT;
	// Trp
	dwTime = (cur_NandInfo->Trp * 100 + NAND_TIMING_MARGIN) / 100;
	dwData = (dwTime * 1000 + MCLOCK_BEAT - 1) / MCLOCK_BEAT;
	dwData = (dwData + 1) > 12 ? 0xF : (dwData + 1);
	dwSMCRData |= dwData << EMC_SMCR_TAW_BIT;  //add 1 for supportting 16GB nand.
	//dwSMCRData |= (dwData) << EMC_SMCR_TAW_BIT;

	dwTime = cur_NandInfo->Trhw > cur_NandInfo->Twhr ? cur_NandInfo->Trhw : cur_NandInfo->Twhr;
	dwTime = (dwTime * 100 + NAND_TIMING_MARGIN) / 100;
	dwData = (dwTime * 1000 + MCLOCK_BEAT - 1) / MCLOCK_BEAT;
	dwData = dwData > 15 ? 0xF : dwData;
	dwSMCRData |= dwData << EMC_SMCR_STRV_BIT;

	#undef MCLOCK_BEAT
	//dwSMCRData = 0x77FFF000;
	return dwSMCRData;
}

static inline void NandSetReg(unsigned int selid,unsigned int dwSMCRData)
{
	unsigned int dwData = 0;
	OP_NAND_LOCK();
	volatile unsigned int *reg_emcr = (unsigned int *)emc_smcr[selid];
	dwData = *reg_emcr;
	*reg_emcr = (dwSMCRData | (dwData & 0x0FF));
	OP_NAND_UNLOCK();
	printf("NAND Flash: cs = %d Timing = 0x%x\r\n", selid,dwSMCRData);
}

static unsigned int NandFindID(unsigned int id,unsigned int extid,int curpoint)
{
	int i,dwNum;
	dwNum = sizeof(g_NandInfo)/sizeof(g_NandInfo[0]);
	for(i = curpoint;i < dwNum;i++)
	{
		if(g_NandInfo[i].dwNandID == id)
		{
			if(extid != 0)
			{
				int j;
				unsigned char c1,c2;
				for(j = 0;j < 4;j++)
				{
					c1 = (extid >> (8 * j)) & 0xff;
					if(c1 != 0)
					{
						c2 = (g_NandInfo[i].dwNandExtID >> (8 * j)) & 0xff;
						if(c1 != c2)
							break;
					}
				}
				// match  ext == 0 is match
				if(j == 4)
					break;
			}else break;
		}
	}
	if(i == dwNum)
		return -1;
	return i;
}

#define MAX_ID_NUM 5

static unsigned int NandScanChipID(int chipcs)
{
	unsigned char cVendorID,cDeviceID,temp;
	unsigned int dwNandID;
	NANDFLASH_BASE = base_chip_addr[chipcs];
	if(chipcs == 1)
	{
		OUTREG32(A_GPIO_PXDATC(CS2_GROUP),CS2_BIT);
		OUTREG32(A_EMC_NFCSR,nand_chip[chipcs]);
	}
	else
		OUTREG32(A_EMC_NFCSR,nand_chip[chipcs]);
	
	NandSendReset();
	
	REG_NAND_CMD = NAND_CMD_READ_ID1;
	REG_NAND_ADDR = NAND_CMD_READ1_00;
	cVendorID = NAND_IO_ADDR;
	cDeviceID = NAND_IO_ADDR;
	dwNandID = ((cVendorID & 0xff) << 8)  | (cDeviceID & 0xff);
	printf("NandScanChipID:NAND ID = 0x%x[0x%x,0x%x] \n",dwNandID,cVendorID,cDeviceID);
	//scan ext id to match
	unsigned int dwnandinfoid = 0,dwnandextid,dwnandid = 0;
	dwnandinfoid = NandFindID(dwNandID,0,0);
	unsigned int olddwnandinfoid = dwnandinfoid;
	unsigned int idnum = 0;
	while(dwnandinfoid != -1)
	{
		if(idnum < (MAX_ID_NUM - 2))
			temp =  NAND_IO_ADDR;
		else
			break;
		dwnandid |= (temp & 0xff) << (8 * idnum);
		dwnandinfoid = NandFindID(dwNandID,dwnandid,olddwnandinfoid);
		if(dwnandinfoid != -1)
			olddwnandinfoid = dwnandinfoid;
		idnum++;
	}
	printf("NAND EXTID = 0x%x\n",dwnandid);
	printf("chipcs = %d nandinfo = %d\n",chipcs,olddwnandinfoid);
	if(chipcs == 1)
		OUTREG32(A_GPIO_PXDATS(CS2_GROUP),CS2_BIT);
	else
		OUTREG32(A_EMC_NFCSR,0);
	return olddwnandinfoid;
}

static void NandScanIDAll (void)
{
	#ifdef JZ_DEBUG
		printf("%s()\n", __FUNCTION__);
	#endif
	//find id;
	unsigned int dwnandinfoid;
	int count = 0;
	int startpages = 0;
	int pages = 0;
	int i;
	for(i = 0;i < MAX_CHIP_NUM;i++)
	{
		dwnandinfoid = NandScanChipID(i);
		if(dwnandinfoid != -1)
		{
			unsigned int blocksize = g_NandInfo[dwnandinfoid].dwMaxValidBlocks;
			unsigned int d = ((g_NandInfo[dwnandinfoid].dwNandExtID >> 24) & 0xff);
			d++;
			blocksize /= d;
			pages = g_NandInfo[dwnandinfoid].dwBlockSize / g_NandInfo[dwnandinfoid].dwPageSize;
			chip_cs[count].chipcs = i;
			chip_cs[count].id = dwnandinfoid;
			chip_cs[count].start_pages = startpages;
			printf("chip_sel = %d start pages = %d ",chip_cs[count].chipcs,startpages);
			startpages += blocksize * pages;

			chip_cs[count].end_pages = startpages;
			g_dwpmask[count] = (g_NandInfo[dwnandinfoid].dwPlaneNum >> 4);
			printf("endpages = %d\n",startpages);
			count++;
		}
	}
	if(count == 0)
	{
		printf("not find valid nand id!\n");
		NAND_RESET();
	}

	dwnandinfoid = chip_cs[0].id;
	g_dwchipnum = count;
	g_dwplanenum = g_NandInfo[dwnandinfoid].dwPlaneNum & 0xff;
	g_dwplanemask = (g_NandInfo[dwnandinfoid].dwPlaneNum >> 4);
	printf("jz_nand: g_dwplanemask[0x%x]\n", g_dwplanemask);
	g_dwnandperblock = g_NandInfo[dwnandinfoid].dwBlockSize * g_dwplanenum / g_NandInfo[dwnandinfoid].dwPageSize;
	g_dwbytesperpage = g_NandInfo[dwnandinfoid].dwPageSize;
	g_dwoobsize = g_NandInfo[dwnandinfoid].dwOOBSize;
	g_dweccsteps = g_dwbytesperpage / g_dweccblocksize;

	if (g_dwoobsize < (g_dweccsteps * 13 + ECC_POS))
	{
		g_dweccblocksize = 512;		//8 bit ecc
	}
	else
	{
		g_dweccblocksize = 256;		//16bit ecc
	}

	g_dweccsteps = g_dwbytesperpage / g_dweccblocksize;
}

static int NandInitGpioAsEmc(void)
{
	OUTREG32(A_GPIO_PXFUNS(1),0x00088000);
	OUTREG32(A_GPIO_PXSELC(1),0x00088000);
	OUTREG32(A_GPIO_PXPES(1),0x00088000);

	OUTREG32(A_GPIO_PXFUNS(2),0x30210000);
	OUTREG32(A_GPIO_PXSELC(2),0x30210000);
	OUTREG32(A_GPIO_PXPES(2),0x30210000);

	OUTREG32(A_GPIO_PXFUNC(2),0x08020000);
	OUTREG32(A_GPIO_PXSELC(2),0x08020000);
	OUTREG32(A_GPIO_PXDIRC(2),0x08000000);
	OUTREG32(A_GPIO_PXPES(2),0x08000000);
	OUTREG32(A_GPIO_PXDIRS(2),0x00020000);
	OUTREG32(A_GPIO_PXDATS(2),0x00020000);
	OUTREG32(A_GPIO_PXPEC(2),0x00020000);
}

static int NandHWInit(void)
{
	unsigned int dat;
	int i;
	NandInitGpioAsEmc();
	//initalite all gpio

	for(i = 2;i < MAX_CHIP_NUM;i++)
	{
		OUTREG32(A_GPIO_PXFUNS(2),(1 << (21 + i)));
		OUTREG32(A_GPIO_PXSELC(2),(1 << (21 + i)));
	}

	SETREG32(A_EMC_NFCSR,NFCSR_NFE1);
	dat = INREG32(A_EMC_SMCR1);
	dat &= ~0x0fffff00;

	OUTREG32(A_EMC_SMCR1,(dat | 0x05555500));

	nand_Intr_init();

	#if MULTI_TASK_SUPPORT
		SemDeviceOps  = os_SemaphoreCreate(1);
	#endif

	memset(chip_cs,0,sizeof(NandChipInfo));
	NandScanIDAll();
	unsigned int timing;
	//uninitalite not use gpio
	for(i = 0; i < MAX_CHIP_NUM;i++)
	{
		if(chip_cs[i].end_pages > 0)
		{
			timing = IsNandSupport(chip_cs[i].id, GetCommonClock(CPM_MDIV));
			NandSetReg(chip_cs[i].chipcs,timing);
		}
	}
	return 0;
}

int jz_nand_init(void)
{
	static int ret = 0 ;
	if ( ret > 0 ) return 0;
	ret = 1;
	/////////////////////8bitECC Macro

	NandHWInit();
	#if (DM==1)
	void mng_init_nand(void);
  	mng_init_nand();
  #endif
	return 0;
}

int jz_nand_get_info(pflash_info_t pflashinfo)
{
	NAND_INFO	*cur_NandInfo;
	unsigned int totalblock;
	cur_NandInfo=(NAND_INFO	*)&g_NandInfo[chip_cs[0].id];
	pflashinfo->pnandinfo = cur_NandInfo;
	pflashinfo->dwBytesPerBlock = cur_NandInfo->dwBlockSize * g_dwplanenum;
	pflashinfo->dwDataBytesPerSector = cur_NandInfo->dwPageSize;

	pflashinfo->dwSectorsPerBlock = pflashinfo->dwBytesPerBlock / pflashinfo->dwDataBytesPerSector;

	pflashinfo->dwFSStartBlock = NAND_RESERVE_SIZE / g_dwplanenum; //IMAGE_RSV_LENTH / g_NandInfo.dwBlockSize + 1;

	totalblock =  chip_cs[g_dwchipnum - 1].end_pages / pflashinfo->dwSectorsPerBlock;

	pflashinfo->dwFSTotalBlocks = totalblock  - pflashinfo->dwFSStartBlock;
	pflashinfo->dwFSTotalSectors = totalblock * pflashinfo->dwSectorsPerBlock;

	pflashinfo->dwTotalPhyBlocks = totalblock;

	pflashinfo->dwMaxBadBlocks = 0;//(cur_NandInfo->dwMinValidBlocks) / g_dwplanenum;
	int i,chipnum;
	for(i = 0; i < g_dwchipnum;i++)
	{

		chipnum =(g_NandInfo[chip_cs[i].id].dwNandExtID >> 24) & 0xff;
		chipnum++;
		pflashinfo->dwMaxBadBlocks += (g_NandInfo[chip_cs[i].id].dwMaxValidBlocks - g_NandInfo[chip_cs[i].id].dwMinValidBlocks)/chipnum;
	}
	pflashinfo->dwMaxBadBlocks /= g_dwplanenum;
	#if 0
	printf("dwBytesPerBlock: %d\n",pflashinfo->dwBytesPerBlock);

	printf("dwDataBytesPerSector: %d\n",pflashinfo->dwDataBytesPerSector);

	printf("dwSectorsPerBlock: %d\n",pflashinfo->dwSectorsPerBlock);

	printf("dwFSStartBlock: %d\n",pflashinfo->dwFSStartBlock);

	printf("dwFSTotalBlocks: %d\n",pflashinfo->dwFSTotalBlocks);

	printf("dwFSTotalSectors: %d\n",pflashinfo->dwFSTotalSectors);

	printf("dwTotalPhyBlocks: %d\n",pflashinfo->dwTotalPhyBlocks);

	printf("dwMaxBadBlocks: %d\n",pflashinfo->dwMaxBadBlocks);
	#endif
	//printf("pflashinfo->dwSectorsPerBlock: %d----------------\r\n",pflashinfo->dwSectorsPerBlock);
	return 1;
}

int jz_nand_get_oob_info(unsigned int *ptr)
{
	*ptr =g_dwoobsize ;	
	return 1 ;
}
/*
    test(nand_preconvert can convert)
    nand_convert is convert
*/

int nand_preconvert(PPLL_OPT pllopt)
{
	OP_NAND_LOCK();
	return (pllopt->cpuclock / pllopt->mdiv);
}
int nand_convert(unsigned int mclk)
{
	int i;
	//printf("mclk = %d\n",mclk);
	for(i = 0;i < g_dwchipnum;i++)
	{
		unsigned int timing = IsNandSupport(chip_cs[i].id, mclk);
		unsigned int dwData = 0;
		volatile unsigned int *reg_emcr = (unsigned int *)emc_smcr[chip_cs[i].chipcs];
		dwData = *reg_emcr;
		*reg_emcr = (timing | (dwData & 0x0FF));
	}

	OP_NAND_UNLOCK();
	return 0;
}
#if (DM==1)
static struct dm_jz4740_t nand_dm;
void mng_init_nand(void)
{
	nand_dm.name = "NAND";
	nand_dm.preconvert = nand_preconvert;
	nand_dm.convert = nand_convert;
	dm_register(1, &nand_dm);
}
#endif
int jz_nand_read_page_info (void *dev_id, int page, nand_page_info_t *info)
{
	int ret;
	OP_NAND_LOCK();
	#ifdef JZ_DEBUG
		printf("%s() page: 0x%x\n", __FUNCTION__, page);
	#endif

	unsigned int startpage = page;
	page = SelectChip(startpage);

	ret = nand_read_oob(page,info);

	DeSelectChip(startpage);
	OP_NAND_UNLOCK();
	return ret;
}

int jz_nand_speed_mode(int d)//d == 0 slow;d == 1 fast
{
	if (d == 0)
	{
		g_dwTSZ = DES_TSZ_32BIT;
		g_dwUintSize = 4;
	}
	else if (d == 1)
	{
		g_dwTSZ = DES_TSZ_16BYTE;
		g_dwUintSize = 16;
	}
	else
		return -EIO;

	return 0;
}

int jz_nand_read_statuspage(int page, unsigned char *data, nand_page_info_t *pinfo)
{
	int ret = 0;

	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
	#endif

	#ifdef JZ_DEBUG
		printf("%s() page: 0x%x\n", __FUNCTION__, page);
	#endif

	//printf("parsize =%d\n",parsize);
	OP_NAND_LOCK();
	unsigned int startpage = page;
	page = SelectChip(startpage);

	ret = nand_read_statuspage(page, data, pinfo);

	DeSelectChip(startpage);
	OP_NAND_UNLOCK();
	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		g_ulJZTotalRead += (ulTimeEnd - ulTimeStart) / 6;
	//printf("nandchipdma:g_ulJZTotalRead = %dms\n",g_ulJZTotalRead);
	#endif
	return ret;
}


int jz_nand_write_statuspage(int page, unsigned char *data)
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

	ret = nand_write_statuspage(page,data);

	DeSelectChip(startpage);
	OP_NAND_UNLOCK();

	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		g_ulJZTotalWrite += (ulTimeEnd - ulTimeStart) / 6;
	#endif

	return ret;
}

int jz_nand_read_page (void *dev_id, int page, unsigned char *data,nand_page_info_t *info)
{
	int ret = 0;

	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
	#endif

	#ifdef JZ_DEBUG
		printf("%s() page: 0x%x\n", __FUNCTION__, page);
	#endif
	OP_NAND_LOCK();
	unsigned int startpage = page;
	page = SelectChip(startpage);

	ret = nand_read_page(dev_id, page, data,info);

	DeSelectChip(startpage);
	OP_NAND_UNLOCK();
	#if BACHMARK
	if (dev_id == 0)
	{
		ulTimeEnd = Get_PerformanceCounter();
		g_ulJZTotalRead += (ulTimeEnd - ulTimeStart) / 6;
	//printf("nandchipdma:g_ulJZTotalRead = %dms\n",g_ulJZTotalRead);
	}
	#endif
	return ret;
}

int jz_nand_write_page(void *dev_id, int page, unsigned char *data,nand_page_info_t *info)
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

	ret = nand_write_page(dev_id,page,data,info);

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


// must one block
int jz_nand_multiread(void *dev_id, unsigned int page,unsigned int pagecount,
					  unsigned char *mask,unsigned char *data,nand_statuspage_t *infopage)
{
	int ret = 0;
	unsigned char *ptr;
	unsigned int pageinblock;

	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
	#endif

	#ifdef JZ_DEBUG
	  printf("%s() page: 0x%x NAND_PAGE_MASK = 0x%x\n", __FUNCTION__, page,*mask);
	#endif

	unsigned int startpage = page;
	OP_NAND_LOCK();

	page = SelectChip(startpage);
	ptr = (unsigned char *) data;

	pageinblock = 0;
	while(pagecount--)
	{
		if(*mask == 0)
		{
			if ((infopage->page_status[pageinblock] & 1) == 0)
			{
				ret |= nand_multi_read_page(page, mask, ptr);
			}
		}

		ptr += g_dwbytesperpage;//g_dweccblocksize * g_dweccsteps;
		pageinblock++;
		page++;
		mask++;
	}

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

int jz_nand_write_page_info(void *dev_id, unsigned int page, nand_page_info_t *info)
{
	int ret;
	unsigned int startpage = page;

	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
	#endif

	OP_NAND_LOCK();
	page = SelectChip(startpage);

	ret = nand_write_page_info(page, info);

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

int jz_nand_erase_block(void *dev_id, unsigned int block)
{
	int ret;
	unsigned long addr = block * g_dwnandperblock;

	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
	#endif

	#ifdef JZ_DEBUG
		printf("erase block: %d !\n", block);
	#endif

	OP_NAND_LOCK();

	if(dev_id != NULL)
	{
		//convert sigle plane to erase
		addr = block * (g_dwnandperblock / g_dwplanenum);
	}

	unsigned int startpage = addr;
	addr = SelectChip(startpage);

	ret = nand_erase_block(dev_id,addr);

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

int jz_nand_multiwrite(	void				*dev_id,
						unsigned int		page,
						unsigned int		pagecount,
						unsigned char		*mask,
						unsigned char		*data,
						nand_statuspage_t	*infopage)
{
	int ret;

	#if BACHMARK
		ulTimeEnd = Get_PerformanceCounter();
		ulTimeStart = ulTimeEnd;
	#endif
	OP_NAND_LOCK();

	unsigned int startpage = page;
	page = SelectChip(startpage);
	if (g_dwplanenum == 2)
		ret = nand_multiwrite_p2(dev_id, page, pagecount, mask, data, infopage);
	else
		ret = nand_multiwrite_p1(dev_id, page, pagecount, mask, data, infopage);

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
