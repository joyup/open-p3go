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
 *  Create:   2008-10-09, by xyzhang
 *
 *            
 *
 *******************************************************************************
 */

#include <mmc_config.h>
#include <bsp.h>
#include <jz4755.h>
#define CACHE_MAX_NUM 128 //16
#define SECTOR_SIZE 512
typedef struct {
	unsigned short  CacheState;
	unsigned short  UseCount;
	unsigned short  CacheChange;
	unsigned short  CacheReserve;	
	unsigned int BlockId;
	unsigned char   aBlockData[SECTOR_SIZE];
} MMC__LB_CACHE;


#define FREE_CACHE 0
#define PREWRITE_CACHE 2
#define OFTEN_USE_CACHE 3
#if 0
#define dprintf(x,...)  printf(x)
#else
#undef  dprintf
#define dprintf(x,...)
#endif

static MMC__LB_CACHE mmc_cache[CACHE_MAX_NUM]  __attribute__ ((aligned (4)));
static unsigned short Cur_CacheCount = 0;


#define DMA_ENABLE 0

#if DMA_ENABLE
#define DMA_CHANNEL 5
#define PHYSADDR(x) ((x) & 0x1fffffff)
#else
#define lb_memcpy memcpy
#endif

#if DMA_ENABLE
static void lb_memcpy(void *target,void* source,unsigned int len)
{
    int ch = DMA_CHANNEL;
    if(((unsigned int)source < 0xa0000000) && len)
	    dma_cache_wback_inv((unsigned long)source, len);
    if(((unsigned int)target < 0xa0000000) && len)
	    dma_cache_wback_inv((unsigned long)target, len);
	
	
    REG_DMAC_DSAR(ch) = PHYSADDR((unsigned long)source);       
    REG_DMAC_DTAR(ch) = PHYSADDR((unsigned long)target);       
    REG_DMAC_DTCR(ch) = len / 32;	            	    
    REG_DMAC_DRSR(ch) = DMAC_DRSR_RS_AUTO;	        	
    REG_DMAC_DCMD(ch) = DMAC_DCMD_SAI| DMAC_DCMD_DAI | DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32|DMAC_DCMD_DS_32BYTE;
    REG_DMAC_DCCSR(ch) = DMAC_DCCSR_EN | DMAC_DCCSR_NDES;
    while (REG_DMAC_DTCR(ch));
}
#endif
	
static void _MMC_LB_InitCache()
{
	int i,ret = 0;
	MMC__LB_CACHE *pCache = mmc_cache;
	i = 0;
	while(1)
	{
		if(i >= CACHE_MAX_NUM)
		{
			ret = -1;
			break;
		}
		pCache->CacheState = FREE_CACHE;
		pCache->UseCount = 0;
		pCache->CacheChange = 0;
		pCache++;
		i++;
	}
	Cur_CacheCount = 0;
	
}

static  MMC__LB_CACHE * _MMC_LB_GetFreeCache()
{
	int i,ret = 0;
	MMC__LB_CACHE *pCacheInfo = &mmc_cache[Cur_CacheCount];
	while(1)
	{
        if(ret >= CACHE_MAX_NUM)
			return 0;
		if(pCacheInfo >= &mmc_cache[CACHE_MAX_NUM -1])
		{
			pCacheInfo = mmc_cache;
			Cur_CacheCount = 0;
		}
		
		if(pCacheInfo->CacheState == FREE_CACHE)
		{
           	return pCacheInfo;
		}
		pCacheInfo++;
		Cur_CacheCount++;
		ret++;
	}
	return 0;
}
#define _MMC_LB_Write(pCache)  MMC_WriteBlock(pCache->BlockId,pCache->aBlockData)

static void _MMC_LB_CloseCACHES(unsigned int sectorstart,unsigned int sectorend){
  unsigned int i;
  MMC__LB_CACHE *pCache = mmc_cache;
  for( i = 0;i < CACHE_MAX_NUM;i++){
    if((pCache->CacheState != FREE_CACHE) && (pCache->BlockId >= sectorstart) && (pCache->BlockId < sectorend)){
      pCache->CacheChange = 0;
      pCache->CacheState = FREE_CACHE;			    
      pCache->UseCount = 0;
    }
    pCache++;
    
  }
    
}


static void _MMC_LB_FLUSHCACHES(unsigned int sectorstart,unsigned int sectorend){
  unsigned int i;
  MMC__LB_CACHE *pCache = mmc_cache;
  for( i = 0;i < CACHE_MAX_NUM;i++){
    if((pCache->CacheState != FREE_CACHE) && (pCache->BlockId >= sectorstart) && (pCache->BlockId < sectorend)){
	    _MMC_LB_Write(pCache);
	    pCache->CacheChange = 0;
	    pCache->CacheState = FREE_CACHE;			    
	    pCache->UseCount = 0;
    }
    pCache++;
    
  }
    
}

