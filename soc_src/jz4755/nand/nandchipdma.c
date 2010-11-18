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
 *  Create:   2008-09-24, by xyzhangs
 *
 *  Maintain :lvjing
 *
 *******************************************************************************
 */
#include <os_api.h>
#include <header.h>
#include <jz_nand.h>
#include <nand_cfg.h>
#include <dm.h>

//#define JZ_DEBUG
//#undef printf

#ifndef JZ_DEBUG

	#define printfBCH()
	#define printfDMA(x)
	#define printfDMADescriptor(x)

#endif


extern unsigned int g_dwTSZ;
extern unsigned int g_dwUintSize;
extern unsigned int g_dwchipnum;
extern unsigned int g_dwplanenum;
extern unsigned int g_dwplanemask;
extern unsigned int g_dwnandperblock;
extern unsigned int g_dweccsteps;
extern unsigned int g_dwbytesperpage;
extern unsigned int g_dwoobsize;
extern unsigned int g_dweccblocksize;

//
//	NAND OP
//
extern unsigned int NANDFLASH_BASE;

/* global mutex between yaffs2 and ssfdc */

#define DMA_INTERRUPT_ENABLE 1

#define PHYSADDR(x) 	((x) & 0x1fffffff)

#if DMA_INTERRUPT_ENABLE
	static OS_EVENT * dma_sem_nand = NULL;
	static OS_EVENT * dma_sem_bch = NULL;
#endif

#define NAND_INTERRUPT 1
#if NAND_INTERRUPT
	static OS_EVENT * rb_sem = NULL;

	#define NAND_CLEAR_RB() (OUTREG32(A_GPIO_PXFLGC(RB_GPIO_GROUP), RB_GPIO_BIT_FLG))

	#define nand_wait_ready()                     \
	do{                                           \
	    unsigned int timeout = (1000 * 1000 / OS_TICKS_PER_SEC);             \
		while((!(INREG32(A_GPIO_PXFLG(RB_GPIO_GROUP)) & RB_GPIO_BIT_FLG)) && timeout--);\
	    OUTREG32(A_GPIO_PXFLGC(RB_GPIO_GROUP),RB_GPIO_BIT_FLG); \
	}while(0)

	#define READDATA_TIMEOUT	(100 * 1000 / OS_TICKS_PER_SEC)
	#define WRITEDATA_TIMEOUT	(800 * 1000 / OS_TICKS_PER_SEC)
	#define PAGE_TIMEOUT		(100 * 1000 / OS_TICKS_PER_SEC)
/*
	#define CLEAR_RB()						\
	do {									\
		NAND_CLEAR_RB();					\
		OUTREG32(A_GPIO_PXIMC(RB_GPIO_GROUP), RB_GPIO_BIT_FLG);	\
	} while (0)
*/
	#define CLEAR_RB()						\
	do {									\
		unsigned char err;					\
		NAND_CLEAR_RB();					\
		OUTREG32(A_GPIO_PXIMC(RB_GPIO_GROUP), RB_GPIO_BIT_FLG);	\
		os_SemaphoreSet(rb_sem,0,&err); \
	} while (0)

	#define WAIT_RB()						\
	do {									\
		unsigned char err;					\
		os_SemaphorePend(rb_sem,PAGE_TIMEOUT,&err);\
	} while (0)

#else
	#define CLEAR_RB()
	static inline void nand_wait_ready(void)
	{
	  	unsigned int timeout = 100;
	  	while((INREG32(A_GPIO_PXPIN(RB_GPIO_GROUP)) & RB_GPIO_BIT_FLG) && timeout--);
	    while (!(INREG32(A_GPIO_PXPIN(RB_GPIO_GROUP)) & RB_GPIO_BIT_FLG));
	}
	#define WAIT_RB()	nand_wait_ready()
#endif

#define NAND_RESET() while(1)

//-----------------------------------------------------------------------------
//
//	Define uncache buffers
//
//-----------------------------------------------------------------------------

static unsigned char	ucBuff[4096] __attribute__ ((aligned(4096)));
static PDMA_Descriptor	pnandWdesc[16];
static PDMA_Descriptor	pnandRdesc[16];
static PDMA_Descriptor	pbchDecdesc[32];
static PDMA_Descriptor	pbchEncdesc[32];
static PDmaBchDeTarget	pstBchDeTarget[16];

static unsigned int	*pdwZero;
static unsigned int	*pdwNand[2];
static unsigned char *pucCMD[16];
static unsigned char *pucAddr[16];
static nand_page_info_t *info;
static nand_page_info_t *info1;
#define UNCACHE(x) ((unsigned int)x | 0xa0000000)

//-----------------------------------------------------------------------------
//
//	Functions
//
//-----------------------------------------------------------------------------

#ifdef JZ_DEBUG
static void printfBCH(void)
{
	printf("A_BCH_BHCR = 0x%x\r\n",INREG32(A_BCH_BHCR));
	printf("A_BCH_BHCNT = 0x%x\r\n",INREG32(A_BCH_BHCNT));
	printf("A_BCH_BHINTS = 0x%x\r\n",INREG32(A_BCH_BHINTS));
	printf("A_BCH_BHINTE = 0x%x\r\n",INREG32(A_BCH_BHINTE));
}
static void printfDMADescriptor(PDMA_Descriptor descriptor)
{
	printf("descriptor addr = 0x%x\n",descriptor);
	printf("dwDES[0] = 0x%x\r\n",descriptor->dwDES[0]);
	printf("dwDES[1] = 0x%x\r\n",descriptor->dwDES[1]);
	printf("dwDES[2] = 0x%x\r\n",descriptor->dwDES[2]);
	printf("dwDES[3] = 0x%x\r\n",descriptor->dwDES[3]);
	printf("dwDES[4] = 0x%x\r\n",descriptor->dwDES[4]);
	printf("dwDES[5] = 0x%x\r\n",descriptor->dwDES[5]);

}
static void printfDMA(int ch)
{
	printf("ch = %d \r\n",ch);
	printf("A_DMA_DSA = 0x%x\r\n",INREG32(A_DMA_DSA(ch)));
	printf("A_DMA_DTA = 0x%x\r\n",INREG32(A_DMA_DTA(ch)));
	printf("A_DMA_DTC = 0x%x\r\n",INREG32(A_DMA_DTC(ch)));
	printf("A_DMA_DRT = 0x%x\r\n",INREG32(A_DMA_DRT(ch)));
	printf("A_DMA_DCS = 0x%x\r\n",INREG32(A_DMA_DCS(ch)));
	printf("A_DMA_DCM = 0x%x\r\n",INREG32(A_DMA_DCM(ch)));
	printf("A_DMA_DDA = 0x%x\r\n",INREG32(A_DMA_DDA(ch)));
	printf("A_DMA_DSD = 0x%x\r\n",INREG32(A_DMA_DSD(ch)));
	printf("A_DMA_DMAC = 0x%x\r\n",INREG32(A_DMA_DMAC(ch / 6)));
	printf("A_DMA_DIRQP = 0x%x\r\n",INREG32(A_DMA_DIRQP(ch / 6)));
	printf("A_DMA_DDR = 0x%x\r\n",INREG32(A_DMA_DDR(ch / 6)));
	printf("A_DMA_DCKE = 0x%x\r\n",INREG32(A_DMA_DCKE(ch / 6)));
}
#endif

