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


#include <bsp.h>
#include <header.h>

#if BACHMARK
	#define TESTWRITE  1
	#define TESTREAD	 0
	#undef printf
	unsigned long	ullbTimeStart = 0,ullbTimeEnd = 0;
	extern unsigned long	ulssfdcTimeStart;
	unsigned long g_ulLBTotalRead = 0;
	unsigned long g_ulLBTotalWrite = 0;
	
	extern unsigned long g_ulSSFDCTotalRead;
	extern unsigned long g_ulSSFDCTotalWrite;
	extern unsigned long g_ulJZTotalRead;
	extern unsigned long g_ulJZTotalWrite;
#endif

#define	BACHMARKINIT	0
#if BACHMARKINIT
	#undef printf
	unsigned long	ullbTimeStart = 0,ullbTimeEnd = 0;
	unsigned long	g_ulLBTotalInit = 0;
#endif

#define CACHE_MAX_NUM 32
#define SECTOR_SIZE 4096
typedef struct {
	unsigned short  CacheState;
	unsigned short  UseCount;
	unsigned short  CacheChange;
	unsigned short  CacheReserve;	
	unsigned int BlockId;
    unsigned char   *aBlockData;
} SSFDC__LB_CACHE;

#define FREE_CACHE 0
#define PREWRITE_CACHE 2
#define OFTEN_USE_CACHE 3
#define CACHE_TO_UNCATCH(x) ((unsigned int)x | 0xa0000000)
static unsigned int __aBlockData[SECTOR_SIZE * CACHE_MAX_NUM / 4] __attribute__ ((aligned (32)));
//static unsigned char *__aBlockData = NULL;
static SSFDC__LB_CACHE ssfdc_cache[CACHE_MAX_NUM];
static unsigned short Cur_CacheCount = 0;
static unsigned int total_sec  = 0;
extern unsigned int g_persector_size;

#define DMA_ENABLE 1

#if DMA_ENABLE
	#define lb_memcpy dma_nand_copy_wait
#else
#define lb_memcpy memcpy
#endif

static void _NAND_LB_InitCache()
{
	int i;
	SSFDC__LB_CACHE *pCache = ssfdc_cache;
	//__aBlockData = (unsigned char *)alignAlloc(32,CACHE_MAX_NUM * g_persector_size * sizeof(unsigned char));
	unsigned char * ptr = (unsigned char *)CACHE_TO_UNCATCH(__aBlockData);
	for(i = 0;i < CACHE_MAX_NUM;i++)
	{
		pCache->CacheState = FREE_CACHE;
		pCache->UseCount = 0;
		pCache->CacheChange = 0;
		pCache->aBlockData = ptr;
		ptr+=g_persector_size;
		pCache++;
	}
	Cur_CacheCount = 0;
}