inline static int Get_MMC_CacheFreeCount()
{
	MMC__LB_CACHE *pCache = mmc_cache;
	MMC__LB_CACHE *pEndCache = &mmc_cache[CACHE_MAX_NUM];
	unsigned int count = 0;
	while(pCache < pEndCache)
	{
		if(pCache->CacheState == FREE_CACHE)
			count++;
		pCache++;	
	}
	return count;
	
}
static void _MMC_LB_PreWiteToMmc(MMC__LB_CACHE *pCache,unsigned short *count,unsigned int update)
{
	MMC__LB_CACHE *pWriteCache;
	//SSFDC__LB_CACHE *pOldCache = pCache;
	MMC__LB_CACHE *pEndCache = &mmc_cache[CACHE_MAX_NUM];
	unsigned int sector = -1;
	unsigned int flag;
	while(1)
	{
		sector = -1;
		flag = 0;
		pWriteCache = mmc_cache;
		while(pWriteCache < pEndCache)
		{
			if(pWriteCache->CacheState == update) //PREWRITE_CACHE
			{
				if(pWriteCache->BlockId < sector)
				{
						sector = pWriteCache->BlockId;
						pCache = pWriteCache;
				}
			}else
					flag++;
			pWriteCache++;
		}
		
		if(flag < CACHE_MAX_NUM)
		{	
			if(pCache->CacheChange)
			{
				_MMC_LB_Write(pCache);
				pCache->CacheChange = 0;
	    }
			pCache->CacheState = FREE_CACHE;			    
			pCache->UseCount = 0;
			(*count)++;
	 }else
	 	   break;		
	}
}
static void _MMC_LB_OftenToMmc(MMC__LB_CACHE *pCache,unsigned short *count,unsigned int update)
{
	MMC__LB_CACHE *pWriteCache = pCache;
	MMC__LB_CACHE *pOldCache = pCache;
	MMC__LB_CACHE *pEndCache = &mmc_cache[CACHE_MAX_NUM];
	
	while(pCache)
	{
		
		if(pCache->CacheState == OFTEN_USE_CACHE)
		{
			if(pWriteCache->CacheState != OFTEN_USE_CACHE)
				pWriteCache = pCache;
			else if(pWriteCache->UseCount > pCache->UseCount)
			{
				pWriteCache = pCache;
			}
			
		}
		pCache++;
		if(pCache >= pEndCache)
			break;
	}
	if(pWriteCache->CacheState == OFTEN_USE_CACHE)
	{
		(*count)++;
		if(pWriteCache->CacheChange)
			_MMC_LB_Write(pWriteCache);
		//if(update == 1)
		pWriteCache->CacheState = FREE_CACHE;			    
			
		pWriteCache->UseCount = 0;
		pWriteCache->CacheChange = 0;
		if(update != -1)
			update--;
		if(update != 0)
			_MMC_LB_OftenToMmc(pOldCache,count,update);
		
	}
}

static int _MMC_LB_FreeCache(unsigned int update)
{
	unsigned short freecount = 0,totalfree = 0;
    
	freecount = 0;
	
	_MMC_LB_PreWiteToMmc(mmc_cache,&freecount,PREWRITE_CACHE);
    
	totalfree += freecount;
	if(freecount == 0)
	{
		freecount = 0;	
		_MMC_LB_PreWiteToMmc(mmc_cache,&freecount,OFTEN_USE_CACHE);
		totalfree += freecount;
		update = 0;
	}	
	if(update)
	{	
		if(Get_MMC_CacheFreeCount() < CACHE_MAX_NUM * 1 / 4) // because fat is 4 sector
		{
			//printf("!!!!!!!!!!!!!!!!11\n");
			freecount = 0;	
			_MMC_LB_PreWiteToMmc(mmc_cache,&freecount,OFTEN_USE_CACHE);
			totalfree += freecount;
		}
  }

	return totalfree;
}
static int _MMC_LB_GetFromCache(unsigned int Sector, void *pBuffer) {

	MMC__LB_CACHE *pCache = &mmc_cache[Cur_CacheCount];
	MMC__LB_CACHE *pUseCache = 0;
	unsigned short i;
	dprintf("sector = %x pBuffer = %x\n",Sector,pBuffer);
	
	if(pCache >= &mmc_cache[CACHE_MAX_NUM])
		pCache = mmc_cache;

	i = 0;
	while (1) {
		
		if(pCache->CacheState != FREE_CACHE)
		{
			
			if (Sector == pCache->BlockId)  {
				dprintf("Cache is use = %d\r\n",pCache->BlockId);
				
				pUseCache = pCache;
				
				pCache->UseCount++;
				if(pCache->UseCount == 0)
					pCache->UseCount = -1;
			  
				pCache->CacheState = OFTEN_USE_CACHE;
				
			}
		}
		pCache--;
		if(pCache < mmc_cache)
			pCache = &mmc_cache[CACHE_MAX_NUM - 1];
		
		i++;
		if (i >= CACHE_MAX_NUM) {
			break;  /* Sector not in cache */
		}
	}
	if (pUseCache) {
		//dprintf("From Cache %d\r\n",Sector);
		
		lb_memcpy(pBuffer, pUseCache->aBlockData, SECTOR_SIZE);
		return 0;
	}
	return -1;  
}