static void NandInitBuff(void)
{
	unsigned char	*puctemp;
	unsigned int	i = 0;
	unsigned char	*pucUnCache;
	
	puctemp = ucBuff;

	//--------Descriptor init------------------

	pnandRdesc[0] = (PDMA_Descriptor)puctemp;
	pnandRdesc[0]->dwDES[4] = 0;
	//printf("pnandRdesc = 0x%x\n",pnandRdesc[0]);
	puctemp += sizeof(DMA_Descriptor);

	for(i = 1;i < 16;i++)
	{
		pnandRdesc[i] = (PDMA_Descriptor)puctemp;
		pnandRdesc[i - 1]->dwDES[3] = ((((unsigned long)pnandRdesc[i] & 0xFF0) >> 4) << 24);
		pnandRdesc[i]->dwDES[4] = 0;
		//printf("pnandRdesc = 0x%x\n",pnandRdesc[i]);
		puctemp += sizeof(DMA_Descriptor);
	}

	pbchDecdesc[0] = (PDMA_Descriptor)puctemp;
	pbchDecdesc[0]->dwDES[4] = 0;
	//printf("pbchDecdesc[0] = 0x%x\n",pbchDecdesc[0]);
	puctemp += sizeof(DMA_Descriptor);
	for(i = 1;i < 32;i++)
	{
		pbchDecdesc[i] = (PDMA_Descriptor)puctemp;
		pbchDecdesc[i-1]->dwDES[3] = ((((unsigned long)pbchDecdesc[i] & 0xFF0) >> 4) << 24);
		pbchDecdesc[i]->dwDES[4] = 0;
		//printf("pbchDecdesc[%d] = 0x%x,dwDES[3]=0x%x\n",i,pbchDecdesc[i],pbchDecdesc[i-1]->dwDES[3]);
		puctemp += sizeof(DMA_Descriptor);
	}

	pbchEncdesc[0] = (PDMA_Descriptor)puctemp;
	pbchEncdesc[0]->dwDES[4] = 0;
	puctemp += sizeof(DMA_Descriptor);
	for(i = 1;i < 32;i++)
	{
		pbchEncdesc[i] = (PDMA_Descriptor)puctemp;
		puctemp += sizeof(DMA_Descriptor);
		pbchEncdesc[i-1]->dwDES[3] = ((((unsigned long)pbchEncdesc[i] & 0xFF0) >> 4) << 24);
		pbchEncdesc[i]->dwDES[4] = 0;
		//printf("pbchEncdesc[%d] = 0x%x,dwDES[3]=0x%x\n",i,pbchEncdesc[i],pbchEncdesc[i-1]->dwDES[3]);
	}

	pnandWdesc[0] = (PDMA_Descriptor)puctemp;
	pnandWdesc[0]->dwDES[4] = 0;
	puctemp += sizeof(DMA_Descriptor);
	for(i = 1;i < 16;i++)
	{
		pnandWdesc[i] = (PDMA_Descriptor)puctemp;
		puctemp += sizeof(DMA_Descriptor);
		pnandWdesc[i-1]->dwDES[3] = ((((unsigned long)pnandWdesc[i] & 0xFF0) >> 4) << 24);
		pnandWdesc[i]->dwDES[4] = 0;
	}

	//--------------------------------------------
	__dcache_writeback_all();
	
	pucUnCache = (unsigned char *)UNCACHE(puctemp);
	puctemp = pucUnCache;

	info = (nand_page_info_t *)puctemp;
	puctemp += sizeof(nand_page_info_t);
	
	info1 = (nand_page_info_t *)puctemp;
	puctemp += sizeof(nand_page_info_t);
	
	pdwZero = (unsigned int *)puctemp;
	*pdwZero = 0;
	puctemp += sizeof(unsigned int);

	for(i = 0;i < 2;i++)
	{
		pdwNand[i] = (unsigned int *)puctemp;
		puctemp += sizeof(unsigned int);
	}

	for(i = 0;i < 16;i++)
	{
		pstBchDeTarget[i] = (PDmaBchDeTarget)puctemp;
		puctemp += sizeof(DmaBchDeTarget);
	}

	for(i = 0;i < 16;i++)
	{
		pucCMD[i] = (unsigned char *)puctemp;
		puctemp += sizeof(unsigned char);
	}
	for(i = 0;i < 16;i++)
	{
		pucAddr[i] = (unsigned char *)puctemp;
		puctemp += sizeof(unsigned char);
	}
	printf("ucBuff=0x%x, puctemp=0x%x", ucBuff, puctemp);
	printf("All of the buffer size = 0x%x\n",(puctemp - ucBuff));
}

static void DmaInitUseDesNand(void)
{
	int i;
	int ch = NAND_DMA_CHANNEL;
	OUTREG32(A_DMA_DCKE(ch / 6),(1 << (ch % 6)));//Open channel clock
	CLRREG32(A_DMA_DMAC(ch / 6), (DMAC_HALT | DMAC_ADDR_ERR));//Ensure DMAC.AR = 0,DMAC.HLT = 0

	CLRREG32(A_DMA_DCS(ch), DCS_AR | DCS_HLT | DCS_TT | DCS_INV); // Ensure DCSn.AR = 0, DCSn.HLT = 0, DCSn.TT = 0, DCSn.INV = 0
	OUTREG32(A_DMA_DTC(ch), 0);//DTCn = 0
	CLRREG32(A_DMA_DCS(ch), DCS_NDES);
	SETREG32(A_DMA_DCS(ch), DCS_DES8);

}

static void DmaInitUseDesBch(void)
{
	int i;
	int ch = BCH_DMA_CHANNEL;
	OUTREG32(A_DMA_DCKE(ch / 6),(1 << (ch % 6)));//Open channel clock
	CLRREG32(A_DMA_DMAC(ch / 6), (DMAC_HALT | DMAC_ADDR_ERR));//Ensure DMAC.AR = 0,DMAC.HLT = 0

	CLRREG32(A_DMA_DCS(ch), DCS_AR | DCS_HLT | DCS_TT | DCS_INV); // Ensure DCSn.AR = 0, DCSn.HLT = 0, DCSn.TT = 0, DCSn.INV = 0
	OUTREG32(A_DMA_DTC(ch), 0);//DTCn = 0
	CLRREG32(A_DMA_DCS(ch), DCS_NDES);
	SETREG32(A_DMA_DCS(ch), DCS_DES8);
}


static int SetZero(void *target,unsigned int len)
{
	unsigned int dwtemp;
	unsigned char err;
	int ch = NAND_DMA_CHANNEL;
	#ifdef JZ_DEBUG
		printf("setzero:target = 0x%x,len = %d\n",target,len);
	#endif
 	*pdwZero = 0;
	if(((unsigned int)target < 0xa0000000) && len)
		dma_cache_wback_inv((unsigned int)target, len);

	CLRREG32(A_DMA_DCS(ch), DCS_CTE);
	OUTREG32(A_DMA_DSA(ch), PHYSADDR((unsigned long)pdwZero));
	OUTREG32(A_DMA_DTA(ch), PHYSADDR((unsigned long)target));
	OUTREG32(A_DMA_DTC(ch), NAND_DMA_TC(len));
	OUTREG32(A_DMA_DRT(ch), DRT_AUTO);
	#if DMA_INTERRUPT_ENABLE
		OUTREG32(A_DMA_DCM(ch), (DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT
									| NAND_DMA_TSZ | DCM_TRANS_INTR_EN));
	#else
		OUTREG32(A_DMA_DCM(ch), (DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT| NAND_DMA_TSZ));
	#endif
	CLRREG32(A_DMA_DCS(ch), DCS_TT);
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE | DCS_NDES);

	#if DMA_INTERRUPT_ENABLE
		os_SemaphorePend(dma_sem_nand, 500, &err);
		if(err)
		{
			//printf("SetZero timeout!!\n");
			printfDMA(ch);
			CLRREG32(A_DMA_DMAC(NAND_DMA_CHANNEL),(DMAC_INT_EN));
			CLRREG32(A_DMA_DCS(NAND_DMA_CHANNEL),(DCS_CTE | DCS_TT | DCS_CT));
			return err;
		}
		return 0;
	#else
		while (!(INREG32(A_DMA_DCS(ch)) & DCS_TT));
		return 0;
	#endif
}

static int SetFF(void *target,unsigned int len)
{
	unsigned int dwtemp;
	unsigned char err;
	int ch = NAND_DMA_CHANNEL;
	#ifdef JZ_DEBUG
		printf("setzero:target = 0x%x,len = %d\n",target,len);
	#endif
 	*pdwZero = 0xFFFFFFFF;
	if(((unsigned int)target < 0xa0000000) && len)
		dma_cache_wback_inv((unsigned int)target, len);

	CLRREG32(A_DMA_DCS(ch), DCS_CTE);
	OUTREG32(A_DMA_DSA(ch), PHYSADDR((unsigned long)pdwZero));
	OUTREG32(A_DMA_DTA(ch), PHYSADDR((unsigned long)target));
	OUTREG32(A_DMA_DTC(ch), NAND_DMA_TC(len));
	OUTREG32(A_DMA_DRT(ch), DRT_AUTO);
	#if DMA_INTERRUPT_ENABLE
		OUTREG32(A_DMA_DCM(ch), (DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT
									| NAND_DMA_TSZ | DCM_TRANS_INTR_EN));
	#else
		OUTREG32(A_DMA_DCM(ch), (DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT| NAND_DMA_TSZ));
	#endif
	CLRREG32(A_DMA_DCS(ch), DCS_TT);
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE | DCS_NDES);

	#if DMA_INTERRUPT_ENABLE
		os_SemaphorePend(dma_sem_nand, 500, &err);
		if(err)
		{
			//printf("SetZero timeout!!\n");
			printfDMA(ch);
			CLRREG32(A_DMA_DMAC(NAND_DMA_CHANNEL),(DMAC_INT_EN));
			CLRREG32(A_DMA_DCS(NAND_DMA_CHANNEL),(DCS_CTE | DCS_TT | DCS_CT));
			return err;
		}
		return 0;
	#else
		while (!(INREG32(A_DMA_DCS(ch)) & DCS_TT));
		return 0;
	#endif
}