static  SSFDC__LB_CACHE * _NAND_LB_GetFreeCache()
{
	int i,ret = 0;
	SSFDC__LB_CACHE *pCacheInfo = &ssfdc_cache[Cur_CacheCount];
	while(1)
	{
    if(ret >= CACHE_MAX_NUM)
			return 0;
		if(pCacheInfo >= &ssfdc_cache[CACHE_MAX_NUM])
		{
			pCacheInfo = ssfdc_cache;
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
#define _NAND_LB_Write(pCache) ssfdc_nftl_write(pCache->BlockId,pCache->aBlockData,1)

static void _NAND_LB_CloseCACHES(unsigned int sectorstart,unsigned int sectorend)
{
	unsigned int i;
	SSFDC__LB_CACHE *pCache = ssfdc_cache;
	for( i = 0;i < CACHE_MAX_NUM;i++)
	{
		if((pCache->CacheState != FREE_CACHE) && (pCache->BlockId >= sectorstart) && (pCache->BlockId < sectorend))
		{
			pCache->CacheChange = 0;
			pCache->CacheState = FREE_CACHE;			    
			pCache->UseCount = 0;
		}
		pCache++;
    
	}    
    
}

static void _NAND_LB_FLUSHCACHES(unsigned int sectorstart,unsigned int sectorend)
{
	unsigned int i;
	SSFDC__LB_CACHE *pCache = ssfdc_cache;
	for( i = 0;i < CACHE_MAX_NUM;i++)
	{
		if((pCache->CacheState != FREE_CACHE) && (pCache->BlockId >= sectorstart) && (pCache->BlockId < sectorend))
		{
			if(pCache->CacheChange)
				_NAND_LB_Write(pCache);
			pCache->CacheChange = 0;
			pCache->CacheState = FREE_CACHE;			    
			pCache->UseCount = 0;
		}
		pCache++;
    
	}
    
}
inline static int Get_NAND_CacheFreeCount()
{
	SSFDC__LB_CACHE *pCache = ssfdc_cache;
	SSFDC__LB_CACHE *pEndCache = &ssfdc_cache[CACHE_MAX_NUM];
	unsigned int count = 0;
	while(pCache < pEndCache)
	{
		if(pCache->CacheState == FREE_CACHE)
			count++;
		pCache++;	
	}
	return count;
	
}
static unsigned int _NAND_LB_PreWiteToNand(SSFDC__LB_CACHE *pCache,unsigned short *count,unsigned int update)
{
	SSFDC__LB_CACHE *pWriteCache;
	//SSFDC__LB_CACHE *pOldCache = pCache;
	SSFDC__LB_CACHE *pEndCache = &ssfdc_cache[CACHE_MAX_NUM];
	unsigned int sector = -1;
	unsigned int flag;
	while(1)
	{
		sector = -1;
		flag = 0;
		pWriteCache = ssfdc_cache;
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
				_NAND_LB_Write(pCache);
				pCache->CacheChange = 0;
			}
			pCache->CacheState = FREE_CACHE;			    
			pCache->UseCount = 0;
			(*count)++;
	 }else
	 	   break;		
	}
}
static void _NAND_LB_OftenToNand(SSFDC__LB_CACHE *pCache,unsigned short *count,unsigned int update)
{
	//printf("%s!\n",__FUNCTION__);
	SSFDC__LB_CACHE *pWriteCache = pCache;
	SSFDC__LB_CACHE *pOldCache = pCache;
	SSFDC__LB_CACHE *pEndCache = &ssfdc_cache[CACHE_MAX_NUM];
	
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
			_NAND_LB_Write(pWriteCache);
		//if(update == 1)
		pWriteCache->CacheState = FREE_CACHE;			    
			
		pWriteCache->UseCount = 0;
		pWriteCache->CacheChange = 0;
		if(update != -1)
			update--;
		if(update != 0)
			_NAND_LB_OftenToNand(pOldCache,count,update);
		
	}
}

static int _NAND_LB_FreeCache(unsigned int update)
{
	unsigned short freecount = 0,totalfree = 0;
    
	freecount = 0;
	
	_NAND_LB_PreWiteToNand(ssfdc_cache,&freecount,PREWRITE_CACHE);
    
	totalfree += freecount;
	
	if(freecount == 0)
	{
		freecount = 0;	
		_NAND_LB_PreWiteToNand(ssfdc_cache,&freecount,OFTEN_USE_CACHE);
		totalfree += freecount;
		update = 0;
	}	
	if(update)
	{	
		if(Get_NAND_CacheFreeCount() < CACHE_MAX_NUM * 1 / 4) // because fat is 4 sector
		{
			freecount = 0;	
			_NAND_LB_PreWiteToNand(ssfdc_cache,&freecount,OFTEN_USE_CACHE);
			totalfree += freecount;
		}
  	}
  
	//printf("Free = %d\r\n",totalfree);
	return totalfree;
}

