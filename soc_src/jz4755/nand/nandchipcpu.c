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

//#define JZ_DEBUG
//#undef printf

extern unsigned int g_dwchipnum;
extern unsigned int g_dwplanenum;
extern unsigned int g_dwplanemask;
extern unsigned int g_dwnandperblock;
extern unsigned int g_dweccsteps;

extern unsigned int g_dwbytesperpage;
extern unsigned int g_dwoobsize;

extern	PJZ4755_GPIO2  v_pGPIOReg;

//
//	NAND OP
//
extern unsigned int NANDFLASH_BASE;

static void inline NandSendReadCacheAddr(unsigned short offset);
/* global mutex between yaffs2 and ssfdc */

#define setzero(x,y) memset(x,0,y)

#define NAND_INTERRUPT 1
#if NAND_INTERRUPT
	static OS_EVENT * rb_sem = NULL;
 
	#define NAND_CLEAR_RB() (OUTREG32(A_GPIO_PXFLGC(RB_GPIO_GROUP), RB_GPIO_BIT_FLG))
	
	#define nand_wait_ready()                     \
	do{                                           \
	    unsigned int timeout = (1000 * 1000 / OS_TICKS_PER_SEC);             \
		while((!(INREG32(A_GPIO_PXFLG(RB_GPIO_GROUP)) & RB_GPIO_BIT_FLG))& timeout--);\
	    OUTREG32(A_GPIO_PXFLGC(RB_GPIO_GROUP),RB_GPIO_BIT_FLG); \
	}while(0)
	
	#define READDATA_TIMEOUT	(100 * 1000 / OS_TICKS_PER_SEC)
	#define WRITEDATA_TIMEOUT	(800 * 1000 / OS_TICKS_PER_SEC)
	#define PAGE_TIMEOUT		(100 * 1000 / OS_TICKS_PER_SEC)
	
	#define CLEAR_RB()						\
	do {									\
		NAND_CLEAR_RB();					\
		OUTREG32(A_GPIO_PXIMC(RB_GPIO_GROUP), RB_GPIO_BIT_FLG);	\
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
	  while ((v_pGPIOReg->group[RB_GPIO_GROUP].PIN & 0x8000000) && timeout--);
	  while (!(v_pGPIOReg->group[RB_GPIO_GROUP].PIN & 0x8000000));
	}
	#define WAIT_RB()	nand_wait_ready()
#endif

#define BCH_INITERRUPT 0
#if BCH_INITERRUPT
	static OS_EVENT * bch_sem = NULL;
	#define NAND_BCH_DECODE_SYNC()     \
	do{                                  \
		unsigned char err;                 \
		OUTREG32(A_BCH_BHINTES, BCH_INTS_DECF); \
		os_SemaphorePend(bch_sem,2,&err);          \
		if(err)                            \
			   printf("rs wait timeout!\n"); \
	}while(0)
#else
	#define NAND_BCH_DECODE_SYNC() while (!(INREG32( A_BCH_BHINTS ) & BCH_INTS_DECF))//while (!(REG_EMC_NFINTS & EMC_NFINTS_DECF))
#endif
#define NAND_RESET() while(1)

//-----------------------------------------------------------------------------
//
//	Functions
//
//-----------------------------------------------------------------------------

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

int nand_read_oob(int page,nand_page_info_t *info)
{
 	unsigned char *ptr;
 	unsigned int i;
	if (NandSendReadAddr(page,g_dwbytesperpage))
		return -1;
	ptr = (unsigned char *)info;	
	
	for ( i = 0; i < g_dwoobsize; i++) 
	{
		unsigned char d;
		d =NAND_IO_ADDR;
		*ptr++ = d;
	}
	return 0;
	
}
static inline int BchCorrect(unsigned char *dat, int idx)
{
	int i, bit;  // the 'bit' of i byte is error 
	//printf("error bit index=%d\n",idx);
	idx--;
	if (idx < (JZSOC_ECC_BLOCK * 8))
	{
		i = idx / 8;
		bit = idx % 8;
		dat[i] ^= (1 << bit);
	}
	else
	{
		printf("error bit index=%d\n",idx);	
	}	
	return 0;
}