static inline void NandWriteDesSet(unsigned int	dwDesIndex,void *target,void *source,unsigned int len)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	#ifdef JZ_DEBUG
		printf("%s():target = 0x%08x source = 0x%08x\r\n", __FUNCTION__,target,source);
	#endif

	if(((unsigned int)source < 0xa0000000) && len)
		dma_cache_wback_inv(((unsigned long)source), len);

	if(((unsigned int)target < 0xa0000000) && len)
		dma_cache_wback_inv(((unsigned long)target), len);

	pnandWdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)source);
	pnandWdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)target);
	pnandWdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pnandWdesc[dwDesIndex]->dwDES[3] |= len / g_dwUintSize;
	pnandWdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;

	pnandWdesc[dwDesIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_32BIT | DES_DP_32BIT | DES_BLAST
									| g_dwTSZ | DES_LINK_EN;

}

static inline void NandWriteOObDesSet(unsigned int	dwDesIndex,void *target,void *source,unsigned int len)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;
	
	#ifdef JZ_DEBUG
		printf("%s():target = 0x%08x source = 0x%08x\r\n", __FUNCTION__,target,source);
	#endif
	
	if(((unsigned int)source < 0xa0000000) && len)
		dma_cache_wback_inv(((unsigned long)source), len);
			
	if(((unsigned int)target < 0xa0000000) && len)
		dma_cache_wback_inv(((unsigned long)target), len);
	
	pnandWdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)source);
	pnandWdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)target);
	pnandWdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pnandWdesc[dwDesIndex]->dwDES[3] |= len / 4;
	pnandWdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;
	
	pnandWdesc[dwDesIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_32BIT | DES_DP_32BIT | DES_BLAST
									| DES_TSZ_32BIT | DES_LINK_EN;	
									
}

static inline void NandWriteMemcpyEnable(void)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	dwtemp = PHYSADDR((unsigned long)pnandWdesc[0]);
	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_DMA_DRT(ch), DRT_AUTO);
	CLRREG32(A_DMA_DCS(ch), DCS_NDES);
	SETREG32(A_DMA_DCS(ch), DCS_DES8);
	CLRREG32(A_DMA_DCS(ch),(DCS_TT | DCS_CT));
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE);
}

static inline unsigned char  NandWriteMemcpyEnableTEST(void)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	dwtemp = PHYSADDR((unsigned long)pnandWdesc[0]);
	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_DMA_DRT(ch), DRT_AUTO);
	CLRREG32(A_DMA_DCS(ch), DCS_NDES);
	SETREG32(A_DMA_DCS(ch), DCS_DES8);
	CLRREG32(A_DMA_DCS(ch),(DCS_TT | DCS_CT));
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE);

	#if DMA_INTERRUPT_ENABLE
		os_SemaphorePend(dma_sem_nand, 500, &err);
		CLRREG32(A_DMA_DCS(ch),(DCS_CTE | DCS_TT | DCS_CT));
		if(err)
		{
			printf("Bch encode and write: bch timeout!!\n");
			printfDMA(NAND_DMA_CHANNEL);
			CLRREG32(A_DMA_DMAC(NAND_DMA_CHANNEL),(DMAC_INT_EN));
			CLRREG32(A_DMA_DCS(NAND_DMA_CHANNEL),(DCS_CTE | DCS_TT | DCS_CT));
			return err ;
		}
	#else
		while (!(INREG32(A_DMA_DCS(NAND_DMA_CHANNEL)) & DCS_TT));
	#endif
	return 0;
}

static inline void NandReadDesSet(unsigned int	dwDesIndex,void *target,void *source,unsigned int len)
{
	#ifdef JZ_DEBUG
		printf("%s():target = 0x%08x source = 0x%08x\r\n", __FUNCTION__,target,source);
	#endif

	if(((unsigned int)source < 0xa0000000) && len)
		dma_cache_wback_inv(source, len);

	if(((unsigned int)target < 0xa0000000) && len)
		dma_cache_wback_inv(target, len);

	pnandRdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)source);
	pnandRdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)target);
	pnandRdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pnandRdesc[dwDesIndex]->dwDES[3] |= len / g_dwUintSize;
	pnandRdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;

	//set DES_SP_32BIT for EMC has 32bit fifo
	pnandRdesc[dwDesIndex]->dwDES[0] = DES_DAI | DES_SAI | DES_SP_32BIT | DES_DP_32BIT
									| g_dwTSZ | DES_LINK_EN | DES_BLAST;
}


static inline void NandWirteAddrDesSet(unsigned int dwDesIndex,void *target,void *source,unsigned int len)
{
	#ifdef JZ_DEBUG
		printf("%s():target = 0x%08x source = 0x%08x,sourcedata = 0x%x\r\n", __FUNCTION__,target,source,*(unsigned char *)source);
	#endif

	pnandWdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)source);
	pnandWdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)target);
	pnandWdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pnandWdesc[dwDesIndex]->dwDES[3] |= len;
	pnandWdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;

	pnandWdesc[dwDesIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_08BIT | DES_DP_08BIT
								| DES_TSZ_08BIT | DES_BLAST | DES_LINK_EN;

}


static inline void NandBchDmaDesSetEncode(unsigned int dwDescrIndex,void *target,void *Datasource)
{
	#ifdef JZ_DEBUG
		printf("%s():dwDescrIndex = %d,target = 0x%08x ,source = 0x%08x\r\n", __FUNCTION__,dwDescrIndex,target,Datasource);
	#endif

	if((unsigned int)Datasource < 0xa0000000)
		dma_cache_wback_inv((unsigned long)Datasource, g_dweccblocksize);

	if((unsigned int)target < 0xa0000000)
		dma_cache_wback_inv((unsigned long)target, PAR_SIZE);

	pbchEncdesc[dwDescrIndex]->dwDES[1] = PHYSADDR((unsigned long)Datasource);
	pbchEncdesc[dwDescrIndex]->dwDES[2] = PHYSADDR((unsigned long)target);
	pbchEncdesc[dwDescrIndex]->dwDES[3] &= 0xFF000000;
	pbchEncdesc[dwDescrIndex]->dwDES[3] |= g_dweccblocksize / 16;
	pbchEncdesc[dwDescrIndex]->dwDES[5] = DRT_BCH_EN;

	pbchEncdesc[dwDescrIndex]->dwDES[0] = DES_SAI | DES_DAI |DES_SP_32BIT | DES_BLAST
										| DES_DP_08BIT | DES_TSZ_16BYTE | DES_LINK_EN ;
}


static inline unsigned char NandBchDmaEncodeEnable(void)
{
	int ch = BCH_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	dwtemp = PHYSADDR((unsigned long)pbchEncdesc[0]);
	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_DMA_DRT(ch), DRT_BCH_EN);
	OUTREG32(A_BCH_BHCSR, (BCH_BDMA));
	CLRREG32(A_DMA_DCS(ch),(DCS_TT | DCS_CT));
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE);

	#if DMA_INTERRUPT_ENABLE
		os_SemaphorePend(dma_sem_bch, 500, &err);
		if(err)
		{
		//	printf("Bch encode timeout!!\n");
			printfDMA(BCH_DMA_CHANNEL);
			CLRREG32(A_DMA_DMAC(BCH_DMA_CHANNEL) , (DMAC_INT_EN));
			CLRREG32(A_DMA_DCS(BCH_DMA_CHANNEL) , (DCS_TT |DCS_CT |DCS_CTE));
			return err;

		}
	#else
		while (!(INREG32(A_DMA_DCS(ch)) & DCS_TT));
	#endif
	return 0;
}


static inline void NandBchDmaDecodePreStart(unsigned int dwDesIndex)
{
	int ch = BCH_DMA_CHANNEL;
	unsigned int dwtemp;

	*pdwNand[0] = (1 << (ch % 6));
	//printf("A_DMA_DDRS = 0x%x\n",*pdwNand[0]);
	pnandRdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)pdwNand[0]);
	pnandRdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)A_DMA_DDRS(ch/6));
	pnandRdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pnandRdesc[dwDesIndex]->dwDES[3] |= 1;
	pnandRdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;

	pnandRdesc[dwDesIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_32BIT | DES_DP_32BIT
								| DES_TSZ_32BIT | DES_BLAST | DES_LINK_EN;
	dwDesIndex++;

	*pdwNand[1] = (DCS_DES8 | DCS_CTE);
	//printf("A_DMA_DCS = 0x%x\n",*pdwNand[1]);
	pnandRdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)pdwNand[1]);
	pnandRdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)A_DMA_DCS(ch));
	pnandRdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pnandRdesc[dwDesIndex]->dwDES[3] |= 1;
	pnandRdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;

	pnandRdesc[dwDesIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_32BIT | DES_DP_32BIT
								| DES_TSZ_32BIT | DES_BLAST;

	dwtemp = PHYSADDR((unsigned long)pbchDecdesc[0]);

	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_BCH_BHCSR, (BCH_BDMA));
	OUTREG32(A_DMA_DRT(ch), DRT_BCH_DE);

}