static int _NAND_LB_GetFromCache(unsigned int Sector, void *pBuffer) 
{

	SSFDC__LB_CACHE *pCache = &ssfdc_cache[Cur_CacheCount];
	SSFDC__LB_CACHE *pUseCache = 0;
	unsigned short i;
	//printf("LB cache sector = %x pBuffer = %x\n",Sector,pBuffer);
	if(pCache >= &ssfdc_cache[CACHE_MAX_NUM])
		pCache = ssfdc_cache;

	i = 0;
	while (1) 
	{	
		if(pCache->CacheState != FREE_CACHE)
		{
			if (Sector == pCache->BlockId)  
			{
				pUseCache = pCache;
				
				pCache->UseCount++;
				if(pCache->UseCount == 0)
					pCache->UseCount = -1;
				
				pCache->CacheState = OFTEN_USE_CACHE;
				
				break; 
			}
		}
		pCache--;
		if(pCache < ssfdc_cache)
			pCache = &ssfdc_cache[CACHE_MAX_NUM - 1];
		
		i++;
		if (i >= CACHE_MAX_NUM) 
		{
			break;  /* Sector not in cache */
		}
	}
	if (pUseCache) 
	{	
		lb_memcpy(pBuffer, pUseCache->aBlockData, g_persector_size);
		return 0;
	}
	return -1;  
}

static void _NAND_LB_ClearCache() 
{
	unsigned short freecount = 0;
	
	_NAND_LB_PreWiteToNand(ssfdc_cache,&freecount,PREWRITE_CACHE);
	_NAND_LB_PreWiteToNand(ssfdc_cache,&freecount,OFTEN_USE_CACHE);
}
static void _NAND_LB_CopyToCache(unsigned int Sector, void *pBuffer,unsigned short rw) 
{
	SSFDC__LB_CACHE *pCache = _NAND_LB_GetFreeCache();
	
	if(!pCache)
	{
		_NAND_LB_FreeCache(rw);
		pCache = _NAND_LB_GetFreeCache();
	}
	pCache->BlockId = Sector;
	pCache->CacheState = PREWRITE_CACHE;
	pCache->UseCount = 0;
	pCache->CacheChange = rw;
	lb_memcpy(pCache->aBlockData,pBuffer,g_persector_size);
}