static int NandBchCorrect(unsigned char *tmpbuf)
{
	unsigned int stat,ret = 0;
	unsigned int pBchBHERR[4];
	stat = INREG32(A_BCH_BHINTS);//REG_EMC_NFINTS;
	pBchBHERR[0] = INREG32(A_BCH_BHERR0);
	pBchBHERR[1] = INREG32(A_BCH_BHERR1);
	pBchBHERR[2] = INREG32(A_BCH_BHERR2);
	pBchBHERR[3] = INREG32(A_BCH_BHERR3);
	
	if (stat & BCH_INTS_ERR) 
	{
		if (stat & BCH_INTS_UNCOR) 
		{
			printf("Uncorrectable error occurred\n");
			printf("stat = 0x%x\n", stat);
			return -EIO;
		}
		else 
		{
			//printf("Correctable error occurred\n");
			unsigned int errcnt = (stat & BCH_INTS_ERRC_MASK) >> BCH_INTS_ERRC_BIT;

			//printf("stat = 0x%x error count = %d\n", stat,errcnt);
			switch (errcnt) 
			{
				case 8:
					ret = BchCorrect(tmpbuf, (pBchBHERR[3] & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
				case 7:
					ret = BchCorrect(tmpbuf, (pBchBHERR[3] & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
				case 6:
					ret = BchCorrect(tmpbuf, (pBchBHERR[2] & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
				case 5:
					ret = BchCorrect(tmpbuf, (pBchBHERR[2] & BCH_ERR_INDEX_EVEN_MASK) >> BCH_ERR_INDEX_EVEN_BIT);
				case 4:
					ret = BchCorrect(tmpbuf, (pBchBHERR[1] & BCH_ERR_INDEX_ODD_MASK) >> BCH_ERR_INDEX_ODD_BIT);
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
				dma_cache_wback_inv(tmpbuf,512);
		}
		
	}
	return ret;	
}

int nand_read_page (void *dev_id, int page, unsigned char *data,nand_page_info_t *info)
{
	int i,j = 0,ret = 0;
	unsigned char *ptr;
	unsigned char *par;
	unsigned char *tmpbuf;
	unsigned int pagecontinue = -1;
		
	#ifdef JZ_DEBUG
		printf("%s() page: 0x%x\n", __FUNCTION__, page);
	#endif
		
	ptr = data;
	par = (unsigned char *)&info->ecc_field;
	
	for(j = 0; (j < g_dweccsteps) && (ret == 0); j++)
	{
		if(((dev_id == 0)&&((info->page_status & 0xf) != 0xf)) || ((dev_id !=0) && ((
		info->ecc_field[j*PAR_SIZE + 0] & info->ecc_field[j*PAR_SIZE + 1] & info->ecc_field[j*PAR_SIZE + 2] &
		info->ecc_field[j*PAR_SIZE + 3] & info->ecc_field[j*PAR_SIZE + 4] & info->ecc_field[j*PAR_SIZE + 5] &
		info->ecc_field[j*PAR_SIZE + 6] ) != 0xff )
			))
		{
			tmpbuf = ptr;
			if(j != pagecontinue)
				NandSendReadCacheAddr(j*JZSOC_ECC_BLOCK);
			NAND_BCH_DECODE( JZSOC_ECC_BLOCK + PAR_SIZE);
			
			for(i = 0; i < JZSOC_ECC_BLOCK; i++)
			{
				unsigned char dat;
				dat = NAND_IO_ADDR;
				OUTREG8(A_BCH_BHDR, dat);
				*ptr++ = dat;
			}

			for(i = 0; i < PAR_SIZE; i++)
			{
				OUTREG8(A_BCH_BHDR, *par++);
			}
			
			NAND_BCH_DECODE_SYNC();
			NAND_BCH_DISABLE();
			
			ret =  NandBchCorrect(tmpbuf);
			if(ret == -EIO)
			{
				printf("%s page = 0x%x,sector = %d,%x\n",__FUNCTION__,page,j,info->page_status);			
			}
			pagecontinue = j;
			pagecontinue++;
		}
		else
		{
			setzero(ptr,JZSOC_ECC_BLOCK);
			par += PAR_SIZE;
			ptr += JZSOC_ECC_BLOCK;
		}	
	}
	
	return ret;
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

static inline void NandSendWriteCacheAddr(unsigned int offset)
{
//	printf("writecacheaddr = 0x%x\n",offset);
	REG_NAND_CMD = NAND_CMD_PAGE_RANDOM_PROGRAM_START;
	REG_NAND_ADDR = (unsigned char)((offset >> 0) & 0x0FF);
	REG_NAND_ADDR = (unsigned char)((offset >> 8) & 0x0FF);	
}

int nand_write_page(int page, unsigned char *data, nand_page_info_t *info)
{
	int i,j;
	unsigned char *ptemp;
	unsigned char *ptr;
		
	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif
		
	NandSendWriteAddr(page,0);
	ptr = (unsigned char *) data;

	unsigned char *par = (unsigned char *)&(info->ecc_field);
		
	for(j = 0;j<g_dweccsteps; j++)
	{
		NAND_BCH_ENCODE(JZSOC_ECC_BLOCK);// + (ECC_POS / g_dweccsteps));
		for(i =0; i < JZSOC_ECC_BLOCK; i++)	
		{
			OUTREG8(A_BCH_BHDR, *ptr);
			NAND_IO_ADDR = *ptr++;
		}
		
		__nand_ecc_encode_sync();
	
		ptemp = (unsigned char *)A_BCH_BHPAR0;
		
		for (i = 0; i < PAR_SIZE; i++)
		{
			*par++ = *ptemp++;
		}
		
		NAND_BCH_DISABLE();

	}
	
	ptr = (unsigned char *) info;
	for (i = 0; i < g_dwoobsize; i++)
		NAND_IO_ADDR = *ptr++;
	
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

#if NAND_INTERRUPT
static void RBHandler(unsigned int arg)
{
	__gpio_mask_irq(arg);
	os_SemaphorePost(rb_sem);	
}
#endif

#if BCH_INITERRUPT
static void NandBchDecodeHandler(unsigned int arg)
{
	OUTREG32(A_BCH_BHINTEC,BCH_INTS_DECF);
	OUTREG32(A_BCH_BHINTS,BCH_INTS_DECF);
	os_SemaphorePost(bch_sem);
}
#endif

static void inline NandSendReadCacheAddr(unsigned short offset)
{
	REG_NAND_CMD = NAND_CMD_RANDOM_READ;
	REG_NAND_ADDR = (unsigned char)((offset & 0x000000FF) >> 0);
	REG_NAND_ADDR = (unsigned char)((offset & 0x0000FF00) >> 8);
	REG_NAND_CMD = NAND_CMD_RANDOM_READ_CONFIRM;
}

// must sigle block
int nand_multi_read_page(unsigned int page, unsigned char *mask,unsigned char *data,nand_page_info_t *info)
{
	int i,j,ret = 0;
	unsigned char *ptr;
	unsigned char *par;
	unsigned char *tmpbuf,*tmpbuf1;
	unsigned int continue_page = -1;
	unsigned int dat;
		
	#define NAND_PAGE_MASK ((1 << g_dweccsteps) - 1)
		
	#ifdef JZ_DEBUG
	  printf("%s() page: 0x%x NAND_PAGE_MASK = 0x%x\n", __FUNCTION__, page,*mask);
	#endif
	
	ptr = (unsigned char *) data;
	par = (unsigned char *)&(info->ecc_field);
						
	for(j = 0; (j < g_dweccsteps) && (ret == 0); j++)
	{
		if(((*mask & (1 << j)) == 0)&&((info->page_status & (1<<j)) == 0))
		{
			tmpbuf = ptr;
			if(j != continue_page)
			{
				NandSendReadCacheAddr(j * JZSOC_ECC_BLOCK);
				continue_page = j;
			}
			NAND_BCH_DECODE( JZSOC_ECC_BLOCK + PAR_SIZE);
			
			for(i = 0; i < JZSOC_ECC_BLOCK; i++)
			{
				dat = NAND_IO_ADDR;
				OUTREG8(A_BCH_BHDR, dat);
				*ptr++ = dat;
			}
			
			for(i = 0; i < PAR_SIZE; i++)
			{
				dat = *par++;
				OUTREG8(A_BCH_BHDR, dat);
			}
			
			NAND_BCH_DECODE_SYNC();
			NAND_BCH_DISABLE();
			
			ret =  NandBchCorrect(tmpbuf);
		
			if(ret == -EIO)
			{
				printf("%s rs error:page = 0x%x,sector = %d\n",__FUNCTION__,page,j);								  
			}
			
			continue_page++;
			*mask |= (1 << j);
		}
		else
		{
			ptr += JZSOC_ECC_BLOCK;
			par += PAR_SIZE;
		}			
	}	

	return ret;
}

int nand_write_page_info(unsigned int page, nand_page_info_t *info)
{
	int i;
	unsigned char *ptr;

	NandSendWriteAddr(page,g_dwbytesperpage);	
	ptr = (unsigned char *) info;
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
		
	REG_NAND_CMD = NAND_CMD_BLOCK_ERASE_CONFIRM;
	
	CLEAR_RB();
	
	#if NAND_INTERRUPT
		unsigned char err;
		os_SemaphorePend(rb_sem,PAGE_TIMEOUT,&err);
		if(err)
		{
			goto erase_error;	
		}
	#else
		nand_wait_ready();
		printf("erase block:wait!\n");
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

static inline void NandWritePageData(unsigned char mask, unsigned char *data, nand_page_info_t *info)
{
	int i,j;
	unsigned char *ptr,*pOOB;
	unsigned int continue_page;
	unsigned char *par = (unsigned char *)&(info->ecc_field);
	unsigned char *ptemp;
		
	ptr = (unsigned char *) data;
		
	continue_page = 0;
	
	for ( j = 0;j < g_dweccsteps; j++ )
	{
		if (mask != 0)
		{	
			if (continue_page != j)
			{
				NandSendWriteCacheAddr(JZSOC_ECC_BLOCK * j);
				continue_page = j;
			}
			continue_page++;

	  		NAND_BCH_ENCODE(JZSOC_ECC_BLOCK );//+ (ECC_POS / g_dweccsteps));
			for(i =0; i < JZSOC_ECC_BLOCK; i++)	
			{
				OUTREG8(A_BCH_BHDR, *ptr);
				NAND_IO_ADDR = *ptr++;
			}
			__nand_ecc_encode_sync();

			ptemp = (unsigned char *)A_BCH_BHPAR0;
	
			for (i = 0; i < PAR_SIZE; i++)
			{
				*par++ = *ptemp++;
			}
					
			NAND_BCH_DISABLE();
	
		}
		else
		{
			ptr += JZSOC_ECC_BLOCK;
			par += PAR_SIZE;
		}
	}

	if (continue_page != g_dweccsteps)
	{
		NandSendWriteCacheAddr(JZSOC_ECC_BLOCK *  g_dweccsteps);
	}

	pOOB = (unsigned char *)info;
	for (i = 0; i < g_dwoobsize; i++)
		NAND_IO_ADDR = *pOOB++;

}

int nand_multiwrite_p2(void *dev_id, unsigned int page, unsigned int pagecount,unsigned char *mask,unsigned char *data, nand_page_info_t *info)
{
	int i,j;
	unsigned char *ptr1,*ptr2,*mask1,*mask2,*pOOB;
	nand_page_info_t	*info1,*info2;
	#if NAND_INTERRUPT
		unsigned char err;
	#endif
	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif
	
	pagecount = pagecount / 2;
	unsigned int siglepagesperblock = pagecount;
	ptr1 = (unsigned char *)data;
	mask1 = mask;
	info1 = info;

	ptr2 = (unsigned char *)data + pagecount * g_dwbytesperpage;
	mask2 = mask + pagecount;
	info2 = info + pagecount; 
	
	while(pagecount--)
	{
		if((*mask1 != 0) || (*mask2 != 0))
		{
			// Send the first Sector Address
			P2_SEND_SECTOR_ADDR1(page);

			// Send the first Sector Data
			if (*mask1 != 0)
				NandWritePageData(*mask1, ptr1, info1);

			CLEAR_RB();
			// Send the Sector Confirm CMD
			REG_NAND_CMD = P2_1_CMD_PAGE_PROGRAM_STOP;

			#if NAND_INTERRUPT
				os_SemaphorePend(rb_sem,PAGE_TIMEOUT,&err);
				if(err)
				{
					printf("!!!!!!!error!!! %s() page: 0x%x \n", __FUNCTION__, page);
					goto write_error;
				}
			#else
				nand_wait_ready();
			#endif
      
			// Send the 2nd Sector Address
			P2_SEND_SECTOR_ADDR2(page + siglepagesperblock);
			// Send the 2nd Sector Data
			if (*mask2 != 0)
				NandWritePageData(*mask2, ptr2, info2);

			CLEAR_RB();
			
			REG_NAND_CMD = NAND_CMD_PAGE_PROGRAM_STOP;

			#if NAND_INTERRUPT
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
		info1++;
		info2++;
		page++;
		ptr1 = ptr1 + g_dwbytesperpage;
		ptr2 = ptr2 + g_dwbytesperpage;
	}
	return 0;
	
write_error:
	printf("write page: 0x%x failed\n", page);
	return -EIO;	
}

int nand_multiwrite_p1(void *dev_id, unsigned int page, unsigned int pagecount,unsigned char *mask,unsigned char *data, nand_page_info_t *info)
{
	int i,j;
	unsigned char *ptr,*pOOB;
	
	#ifdef JZ_DEBUG
		printf("!!!!!!!gt!!! %s() page: 0x%x 0x%x 0x%x\n", __FUNCTION__, page, *data, *(data+1));
	#endif

	ptr = (unsigned char *)data;
	
	while(pagecount--)
	{
		if(*mask != 0)
		{
			NandSendWriteAddr(page,0);
			
			NandWritePageData(*mask, ptr, info);
			
            CLEAR_RB();			
			REG_NAND_CMD = NAND_CMD_PAGE_PROGRAM_STOP;
            //Wait R/B
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
		mask++;
		info++;
		page++;
		ptr += JZSOC_ECC_BLOCK * g_dweccsteps;
	}

	return 0;
	
write_error:
	printf("write page: 0x%x failed\n", page);
	return -EIO;	
}

int nand_Intr_init(void)
{	
	#if NAND_INTERRUPT	
		
	    NAND_CLEAR_RB();
		__gpio_as_irq_rise_edge(RB_GPIO_PIN);
		__gpio_disable_pull(RB_GPIO_PIN);
	    if(rb_sem == NULL)
			rb_sem = os_SemaphoreCreate(0);
		request_irq(EIRQ_GPIO_BASE + RB_GPIO_PIN, RBHandler,RB_GPIO_PIN);
	#endif

	#if BCH_INITERRUPT
		bch_sem = os_SemaphoreCreate(0);
		request_irq(IRQ_BCH,NandBchDecodeHandler,0);
		OUTREG32(A_INTC_ICMCR,IRQ_BCH);
		OUTREG32(A_BCH_BHINTEC,0xff);
	#else
		OUTREG32(A_BCH_BHINTEC,0xff);
		OUTREG32(A_INTC_ICMSR,IRQ_BCH);
	#endif

	return 0;
}