static void _MMC_LB_ClearCache() {

	unsigned short freecount = 0;
	// dprintf("Clear Cache\r\n");
		
	_MMC_LB_PreWiteToMmc(mmc_cache,&freecount,PREWRITE_CACHE);
	_MMC_LB_PreWiteToMmc(mmc_cache,&freecount,OFTEN_USE_CACHE);

}
	
	

	
static void _MMC_LB_CopyToCache(unsigned int Sector, void *pBuffer,unsigned short rw) {

	MMC__LB_CACHE *pCache = _MMC_LB_GetFreeCache();
//    printf("Copy to Cache = 0x%08x 0x%08x\r\n",pCache,mmc_cache);
	
	if(!pCache)
	{
		// dprintf("MMC Free!\r\n");
		
		_MMC_LB_FreeCache(rw);
		
		pCache = _MMC_LB_GetFreeCache();
	}

	pCache->BlockId = Sector;
	pCache->CacheState = PREWRITE_CACHE;
	pCache->UseCount = 0;
	pCache->CacheChange = rw;
	
	lb_memcpy(pCache->aBlockData,pBuffer,SECTOR_SIZE);
}


int _MMC_LB_UpdateInCache(unsigned int Sector, void *pBuffer) {
	short i,ret = 0;
	i =  Cur_CacheCount;
	if(Cur_CacheCount > CACHE_MAX_NUM)
		i = 0;
	while(1)
	{
		if(ret >= CACHE_MAX_NUM)
			return -1;
		if(mmc_cache[i].CacheState != FREE_CACHE)
		{
        
			if(mmc_cache[i].BlockId == Sector)
			{
				dprintf("UpdateInCache = %d\r\n",Sector);
				mmc_cache[i].CacheState = OFTEN_USE_CACHE;
				mmc_cache[i].UseCount++;
				mmc_cache[i].CacheChange = 1;
				lb_memcpy(mmc_cache[i].aBlockData,pBuffer,SECTOR_SIZE);
				return 0;
				
			}
		}
		i--;
		if(i < 0)
			i = CACHE_MAX_NUM - 1;

		ret++;
	}
	return -1;
}


/*********************************************************************
*
*             Global functions
*
**********************************************************************

  Functions here are global, although their names indicate a local
  scope. They should not be called by user application.
*/
#define  _MMC_LB_Read(Sector,pBuffer)  MMC_ReadBlock(Sector,pBuffer);

int MMC_LB_Init() {

//    dprintf("MMC Init\r\n");
	
	_MMC_LB_InitCache();
	MMC_Initialize(MMC_4BIT_MODE);	

	return -1;
}
int MMC_LB_FLASHCACHE() {

	//printf("mmc flash!\r\n");
	_MMC_LB_ClearCache();

	return 0;
}
int MMC_LB_Deinit()
{
	MMC_LB_FLASHCACHE();
}
int MMC_LB_MultiRead(unsigned int Sector, void *pBuffer,unsigned int SectorCount) {
	_MMC_LB_FLUSHCACHES(Sector,Sector+SectorCount);
	return MMC_ReadMultiBlock(Sector,SectorCount,pBuffer);
}
	
int MMC_LB_Read(unsigned int Sector, void *pBuffer) {


	int x;
	//printf("LB_Read = %x %x\r\n",Sector,pBuffer);
	
	if(_MMC_LB_GetFromCache(Sector,pBuffer))
	{
		x = _MMC_LB_Read(Sector,pBuffer);
//		printf("LB_Read = %x %x,ret = %d\r\n",Sector,pBuffer,x);
		if(x == 0)
		_MMC_LB_CopyToCache(Sector,pBuffer,0);
		return x;
	}
	return 0;
}

int MMC_LB_MultiWrite(unsigned int Sector, void *pBuffer,unsigned int SectorCount) {
  
	_MMC_LB_CloseCACHES(Sector,Sector + SectorCount);
	return MMC_WriteMultiBlock(Sector,SectorCount,pBuffer);
}


int MMC_LB_Write(unsigned int Sector, void *pBuffer) {
	int x;
	//printf("LB_Write = %x %x\r\n",Sector,pBuffer);
	if(_MMC_LB_UpdateInCache(Sector,pBuffer))
	{
		_MMC_LB_CopyToCache(Sector,pBuffer,1);
		return 0;
	}
	return 0;
}