int _NAND_LB_UpdateInCache(unsigned int Sector, void *pBuffer) {
    short i,ret = 0;
	i =  Cur_CacheCount;
	if(Cur_CacheCount > CACHE_MAX_NUM)
		i = 0;
	while(1)
	{
        if(ret >= CACHE_MAX_NUM)
			return -1;
        if(ssfdc_cache[i].CacheState != FREE_CACHE)
		{       
			if(ssfdc_cache[i].BlockId == Sector)
			{
				//printf("UpdateInCache = %d\r\n",Sector);
				ssfdc_cache[i].CacheState = OFTEN_USE_CACHE;
				ssfdc_cache[i].UseCount++;
				ssfdc_cache[i].CacheChange = 1;
				
				lb_memcpy(ssfdc_cache[i].aBlockData,pBuffer,g_persector_size);
				
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

static unsigned int GetDevTotalSector()
{
	flash_info_t flashinfo;
	ssfdc_nftl_getInfo(0,&flashinfo);
	return flashinfo.dwFSTotalSectors;	

}

/*********************************************************************
*
*             Global functions
*
**********************************************************************

  Functions here are global, although their names indicate a local
  scope. They should not be called by user application.
*/


#define  _NAND_LB_Read(Sector,pBuffer)  ssfdc_nftl_read(Sector,pBuffer,1);

int NAND_LB_Init() {

	static int dInit = 0;
	#if BACHMARKINIT
		Init_PerformanceCounter();
		ullbTimeEnd = Get_PerformanceCounter();
		ullbTimeStart = ullbTimeEnd;
	#endif
	if(dInit == 0)
	{
		#if BACHMARK
			Init_PerformanceCounter();
			g_ulLBTotalRead = 0;
			g_ulLBTotalWrite = 0;
			
			g_ulSSFDCTotalRead = 0;
			g_ulSSFDCTotalWrite = 0;
			g_ulJZTotalRead = 0;
			g_ulJZTotalWrite = 0;
		#endif
		ssfdc_nftl_init();
		ssfdc_nftl_open();
		_NAND_LB_InitCache();
		total_sec=GetDevTotalSector();
		dInit = 1;
	}
	#if BACHMARKINIT
		ullbTimeEnd = Get_PerformanceCounter();
		g_ulLBTotalInit = (ullbTimeEnd - ullbTimeStart) / 6;
		printf("%s:total time = %dus\n",__FUNCTION__,g_ulLBTotalInit);
	#endif
	return -1;
}

int NAND_LB_GetInfo(pflash_info_t pflashinfo)
{
	ssfdc_nftl_getInfo(0,pflashinfo);
	return 0;
}

void TimeInit(unsigned long *ulTimeEnd,unsigned long *ulTimeStart,unsigned long *ulTimeTotal)
{
	Init_PerformanceCounter();
}

void TimeStart(unsigned long *ulTimeEnd,unsigned long *ulTimeStart)
{
	*ulTimeEnd = Get_PerformanceCounter();
	*ulTimeStart = *ulTimeEnd;
}

void TimeStop(unsigned long *ulTimeEnd,unsigned long *ulTimeStart,unsigned long *ulTimeTotal)
{
	*ulTimeEnd = Get_PerformanceCounter();
	*ulTimeTotal = (*ulTimeEnd - *ulTimeStart) / 6;
}

int NAND_LB_FLASHCACHE() 
{
	#if BACHMARK
		TimeStart(&ullbTimeEnd,&ullbTimeStart);
	#endif
	
	_NAND_LB_ClearCache();
	

	
	#if BACHMARK
		ulssfdcTimeStart = Get_PerformanceCounter();
	#endif

	ssfdc_nftl_flush_cache();

	#if BACHMARK
		TimeStop(&ullbTimeEnd,&ullbTimeStart,&g_ulLBTotalWrite);
		printf("%s,%d:total time = %dus\n",__FUNCTION__,__LINE__,g_ulLBTotalWrite);
		g_ulSSFDCTotalWrite += (ullbTimeEnd - ulssfdcTimeStart) / 6;
		#if TESTWRITE
		
		printf("lb:TotalWrite = %dus, per = %d\n",
						(g_ulLBTotalWrite - g_ulSSFDCTotalWrite),
						(g_ulLBTotalWrite - g_ulSSFDCTotalWrite) * 100 / g_ulLBTotalWrite);
		printf("ssfdc:TotalWrite = %dus, per = %d\n",
						(g_ulSSFDCTotalWrite - g_ulJZTotalWrite),
						(g_ulSSFDCTotalWrite - g_ulJZTotalWrite) * 100 / g_ulLBTotalWrite);
		printf("Jz_nand:TotalWrite = %dus, per = %d\n",
						g_ulJZTotalWrite,
						g_ulJZTotalWrite * 100 / g_ulLBTotalWrite);
		printf("TotalWrite = %dus\n",
							g_ulLBTotalWrite);
		printf("\r\n\n");
		#endif
	#endif
	
	return 0;
}
int NAND_LB_Deinit()
{
	NAND_LB_FLASHCACHE();
	/*
	if (__aBlockData)
		deAlloc(__aBlockData);*/
}

int NAND_LB_MultiRead(unsigned int Sector, void *pBuffer,unsigned int SectorCount) 
{
	int ret;
	#if BACHMARK
		ullbTimeEnd = Get_PerformanceCounter();
		ullbTimeStart = ullbTimeEnd;
	#endif
	_NAND_LB_FLUSHCACHES(Sector,Sector+SectorCount);
	ret = ssfdc_nftl_read(Sector,pBuffer,SectorCount);
	#if BACHMARK
		ullbTimeEnd = Get_PerformanceCounter();
		g_ulLBTotalRead += (ullbTimeEnd - ullbTimeStart) / 6;
		
		#if TESTREAD
		printf("lb:TotalRead = %dus, per = %d\n",
							(g_ulLBTotalRead - g_ulSSFDCTotalRead),
							(g_ulLBTotalRead - g_ulSSFDCTotalRead) * 100 / g_ulLBTotalRead);
		printf("ssfdc:TotalRead = %dus, per = %d\n",
							(g_ulSSFDCTotalRead - g_ulJZTotalRead),
							(g_ulSSFDCTotalRead - g_ulJZTotalRead) * 100 / g_ulLBTotalRead);
		printf("Jz_nand:TotalRead = %dus, per = %d\n",
							g_ulJZTotalRead,
							g_ulJZTotalRead * 100 / g_ulLBTotalRead);
							
		printf("\r\n\n");
		#endif
	#endif
	return ret;
}
	
int NAND_LB_Read(unsigned int Sector, void *pBuffer)
{
	int x;
	#if BACHMARK
		ullbTimeEnd = Get_PerformanceCounter();
		ullbTimeStart = ullbTimeEnd;
	#endif
	
	if ((unsigned long)pBuffer < 0xa0000000)
		dma_cache_wback_inv(pBuffer,g_persector_size);
	
	unsigned char *ptr = (unsigned char *)CACHE_TO_UNCATCH(pBuffer);
	if(_NAND_LB_GetFromCache(Sector,ptr))
	{	
		x =  _NAND_LB_Read(Sector,ptr);
		_NAND_LB_CopyToCache(Sector,ptr,0);
		#if BACHMARK
			ullbTimeEnd = Get_PerformanceCounter();
			g_ulLBTotalRead += (ullbTimeEnd - ullbTimeStart) / 6;
			#if TESTREAD
			printf("lb:TotalRead = %dus, per = %d\n",
							(g_ulLBTotalRead - g_ulSSFDCTotalRead),
							(g_ulLBTotalRead - g_ulSSFDCTotalRead) * 100 / g_ulLBTotalRead);
			printf("ssfdc:TotalRead = %dus, per = %d\n",
							(g_ulSSFDCTotalRead - g_ulJZTotalRead),
							(g_ulSSFDCTotalRead - g_ulJZTotalRead) * 100 / g_ulLBTotalRead);
			printf("Jz_nand:TotalRead = %dus, per = %d\n",
							g_ulJZTotalRead,
							g_ulJZTotalRead * 100 / g_ulLBTotalRead);
							
			printf("TotalRead = %dus\n",
							g_ulLBTotalRead);
							
			printf("\r\n\n");
			#endif
		#endif
		return x;
	}
	#if BACHMARK
			ullbTimeEnd = Get_PerformanceCounter();
			g_ulLBTotalRead += (ullbTimeEnd - ullbTimeStart) / 6;
			#if TESTREAD
			printf("lb:TotalRead = %dus, per = %d\n",
							(g_ulLBTotalRead - g_ulSSFDCTotalRead),
							(g_ulLBTotalRead - g_ulSSFDCTotalRead) * 100 / g_ulLBTotalRead);
			printf("ssfdc:TotalRead = %dus, per = %d\n",
							(g_ulSSFDCTotalRead - g_ulJZTotalRead),
							(g_ulSSFDCTotalRead - g_ulJZTotalRead) * 100 / g_ulLBTotalRead);
			printf("Jz_nand:TotalRead = %dus, per = %d\n",
							g_ulJZTotalRead,
							g_ulJZTotalRead * 100 / g_ulLBTotalRead);
							
			printf("TotalRead = %dus\n",
							g_ulLBTotalRead);
							
			printf("\r\n\n");
			#endif
	#endif
	return g_persector_size;
}

int NAND_LB_MultiWrite(unsigned int Sector, void *pBuffer,unsigned int SectorCount) 
{
	int ret;
 	#if BACHMARK
		ullbTimeEnd = Get_PerformanceCounter();
		ullbTimeStart = ullbTimeEnd;
	#endif
	if((Sector + SectorCount) > total_sec)
	{
		printf("Error: sector number + SectorCount > Total sector number!\n");
		return -2;
	}
	_NAND_LB_CloseCACHES(Sector,Sector + SectorCount);
	ret = ssfdc_nftl_write(Sector,pBuffer,SectorCount);
	
	#if BACHMARK
		ullbTimeEnd = Get_PerformanceCounter();
		g_ulLBTotalWrite += (ullbTimeEnd - ullbTimeStart) / 6;
		#if TESTWRITE
		printf("lb:TotalWrite = %dus, per = %d\n",
						(g_ulLBTotalWrite - g_ulSSFDCTotalWrite),
						(g_ulLBTotalWrite - g_ulSSFDCTotalWrite) * 100 / g_ulLBTotalWrite);
		printf("ssfdc:TotalWrite = %dus, per = %d\n",
						(g_ulSSFDCTotalWrite - g_ulJZTotalWrite),
						(g_ulSSFDCTotalWrite - g_ulJZTotalWrite) * 100 / g_ulLBTotalWrite);
		printf("Jz_nand:TotalWrite = %dus, per = %d\n",
						g_ulJZTotalWrite,
						g_ulJZTotalWrite * 100 / g_ulLBTotalWrite);
		printf("TotalWrite = %dus\n",
							g_ulLBTotalWrite);
		printf("\r\n\n");
		#endif
	#endif
	
	return ret;
}


int NAND_LB_Write(unsigned int Sector, void *pBuffer)
{	
	#if BACHMARK
		ullbTimeEnd = Get_PerformanceCounter();
		ullbTimeStart = ullbTimeEnd;
	#endif
	if( Sector >= total_sec)
	{
		printf("Error: sector number > Total sector number!\n");
		return -1;
	}
	
	if ((unsigned long)pBuffer < 0xa0000000)
		dma_cache_wback_inv(pBuffer,g_persector_size);
	
	unsigned char *ptr = (unsigned char *)CACHE_TO_UNCATCH(pBuffer);
	if(_NAND_LB_UpdateInCache(Sector,ptr))
	{
		_NAND_LB_CopyToCache(Sector,ptr,1);
		#if BACHMARK
			ullbTimeEnd = Get_PerformanceCounter();
			g_ulLBTotalWrite += (ullbTimeEnd - ullbTimeStart) / 6;
			#if TESTWRITE
			printf("lb:TotalWrite = %dus, per = %d\n",
							(g_ulLBTotalWrite - g_ulSSFDCTotalWrite),
							(g_ulLBTotalWrite - g_ulSSFDCTotalWrite) * 100 / g_ulLBTotalWrite);
			printf("ssfdc:TotalWrite = %dus, per = %d\n",
							(g_ulSSFDCTotalWrite - g_ulJZTotalWrite),
							(g_ulSSFDCTotalWrite - g_ulJZTotalWrite) * 100 / g_ulLBTotalWrite);
			printf("Jz_nand:TotalWrite = %dus, per = %d\n",
							g_ulJZTotalWrite,
							g_ulJZTotalWrite * 100 / g_ulLBTotalWrite);
			
			printf("TotalWrite = %dus\n",
							g_ulLBTotalWrite);
			
			printf("\r\n\n");
			#endif
		#endif
		return g_persector_size;
		
	}
	#if BACHMARK
			ullbTimeEnd = Get_PerformanceCounter();
			g_ulLBTotalWrite += (ullbTimeEnd - ullbTimeStart) / 6;
			#if TESTWRITE
			printf("lb:TotalWrite = %dus, per = %d\n",
							(g_ulLBTotalWrite - g_ulSSFDCTotalWrite),
							(g_ulLBTotalWrite - g_ulSSFDCTotalWrite) * 100 / g_ulLBTotalWrite);
			printf("ssfdc:TotalWrite = %dus, per = %d\n",
							(g_ulSSFDCTotalWrite - g_ulJZTotalWrite),
							(g_ulSSFDCTotalWrite - g_ulJZTotalWrite) * 100 / g_ulLBTotalWrite);
			printf("Jz_nand:TotalWrite = %dus, per = %d\n",
							g_ulJZTotalWrite,
							g_ulJZTotalWrite * 100 / g_ulLBTotalWrite);
			printf("TotalWrite = %dus\n",
							g_ulLBTotalWrite);
			printf("\r\n\n");
			#endif
	#endif
	return g_persector_size;
}