static inline void NandBchDmaEncodePreStart(unsigned int dwDesIndex)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned int dwtemp;

	*pdwNand[0] = (1 << (ch % 6));
	//printf("A_DMA_DDRS = 0x%x\n",*pdwNand[0]);
	pbchEncdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)pdwNand[0]);
	pbchEncdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)A_DMA_DDRS(ch/6));
	pbchEncdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pbchEncdesc[dwDesIndex]->dwDES[3] |= 1;
	pbchEncdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;

	pbchEncdesc[dwDesIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_32BIT | DES_DP_32BIT
								| DES_TSZ_32BIT | DES_BLAST | DES_LINK_EN;
	dwDesIndex++;

	*pdwNand[1] = (DCS_DES8 | DCS_CTE);
	//printf("A_DMA_DCS = 0x%x\n",*pdwNand[1]);
	pbchEncdesc[dwDesIndex]->dwDES[1] = PHYSADDR((unsigned long)pdwNand[1]);
	pbchEncdesc[dwDesIndex]->dwDES[2] = PHYSADDR((unsigned long)A_DMA_DCS(ch));
	pbchEncdesc[dwDesIndex]->dwDES[3] &= 0xFF000000;
	pbchEncdesc[dwDesIndex]->dwDES[3] |= 1;
	pbchEncdesc[dwDesIndex]->dwDES[5] = DRT_AUTO;

	pbchEncdesc[dwDesIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_32BIT | DES_DP_32BIT
								| DES_TSZ_32BIT | DES_BLAST;

	dwtemp = PHYSADDR((unsigned long)pnandWdesc[0]);
	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_DMA_DRT(ch), DRT_AUTO);

}

static inline unsigned char NandWriteMemcpyAndEncode(void)
{
	int ch = BCH_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	dwtemp = PHYSADDR((unsigned long)pbchEncdesc[0]);

	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_BCH_BHCSR, (BCH_BDMA));
	OUTREG32(A_DMA_DRT(ch), DRT_BCH_EN);
	CLRREG32(A_DMA_DCS(ch),(DCS_TT | DCS_CT));
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE);
		//BCH interrupt judge
	#if DMA_INTERRUPT_ENABLE
		os_SemaphorePend(dma_sem_nand, 500, &err);
		CLRREG32(A_DMA_DCS(ch),(DCS_CTE | DCS_TT | DCS_CT));
		if(err)
		{
			printf("Bch encode and write: bch timeout!!\n");
			printfDMA(NAND_DMA_CHANNEL);
			CLRREG32(A_DMA_DMAC(NAND_DMA_CHANNEL),(DMAC_INT_EN));
			CLRREG32(A_DMA_DCS(NAND_DMA_CHANNEL),(DCS_CTE | DCS_TT | DCS_CT));
			return err ;
		}
	#else

		while (!(INREG32(A_DMA_DCS(NAND_DMA_CHANNEL)) & DCS_TT));
	#endif
	return 0;
}

static inline unsigned char NandReadMemcpyAndDecode(void)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	dwtemp = PHYSADDR((unsigned long)pnandRdesc[0]);
	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_DMA_DRT(ch), DRT_AUTO);
	CLRREG32(A_DMA_DCS(ch), DCS_NDES);
	SETREG32(A_DMA_DCS(ch), DCS_DES8);
	CLRREG32(A_DMA_DCS(ch),(DCS_TT | DCS_CT));
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE);

	#if DMA_INTERRUPT_ENABLE
		os_SemaphorePend(dma_sem_bch, 500, &err);
		CLRREG32(A_DMA_DCS(ch),(DCS_CTE | DCS_TT | DCS_CT));
		if(err)
		{
			printf("Read nand and decode timeout!!\n");
			printfDMA(BCH_DMA_CHANNEL);
			CLRREG32(A_DMA_DMAC(BCH_DMA_CHANNEL),(DMAC_INT_EN));
	 		CLRREG32(A_DMA_DCS(BCH_DMA_CHANNEL) , (DCS_TT | DCS_CT |DCS_CTE));
			return err;
		}
	#else
		while (!(INREG32(A_DMA_DCS(BCH_DMA_CHANNEL)) & DCS_TT));
	#endif

	return 0;
}

static inline void NandBchDmaDesSetDecode(unsigned int dwDescrIndex,void *target,void *Datasource,void *Parsource)
{
	#ifdef JZ_DEBUG
		printf("%s():dwDescrIndex = %d target = 0x%08x source = 0x%08x Parsource = 0x%08x\r\n",
				__FUNCTION__,dwDescrIndex,target,Datasource,Parsource);
	#endif

	PDMA_Descriptor pNextDescriptor;

	if((unsigned int)Datasource < 0xa0000000)
		dma_cache_wback_inv((unsigned long)Datasource, g_dweccblocksize);

	if((unsigned int)Parsource < 0xa0000000)
		dma_cache_wback_inv((unsigned long)Parsource, PAR_SIZE);

	if((unsigned int)target < 0xa0000000)
		dma_cache_wback_inv((unsigned long)target, 20);

	pNextDescriptor = pbchDecdesc[dwDescrIndex + 1];

	pbchDecdesc[dwDescrIndex]->dwDES[1] = PHYSADDR((unsigned long)Datasource);
	pbchDecdesc[dwDescrIndex]->dwDES[2] = PHYSADDR((unsigned long)target);
	pbchDecdesc[dwDescrIndex]->dwDES[3] &= 0xFF000000;
	pbchDecdesc[dwDescrIndex]->dwDES[3] |= g_dweccblocksize / 16;
	pbchDecdesc[dwDescrIndex]->dwDES[5] = DRT_BCH_DE;

	pNextDescriptor->dwDES[1] = PHYSADDR((unsigned long)Parsource);
	pNextDescriptor->dwDES[2] = PHYSADDR((unsigned long)target);
	pNextDescriptor->dwDES[3] &= 0xFF000000;
	pNextDescriptor->dwDES[3] |= PAR_SIZE;
	pNextDescriptor->dwDES[5] = DRT_BCH_DE;

	pbchDecdesc[dwDescrIndex]->dwDES[0] = DES_SAI | DES_DAI |DES_SP_32BIT
										| DES_DP_32BIT | DES_TSZ_16BYTE | DES_LINK_EN;

	pNextDescriptor->dwDES[0] = DES_SAI | DES_DAI |DES_SP_08BIT | DES_DP_32BIT | DES_BLAST
										| DES_TSZ_08BIT | DES_LINK_EN;

}

static inline int NandSendReadAddr(unsigned int pageaddr,unsigned int offset)
{
	unsigned char err;

	REG_NAND_CMD = NAND_CMD_READ1_00;

	REG_NAND_ADDR = (unsigned char)((offset & 0x000000FF) >> 0);
	REG_NAND_ADDR = (unsigned char)((offset & 0x0000FF00) >> 8);

	REG_NAND_ADDR = (unsigned char)((pageaddr & 0x000000FF) >> 0);
	REG_NAND_ADDR = (unsigned char)((pageaddr & 0x0000FF00) >> 8);
	if(CONFIG_SSFDC_NAND_ROW_CYCLE >= 3 )
		REG_NAND_ADDR = (unsigned char)((pageaddr & 0x00FF0000) >> 16);

	CLEAR_RB();

	REG_NAND_CMD = NAND_CMD_READSTART;

	//Wait R/B
	#if NAND_INTERRUPT
		os_SemaphorePend(rb_sem,READDATA_TIMEOUT,&err);
		if(err)
		{
			printf("timeout\n");
			printf("pageaddr=%d, offset=%d\n", pageaddr, offset);

			return -1;
		}
	#else
		nand_wait_ready();
	#endif

	return 0;
}

static void inline NandSendReadCacheAddr(unsigned short offset)
{
	REG_NAND_CMD = NAND_CMD_RANDOM_READ;
	REG_NAND_ADDR = (unsigned char)((offset & 0x000000FF) >> 0);
	REG_NAND_ADDR = (unsigned char)((offset & 0x0000FF00) >> 8);
	REG_NAND_CMD = NAND_CMD_RANDOM_READ_CONFIRM;
}

static inline void NandSendWriteCacheAddr(unsigned int offset)
{
	REG_NAND_CMD = NAND_CMD_PAGE_RANDOM_PROGRAM_START;
	REG_NAND_ADDR = (unsigned char)((offset >> 0) & 0x0FF);
	REG_NAND_ADDR = (unsigned char)((offset >> 8) & 0x0FF);
}

int nand_read_oob(int page,nand_page_info_t *pinfo)
{
 	unsigned char *ptr;
 	unsigned int i;
	if(NandSendReadAddr(page,g_dwbytesperpage))
		return -1;
	ptr = (unsigned char *)pinfo;

	for ( i = 0; i < g_dwoobsize; i++)
	{
		unsigned char d;
		d = NAND_IO_ADDR;
		*ptr++ = d;
	}
	//while(1);
	return 0;
}

static inline int BchCorrect(unsigned char *dat, int idx)
{
	int i, bit;  // the 'bit' of i byte is error
	//printf("error bit index=%d\n",idx);
	idx--;
	if (idx < (g_dweccblocksize * 8))
	{
		i = idx / 8;
		bit = idx % 8;
		dat[i] ^= (1 << bit);
	}
	else
	{
		//printf("error bit index=%d\n",idx);
	}

	return 0;
}

static int NandBchCorrect(unsigned char *databuf,unsigned char steps)
{
	unsigned int stat,i,ret = 0,errcntflag = 0;;
	unsigned int *pBchBHERR;
	unsigned char *tmpbuf;
	for(i = 0;i < steps;i++)
	{
		stat = pstBchDeTarget[i]->dwBHINT;
		//printf("stat = 0x%x\n",stat);
		if (stat & BCH_INTS_ERR)
		{
			if (stat & BCH_INTS_UNCOR)
			{
				printf("!!!Uncorrectable error occurred\n");
				printf("stat = 0x%x\n", stat);
				printf("sector = 0x%x\n", i);
				return -EIO;
			}
			else
			{
				tmpbuf = databuf + i * g_dweccblocksize;
				pBchBHERR = (unsigned int *)(pstBchDeTarget[i]->dwBCHERR);

				unsigned int errcnt = (stat & BCH_INTS_ERRC_MASK) >> BCH_INTS_ERRC_BIT;

				//printf("stat = 0x%x error count = %d\n", stat,errcnt);
				//printf("error count = %d\n",errcnt);
				switch (errcnt)
				{
					case 8:
						ret = BchCorrect(tmpbuf, (pBchBHERR[3] & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
					case 7:
						ret = BchCorrect(tmpbuf, (pBchBHERR[3] & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
					case 6:
						ret = BchCorrect(tmpbuf, (pBchBHERR[2] & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
						//printf("bit err too many!error count = %d\n",errcnt);
						errcntflag = BITERR2MANY;
					case 5:
						ret = BchCorrect(tmpbuf, (pBchBHERR[2] & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
					case 4:
						ret = BchCorrect(tmpbuf, (pBchBHERR[1] & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
						//printf("error count = %d\n",errcnt);
					case 3:
						ret = BchCorrect(tmpbuf, (pBchBHERR[1] & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
					case 2:
						ret = BchCorrect(tmpbuf, (pBchBHERR[0] & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
					case 1:
						ret = BchCorrect(tmpbuf, (pBchBHERR[0] & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
						break;
					default:

						break;
				}
				if((unsigned int)tmpbuf < 0xa0000000)
					dma_cache_wback_inv(tmpbuf,g_dweccblocksize);
			}
		}

	}
	return (ret | errcntflag);
}

static void inline NandSendRandomReadAddr( unsigned short pageAddr)
{
	REG_NAND_CMD = 	NAND_CMD_RANDOM_READ;
	REG_NAND_ADDR = (unsigned char)((pageAddr & 0x000000FF) >> 0);
	REG_NAND_ADDR = (unsigned char)((pageAddr & 0x0000FF00) >> 8);
	REG_NAND_CMD =NAND_CMD_RANDOM_READ_CONFIRM;
}

static inline void NandSendWriteAddr(unsigned int pageaddr,unsigned int offset)
{
	REG_NAND_CMD = NAND_CMD_PAGE_PROGRAM_START;
	REG_NAND_ADDR = (unsigned char)((offset >> 0) & 0x0FF);
	REG_NAND_ADDR = (unsigned char)((offset >> 8) & 0x0FF);
	REG_NAND_ADDR = (unsigned char)((pageaddr >> 0) & 0x0FF);
	REG_NAND_ADDR = (unsigned char)((pageaddr >> 8) & 0x0FF);
	if(CONFIG_SSFDC_NAND_ROW_CYCLE >= 3 )
		REG_NAND_ADDR = (unsigned char)((pageaddr >> 16) & 0x0FF);
}

#if NAND_INTERRUPT
static void RBHandler(unsigned int arg)
{
	__gpio_mask_irq(arg);
	os_SemaphorePost(rb_sem);
}
#endif

#if DMA_INTERRUPT_ENABLE
static void DmaHandlerNand(unsigned int arg)
{
	CLRREG32(A_DMA_DMAC(arg),(DMAC_INT_EN));
	CLRREG32(A_DMA_DCS(arg),(DCS_CTE | DCS_TT | DCS_CT));

	if (INREG32(A_DMA_DCS(arg)) & DCS_AR)
	{
		printf("Nand dma address error interrupt: source = 0x%x,target = 0x%x!!\n",
			INREG32(A_DMA_DSA(arg)),INREG32(A_DMA_DTA(arg)));
	}
	else
	{
		os_SemaphorePost(dma_sem_nand);
	}

}
static void DmaHandlerBch(unsigned int arg)
{
	CLRREG32(A_DMA_DMAC(arg),(DMAC_INT_EN));
	CLRREG32(A_DMA_DCS(arg),(DCS_CTE | DCS_TT | DCS_CT));
	if (INREG32(A_DMA_DCS(arg)) & DCS_AR)
	{
		printf("Bch dma address error interrupt: source = 0x%x,target = 0x%x!!\n",
				INREG32(A_DMA_DSA(arg)),INREG32(A_DMA_DTA(arg)));
	}
	else
	{
		os_SemaphorePost(dma_sem_bch);
	}
}
#endif

static int InitInfo(nand_page_info_t *pinfo)
{
	return (SetFF((void *)pinfo,g_dwoobsize));
}

int nand_Intr_init(void)
{
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_DMAC);//__cpm_start_dmac();
	SETREG32(A_DMA_DMAC(0),DMAC_DMA_EN);

	NandInitBuff();

	DmaInitUseDesNand();
	DmaInitUseDesBch();

	printf("nand_intr_init 1\n");
	#if DMA_INTERRUPT_ENABLE
		if(dma_sem_nand == NULL)
			dma_sem_nand = os_SemaphoreCreate(0);
		request_irq(EIRQ_DMA_BASE + NAND_DMA_CHANNEL, DmaHandlerNand, NAND_DMA_CHANNEL);
		if(dma_sem_bch == NULL)
			dma_sem_bch = os_SemaphoreCreate(0);
		request_irq(EIRQ_DMA_BASE + BCH_DMA_CHANNEL, DmaHandlerBch,BCH_DMA_CHANNEL);
	#endif
	printf("nand_intr_init 2\n");
	#if NAND_INTERRUPT
		NAND_CLEAR_RB();
		gpio_intr_init(GITT_R_EDGE, RB_GPIO_PIN);
		__gpio_disable_pull(RB_GPIO_PIN);
		if(rb_sem == NULL)
			rb_sem = os_SemaphoreCreate(0);
		request_irq(EIRQ_GPIO_BASE + RB_GPIO_PIN, RBHandler,RB_GPIO_PIN);
		__gpio_mask_irq(RB_GPIO_PIN);
	#endif

	InitInfo(info);
	InitInfo(info1);
	
	//Disable bch interrupt
	OUTREG32(A_BCH_BHINTEC,0xff);
	OUTREG32(A_INTC_ICMSR,IRQ_BCH);

	printf("nand_intr_init 3\n");
	return 0;
}

int NandSendReset(void)
{
	unsigned int timeout = 100;

	REG_NAND_CMD = NAND_CMD_RESET;
	
	while((INREG32(A_GPIO_PXPIN(RB_GPIO_GROUP)) & RB_GPIO_BIT_FLG) && timeout--);
	while (!(INREG32(A_GPIO_PXPIN(RB_GPIO_GROUP)) & RB_GPIO_BIT_FLG));

	return 0;
}

static inline unsigned char NandDecode(void)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	dwtemp = PHYSADDR((unsigned long)pbchDecdesc[0]);
	OUTREG32(A_DMA_DDA(ch),(dwtemp));
	OUTREG32(A_DMA_DRT(ch), DRT_AUTO);
	CLRREG32(A_DMA_DCS(ch), DCS_NDES);
	SETREG32(A_DMA_DCS(ch), DCS_DES8);
	CLRREG32(A_DMA_DCS(ch),(DCS_TT | DCS_CT));
	OUTREG32(A_DMA_DDRS(ch / 6), (1 << (ch % 6)));
	SETREG32(A_DMA_DCS(ch), DCS_CTE);

	#if DMA_INTERRUPT_ENABLE
		os_SemaphorePend(dma_sem_bch, 500, &err);
		CLRREG32(A_DMA_DCS(ch),(DCS_CTE | DCS_TT | DCS_CT));
		if(err)
		{
			printf("Read nand and decode timeout!!\n");
			printfDMA(BCH_DMA_CHANNEL);
			CLRREG32(A_DMA_DMAC(BCH_DMA_CHANNEL),(DMAC_INT_EN));
	 		CLRREG32(A_DMA_DCS(BCH_DMA_CHANNEL) , (DCS_TT | DCS_CT |DCS_CTE));
			return err;
		}
	#else
		while (!(INREG32(A_DMA_DCS(BCH_DMA_CHANNEL)) & DCS_TT));
	#endif

	return 0;
}

int nand_read_statuspage(int page, unsigned char *data, nand_page_info_t *pinfo)
{
	int i,j = 0,ret = 0;
	unsigned char *ptr;
	unsigned char *par;
	unsigned int dwDescrIndex = 0,dwReadDesIndex = 0;
	nand_page_info_t *ptempinfo;
	#ifdef JZ_DEBUG
		printf("%s() page: 0x%x\n", __FUNCTION__, page);
	#endif

	ptr = (unsigned char *)data;
	if (pinfo)
		ptempinfo = pinfo;
	else
		ptempinfo = info;

	if(nand_read_oob(page, ptempinfo))
	{
		return -EIO;
	}

	par = (unsigned char *)&ptempinfo->ecc_field;

	if ((*par & *(par + 1)& *(par + 2)& *(par + 3)& *(par + 4)
		& *(par + 5)& *(par + 6)& *(par + 7)& *(par + 8)& *(par + 9)
		& *(par + 10)& *(par + 11)& *(par + 12)) == 0xFF)
	{
		//printf("%s() SetFF,page: 0x%x\n", __FUNCTION__, page);
		SetFF(data,512);
		return 0;
	}

	// Read block_addr_field and page_status
	NandSendRandomReadAddr(0);
	// Set Read parameter
	NandReadDesSet(dwReadDesIndex,ptr,(void *)&NAND_IO_ADDR,512);
	dwReadDesIndex++;

	// Set Decode parameter
	for(j = 0; j < (512 / g_dweccblocksize); j++)
	{
		NandBchDmaDesSetDecode(dwDescrIndex,(void *)pstBchDeTarget[j],ptr,par);
		dwDescrIndex += 2;
		ptr += g_dweccblocksize;
		par += PAR_SIZE;
	}

	dwDescrIndex--;
	NAND_BCH_DECODE( g_dweccblocksize + PAR_SIZE);

#if DMA_INTERRUPT_ENABLE
		pbchDecdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
		pbchDecdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
#else
		pbchDecdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
#endif

	NandBchDmaDecodePreStart(dwReadDesIndex);
	//printf("read_page:dwReadDesIndex = %d,dwDescrIndex = %d\n",dwReadDesIndex,dwDescrIndex);

	dma_cache_wback_inv((unsigned long)pbchDecdesc[0], ((dwDescrIndex + 1) << 5));
	dma_cache_wback_inv((unsigned long)pnandRdesc[0], ((dwReadDesIndex + 2) << 5));

	// Read and Decode
	if (NandReadMemcpyAndDecode())
		return -EIO;

	ret =  NandBchCorrect(data,(512 / g_dweccblocksize));

	if(ret == -EIO)
	{
		printf("%s page = 0x%x\n",__FUNCTION__,page);
	}
	
	return ret;
}

int nand_write_statuspage(int page, unsigned char *data)
{
	int i,j;
	unsigned char *ptemp;
	unsigned char *ptr, status;
 	unsigned int dwDescrIndex = 0;
 	unsigned int dwWDescrIndex = 0;
	nand_page_info_t *pinfo = info;
	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif

	ptr = (unsigned char *) data;
	unsigned char *par = (unsigned char *)&(pinfo->ecc_field);

	//--------------------------------------------------
	
	//printf("nand_write_statuspage:page = 0x%X\n", page);
	// Encoding for the first 512 bytes

	NandSendWriteAddr(page, 0);
	NandWriteDesSet(dwWDescrIndex, (void *)&NAND_IO_ADDR, data, 512);

	#if DMA_INTERRUPT_ENABLE
		pnandWdesc[dwWDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
		pnandWdesc[dwWDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
	#else
		pnandWdesc[dwWDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
	#endif

	dma_cache_wback_inv((unsigned long)pnandWdesc[0], 32);
	if ( NandWriteMemcpyEnableTEST())
	{
		return -EIO;
	}

	for(j = 0;j < (512 / g_dweccblocksize); j++)
	{
		NandBchDmaDesSetEncode(dwDescrIndex,par,ptr);
		dwDescrIndex ++;
		par += PAR_SIZE;
		ptr += g_dweccblocksize;
	}

	dwDescrIndex --;
	NAND_BCH_ENCODE(g_dweccblocksize);

#if DMA_INTERRUPT_ENABLE
		pbchEncdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
		pbchEncdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
#else
		pbchEncdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
#endif
	dma_cache_wback_inv((unsigned long)pbchEncdesc[0], ((dwDescrIndex + 1) << 5));

	if ( NandBchDmaEncodeEnable())
		goto write_error;

	NandSendWriteCacheAddr(g_dwbytesperpage);
	ptr = (unsigned char *)pinfo;

	for (i = 0; i < g_dwoobsize; i++)
	{
		NAND_IO_ADDR = *ptr++;
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

int nand_read_page (void *dev_id, int page, unsigned char *data,nand_page_info_t *pinfo)
{
	int i,j = 0,ret = 0;
	unsigned char *ptr;
	unsigned char *par;
	nand_page_info_t *ptempinfo;
	unsigned int dwDescrIndex = 0,dwReadDesIndex = 0;

	#ifdef JZ_DEBUG
		printf("%s() page: 0x%x\n", __FUNCTION__, page);
	#endif
	
	ptr = data;

	if (dev_id != 0)
		ptempinfo = pinfo;
	else
		ptempinfo = info;
	
	if (nand_read_oob(page, ptempinfo))
		return -EIO;

	par = (unsigned char *)&ptempinfo->ecc_field;

	if( (dev_id == 0) || ((dev_id !=0) && ((*par & *(par + 1)& *(par + 2)& *(par + 3)& *(par + 4)
			& *(par + 5)& *(par + 6)& *(par + 7)& *(par + 8)& *(par + 9)
			& *(par + 10)& *(par + 11)& *(par + 12)) != 0xff)))
	{
		NandSendRandomReadAddr(0);
		NandReadDesSet(dwReadDesIndex,ptr,(void *)&NAND_IO_ADDR,g_dwbytesperpage);
		dwReadDesIndex++;
		//printf("g_dweccblocksize = %d\n", g_dweccblocksize);
		for(j = 0; j < g_dweccsteps; j++)
		{
			NandBchDmaDesSetDecode(dwDescrIndex,(void *)pstBchDeTarget[j],ptr,par);
			dwDescrIndex+=2;
			ptr+=g_dweccblocksize;
			par+=PAR_SIZE;
		}
		dwDescrIndex--;
		NAND_BCH_DECODE( g_dweccblocksize + PAR_SIZE);

		#if DMA_INTERRUPT_ENABLE
			pbchDecdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
			pbchDecdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
		#else
			pbchDecdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
		#endif

		NandBchDmaDecodePreStart(dwReadDesIndex);
		//printf("read_page:dwReadDesIndex = %d,dwDescrIndex = %d\n",dwReadDesIndex,dwDescrIndex);
		dma_cache_wback_inv((unsigned long)pbchDecdesc[0], ((dwDescrIndex + 1) << 5));
		dma_cache_wback_inv((unsigned long)pnandRdesc[0], ((dwReadDesIndex + 2) << 5));

		if (NandReadMemcpyAndDecode())
	 		return -EIO;

		ret =  NandBchCorrect(data,g_dweccsteps);
		if(ret == -EIO)
		{
			printf("%s page = 0x%x\n",__FUNCTION__,page);
		}
 	}
	else
	{
		ret = SetZero(data,g_dwbytesperpage);
	}

	return ret;
}

int nand_write_page(void *dev_id,int page, unsigned char *data,nand_page_info_t *pinfo)
{
	int i,j;
	nand_page_info_t *ptempinfo;
	unsigned char *ptemp, puctmp;
	unsigned char *ptr, status;
	unsigned int dwDescrIndex = 0;
	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif
	
	if (dev_id != 0)
		ptempinfo = pinfo;
	else
		ptempinfo = info;

	ptr = (unsigned char *) data;
	
	unsigned char *par = (unsigned char *)&(ptempinfo->ecc_field);
	for(j = 0;j<g_dweccsteps; j++)
	{
		NandBchDmaDesSetEncode(dwDescrIndex,par,ptr);
		dwDescrIndex ++;
		par += PAR_SIZE;
		ptr += g_dweccblocksize;
	}
	
	dwDescrIndex --;
	NAND_BCH_ENCODE(g_dweccblocksize );

	#if DMA_INTERRUPT_ENABLE
		pbchEncdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN; 
		pbchEncdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
	#else
		pbchEncdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN); 
	#endif
	dma_cache_wback_inv((unsigned long)pbchEncdesc[0], ((dwDescrIndex + 1) << 5));
	
	if ( NandBchDmaEncodeEnable())
		goto write_error;
		
	dwDescrIndex = 0;
	NandSendWriteAddr(page,0);
	NandWriteDesSet(dwDescrIndex,(void *)&NAND_IO_ADDR, data, g_dwbytesperpage);
	dwDescrIndex++;
	ptr = (unsigned char *) ptempinfo;
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
// must sigle block

int nand_multi_read_page(unsigned int page, unsigned char *mask,unsigned char *data)
{
	int i,j,ret = 0;
	unsigned char *ptr;
	unsigned char *par;
	unsigned char *tmpbuf;
	unsigned int dat;
	unsigned int dwDescrIndex = 0,dwReadDesIndex = 0;

	#define NAND_PAGE_MASK ((1 << g_dweccsteps) - 1)

	#ifdef JZ_DEBUG
	  printf("%s() page: 0x%x NAND_PAGE_MASK = 0x%x\n", __FUNCTION__, page,*mask);
	#endif

	if(nand_read_oob(page,info))
	{
		printf("%s page 0x%x oob error!",__FUNCTION__,page);
		return -EIO;
	}

	ptr = (unsigned char *) data;
	par = (unsigned char *)&(info->ecc_field);

	tmpbuf = ptr;

	NandSendRandomReadAddr(0);
	NandReadDesSet(dwReadDesIndex,ptr,(void *)&NAND_IO_ADDR,g_dwbytesperpage);
	dwReadDesIndex++;

	for(j = 0; j < g_dweccsteps; j++)
	{
		NandBchDmaDesSetDecode(dwDescrIndex,(void *)pstBchDeTarget[j],ptr,par);
		dwDescrIndex+=2;
		ptr+=g_dweccblocksize;
		par+=PAR_SIZE;
	}

	*mask = 1;

	dwDescrIndex--;
	NAND_BCH_DECODE( g_dweccblocksize + PAR_SIZE);

	#if DMA_INTERRUPT_ENABLE
		pbchDecdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
		pbchDecdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
	#else
		pbchDecdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
	#endif

	NandBchDmaDecodePreStart(dwReadDesIndex);
	//printf("read_page:dwReadDesIndex = %d,dwDescrIndex = %d\n",dwReadDesIndex,dwDescrIndex);
	dma_cache_wback_inv((unsigned long)pbchDecdesc[0], ((dwDescrIndex + 1) << 5));
	dma_cache_wback_inv((unsigned long)pnandRdesc[0], ((dwReadDesIndex +2) << 5));

	if (NandReadMemcpyAndDecode())
		return -EIO;

	ret =  NandBchCorrect(tmpbuf,g_dweccsteps);
	if(ret == -EIO)
	{
		printf("%s page = 0x%x\n",__FUNCTION__,page);
	}

	return ret;
}

int nand_write_page_info(unsigned int page, nand_page_info_t *pinfo)
{
	int i;
	unsigned char *ptr;

	NandSendWriteAddr(page,g_dwbytesperpage);
	ptr = (unsigned char *) pinfo;
	for (i = 0; i < g_dwoobsize; i++)
	{
		NAND_IO_ADDR = *ptr++;
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
	printf("write page : 0x%x 's info failed\n", page);
	return -EIO;
}

int nand_erase_block(void *dev_id, unsigned int addr)
{
	int i;

	REG_NAND_CMD = NAND_CMD_BLOCK_ERASE_START;

	if ((g_dwplanenum == 2) && (dev_id == NULL))
	{
		P2_SEND_ERASE_ADDR(addr);
	}
	else
	{
		REG_NAND_ADDR = (unsigned char)((addr & 0xFF));
		REG_NAND_ADDR = (unsigned char)((addr >> 8) & 0xFF);
		if(CONFIG_SSFDC_NAND_ROW_CYCLE >= 3 )
			REG_NAND_ADDR = (unsigned char)((addr >> 16) & 0xFF);
	}

	CLEAR_RB();
	
	REG_NAND_CMD = NAND_CMD_BLOCK_ERASE_CONFIRM;


	#if NAND_INTERRUPT
		unsigned char err;
		os_SemaphorePend(rb_sem,PAGE_TIMEOUT,&err);
		if(err)
		{
			goto erase_error;
		}
	#else
		nand_wait_ready();
	//	printf("erase block:wait!\n");
	#endif

	REG_NAND_CMD = NAND_CMD_READ_STATUS;
	i = JZ_NAND_ERASE_TIME / 2 ;
	while((NAND_IO_ADDR & 0x01) && (i--));
	if(i > 0)
	{
		return 1;
	}

erase_error:
	printf("erase block: 0x%x failed\n", addr);
	return 0;
}


static inline void NandBCHDescrData( unsigned char *data, nand_page_info_t *info , unsigned int *pdwDescrParIndex)
{
	int i,j;
	unsigned char *ptr,*pOOB;
	unsigned int dwDescrParIndex = *pdwDescrParIndex;
	unsigned char *par ;
	par = (unsigned char *)(info->ecc_field);
	ptr = (unsigned char *) data;

	for ( j = 0;j < g_dweccsteps; j++ ) //g_dweccsteps
	{
		NandBchDmaDesSetEncode(dwDescrParIndex,par,ptr);
		dwDescrParIndex ++ ;
		ptr += g_dweccblocksize;
		par += PAR_SIZE;
	}
	//NAND_BCH_ENCODE(g_dweccblocksize );
	*pdwDescrParIndex = dwDescrParIndex;

}


static inline void NandWritePageData( unsigned char *data, nand_page_info_t *info ,
																										unsigned int *pdwDescrIndex)
{
	unsigned int dwDescrIndex = *pdwDescrIndex;
	unsigned char *ptr,*pOOB;

	pOOB = (unsigned char *)info;
	ptr = (unsigned char *) data;

	NandWriteDesSet(dwDescrIndex ,(void *)&NAND_IO_ADDR, ptr, g_dwbytesperpage); // write page data
	dwDescrIndex++;

	NandWriteOObDesSet(dwDescrIndex ,(void *)&NAND_IO_ADDR, pOOB, g_dwoobsize);//write info data
	dwDescrIndex++;

	*pdwDescrIndex = dwDescrIndex;
}

static inline void NandSendWriteRBAddr(unsigned int dwDescrIndex , void *target,void *source,unsigned int len)
{
	int ch = NAND_DMA_CHANNEL;
	unsigned char err;
	unsigned int dwtemp;

	#ifdef JZ_DEBUG
		printf("%s():target = 0x%08x source = 0x%08x\r\n", __FUNCTION__,target,source);
	#endif

	pnandWdesc[dwDescrIndex]->dwDES[1] = PHYSADDR((unsigned long)source);
	pnandWdesc[dwDescrIndex]->dwDES[2] = PHYSADDR((unsigned long)target);
	pnandWdesc[dwDescrIndex]->dwDES[3] &= 0xFF000000;//The highest byte is next Descriptor offset,needn't change
	pnandWdesc[dwDescrIndex]->dwDES[3] |= len;
	pnandWdesc[dwDescrIndex]->dwDES[5] = DRT_NAND;

	pnandWdesc[dwDescrIndex]->dwDES[0] = DES_SAI | DES_DAI | DES_SP_08BIT | DES_DP_08BIT
								| DES_TSZ_08BIT | DES_BLAST | DES_LINK_EN;
//	printfDMADescriptor(pnandWdesc[dwDescrIndex]);
}

int nand_multiwrite_p2(void *dev_id, unsigned int page, unsigned int pagecount,unsigned char *mask,unsigned char *data, nand_statuspage_t *infopage)
{
	int i,j;
	unsigned char *ptr1,*ptr2,*mask1,*mask2,*pOOB, status;
	unsigned int page1,page2;
	unsigned int dwDescrIndex , dwDescrCmdIndex , dwDescrAddrIndex ;
	unsigned int dwDescrParIndex ;
	unsigned int  pagemsk = 0;
	unsigned int *ptemp ;
	unsigned char tmp ;
	unsigned int siglepagesperblock = g_dwnandperblock / 2;
	//unsigned long	ulTimeStart = 0,ulTimeEnd = 0,ulTime=0,ulSpeed=0 ;

	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif
		
	ptr1 = (unsigned char *)data;
	mask1 = mask;
	page1 = page - 1;
	
	ptr2 = (unsigned char *)data + (siglepagesperblock - 1) * g_dwbytesperpage;
	mask2 = mask1 + siglepagesperblock - 1;
	page2 = page1 + siglepagesperblock;
	
	if(nand_write_statuspage(page1, (unsigned char *)infopage))
		goto write_error;
	
	page1++;
	
	if (*mask2 != 0)
	{
		if (nand_write_page(0,page2, ptr2,0))
			goto write_error;
	}
	
	mask2++;
	page2++;
	ptr2 += g_dwbytesperpage;
	
	pagecount = pagecount / 2;
			
	//printf("write two plane!\n");
	while(pagecount--)
	{
		dwDescrIndex = 0 ,dwDescrCmdIndex = 0 ,dwDescrAddrIndex = 0;
		dwDescrParIndex = 0;
		if((*mask1 != 0) || (*mask2 != 0))
		{
			pagemsk = page1 & g_dwplanemask;

			*pucCMD[dwDescrCmdIndex] = NAND_CMD_PAGE_PROGRAM_START;
			NandWirteAddrDesSet(dwDescrIndex ,(void *)&NAND_CMD_ADDR ,pucCMD[dwDescrCmdIndex] ,1);
			dwDescrCmdIndex++;
			dwDescrIndex++;

			*pucAddr[dwDescrAddrIndex] = 0;
			*pucAddr[dwDescrAddrIndex + 1] = 0;
			*pucAddr[dwDescrAddrIndex + 2] = (pagemsk & 0xff);
			*pucAddr[dwDescrAddrIndex + 3] = ((pagemsk >> 8) & 0xff);
			*pucAddr[dwDescrAddrIndex + 4] = ((pagemsk >> 16) & 0xff);

			NandWirteAddrDesSet(dwDescrIndex ,(void *)&NAND_ADDR_ADDR , pucAddr[dwDescrAddrIndex] ,5);
			dwDescrIndex ++;
			dwDescrAddrIndex += 5;

			if (*mask1 != 0)
			{
				NandBCHDescrData(ptr1, info, &dwDescrParIndex);
				NandWritePageData(ptr1, info, &dwDescrIndex);
			}

			dwDescrIndex --;
			#if DMA_INTERRUPT_ENABLE
				pnandWdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
				pnandWdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
			#else
				pnandWdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
			#endif
			NandBchDmaEncodePreStart(dwDescrParIndex);
			dma_cache_wback_inv((unsigned long)pbchEncdesc[0], ((dwDescrParIndex + 2) << 5));
			dma_cache_wback_inv((unsigned long)pnandWdesc[0], ((dwDescrIndex + 1) << 5));
			NAND_BCH_ENCODE(g_dweccblocksize );
			if (NandWriteMemcpyAndEncode())
				goto write_error;
			
			CLEAR_RB();
			
			REG_NAND_CMD = P2_1_CMD_PAGE_PROGRAM_STOP;
			
			nand_wait_ready();				
			
			dwDescrIndex = 0 ,dwDescrCmdIndex = 0 ,dwDescrAddrIndex = 0;
			dwDescrParIndex = 0;

			*pucAddr[dwDescrAddrIndex] = 0;
			*pucAddr[dwDescrAddrIndex + 1] = 0;
			*pucAddr[dwDescrAddrIndex + 2] = (page2 & 0xff);
			*pucAddr[dwDescrAddrIndex + 3] = ((page2 >> 8) & 0xff);
			*pucAddr[dwDescrAddrIndex + 4] = ((page2 >> 16) & 0xff);

			NandWirteAddrDesSet(dwDescrIndex ,(void *)&NAND_ADDR_ADDR , pucAddr[dwDescrAddrIndex] ,5);
			dwDescrIndex ++;
			dwDescrAddrIndex += 5;

			if (*mask2 != 0)
			{
				NandBCHDescrData(ptr2,info1, &dwDescrParIndex);
				NandWritePageData(ptr2,info1, &dwDescrIndex);
			}
			
			dwDescrIndex --;
			REG_NAND_CMD = P2_2_CMD_PAGE_PROGRAM_START;
			
			#if DMA_INTERRUPT_ENABLE
				pnandWdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
				pnandWdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
			#else
				pnandWdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
			#endif
			NandBchDmaEncodePreStart(dwDescrParIndex);
			dma_cache_wback_inv((unsigned long)pbchEncdesc[0], ((dwDescrParIndex + 2) << 5));
			dma_cache_wback_inv((unsigned long)pnandWdesc[0], ((dwDescrIndex + 1) << 5));
			NAND_BCH_ENCODE(g_dweccblocksize );
			if (NandWriteMemcpyAndEncode())
				goto write_error;
			
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
			if(i <= 0)
				goto write_error;
		}
		
		mask1++;
		mask2++;

		page1++;
		page2++;

		ptr1 = ptr1 + g_dwbytesperpage;
		ptr2 = ptr2 + g_dwbytesperpage;

	}

	return 0;

write_error:
	printf("write page: 0x%x failed\n", page);
	return -EIO;
}

int nand_multiwrite_p1(void *dev_id, unsigned int page, unsigned int pagecount,unsigned char *mask,unsigned char *data, nand_statuspage_t *infopage)
{
	int i,j;
	unsigned char *ptr,*pOOB, status;
	unsigned int dwDescrIndex=0 , dwDescrParIndex =0,dwDescrCmdIndex = 0 ,dwDescrAddrIndex = 0;

	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif

	ptr = (unsigned char *)data;
	
	if (nand_write_statuspage(page - 1, (unsigned char *)infopage))
		goto write_error;
		
	__gpio_as_func0(RB_GPIO_PIN);
//	printf("single plane!\n");
	while(pagecount--)
	{
		if(*mask != 0)
		{
		//	printf("%s page =%d,mask = 0x%x\n",__FUNCTION__,page,*mask);
			dwDescrIndex = 0 ,dwDescrCmdIndex = 0 ,dwDescrAddrIndex = 0;
			dwDescrParIndex = 0;
		//	SETREG32(A_GPIO_PXFLGC(RB_GPIO_GROUP) , RB_GPIO_BIT_FLG);

			NandSendWriteAddr(page,0);

			NandBCHDescrData(ptr, info, &dwDescrParIndex);
			NandWritePageData(ptr, info, &dwDescrIndex);

			*pucCMD[dwDescrCmdIndex] = NAND_CMD_PAGE_PROGRAM_STOP;
			NandWirteAddrDesSet(dwDescrIndex ,(void *)&NAND_CMD_ADDR ,pucCMD[dwDescrCmdIndex] ,1);

			dwDescrCmdIndex++;
			dwDescrIndex++;
			*pucCMD[dwDescrCmdIndex] = NAND_CMD_READ_STATUS;
			NandSendWriteRBAddr(dwDescrIndex ,(void *)&NAND_CMD_ADDR ,pucCMD[dwDescrCmdIndex] ,1);

			#if DMA_INTERRUPT_ENABLE
				pnandWdesc[dwDescrIndex]->dwDES[0] &= ~DES_LINK_EN;
				pnandWdesc[dwDescrIndex]->dwDES[0] |= DES_TRANS_INT_EN;
			#else
				pnandWdesc[dwDescrIndex]->dwDES[0] &= ~(DES_LINK_EN | DES_TRANS_INT_EN);
			#endif

			NandBchDmaEncodePreStart(dwDescrParIndex);

			dma_cache_wback_inv((unsigned long)pbchEncdesc[0], ((dwDescrParIndex + 2) << 5));
			dma_cache_wback_inv((unsigned long)pnandWdesc[0], ((dwDescrIndex + 1) << 5));
			NAND_BCH_ENCODE(g_dweccblocksize);
			/////
			if (NandWriteMemcpyAndEncode())
			{
				printf("NandWriteMemcpyAndEncode timeout\n");
				goto write_error;
			}
			i = g_dwoobsize * JZ_WAIT_STAUTS_DELAY * 1000;
			while((NAND_IO_ADDR & 0x01) &&(i--)) ;
			if(i <= 0)
				goto write_error;
		}
		mask++;
		page++;
		ptr += g_dweccblocksize * g_dweccsteps;
	}

	gpio_intr_init(GITT_R_EDGE, RB_GPIO_PIN);

	return 0;

write_error:
	printf("write page: 0x%x failed\n", page);
	gpio_intr_init(GITT_R_EDGE, RB_GPIO_PIN);
	return -EIO;
}
