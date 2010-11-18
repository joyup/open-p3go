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


#include<stdio.h>
#include <os_api.h>

#define MEM_U8 unsigned char 
#define MEM_ULONG unsigned int 



#define TRUE		1
#define FALSE		0

#define FREE		0
#define RESERVED	1
static const char *statStr[] = {"FREE","RESERVED"};

/* Memory block header = 9 bytes = 4 previous + 4 next + status */

#define SIZE_HEADER	16

//#define prev(i)		*((MEM_ULONG *)&memory[i])
#define prev(i)		(*((MEM_ULONG *)(i)))
#define next(i)		(*((MEM_ULONG *) (i+4)))
#define status(i)	(*((MEM_U8 *) (i+8)))
#define setalign(i,y)  (*((MEM_U8 *)(i-1))) = y
#define getalign(i)  (*((MEM_U8 *)(i-1)))

#define size(i)		(next(i)-i-SIZE_HEADER)

/* if going to split free block, need at least 8 bytes in new free part */

#define MIN_FREE_BYTES   4
#if JZ4740_PMP16
static MEM_U8 *memory = (MEM_U8 *)DRVMEMADDR;
#define MEM_LENGHT DRVMEMLEN
#else
#define MEM_LENGHT (1024*1024*4)
static MEM_U8 memory[MEM_LENGHT] __attribute__ ((aligned (32)));
#endif

static MEM_ULONG first = 0; /*stores address of first byte of heap*/

#if JZ4740_PMP16
static MEM_ULONG last = DRVMEMLEN; /*store address of last byte of heap + 1*/
#else
static MEM_ULONG last = MEM_LENGHT; /*store address of last byte of heap + 1*/
#endif

#define DEBUG_DRV_MEM 1
#if DEBUG_DRV_MEM
static MEM_U8* cur_max_mem;
#endif


#if DEBUG_DRV_MEM
#define max_mem_status(x)                                      \
do{	                                                           \
	    if((int)( (unsigned int)(x) - (unsigned int)cur_max_mem) > 0)                          \
		{                                                      \
			 printf("drv_mem cur max using = %d\n", (unsigned int)(x) - (unsigned int)memory); \
			 cur_max_mem = (MEM_U8 *)(x);                                  \
		}                                                      \
}while(0)
#else
#define max_mem_status(x) 
#endif
void heapInit()
{
	cur_max_mem = memory;
	first = (MEM_ULONG)&memory[0];
	last = (MEM_ULONG)&memory[MEM_LENGHT - SIZE_HEADER];
	
	prev(first)=0;
	next(first)=last;
	status(first)=FREE;

	prev(last) = first;
	next(last) = 0;
	status(last)=RESERVED;
	
	
	return;
}/*end heapInit*/

static int currentNodeAlloc(MEM_ULONG i,MEM_ULONG nbytes)
{
	MEM_ULONG free_size;

	/*handle case of current block being the last*/
  //printf("currentNodeAlloc addr = 0x%08x size = 0x%x\r\n",i,nbytes);

 	if(next(i) == 0){
		free_size = last - i - SIZE_HEADER;
	}else{
	    free_size = size(i);
	}
    //printf("currentNodeAlloc freesize = 0x%x\r\n", free_size);
	
	/*either split current block, use entire current block, or fail*/
    
	if(free_size >= nbytes + SIZE_HEADER + MIN_FREE_BYTES)
	{
		MEM_ULONG old_next;
		MEM_ULONG old_block;
		MEM_ULONG new_block;

		old_next = next(i);
		old_block = i;

		/*fix up original block*/

		next(i) = i+SIZE_HEADER+nbytes;	
		new_block = next(i);
		status(i)=RESERVED;

		/*set up new free block*/

		i = next(i);					
		next(i)=old_next;
		prev(i)=old_block;
		status(i)=FREE;

		/*right nieghbor must point to new free block*/

		if(next(i)!=0)			
		{
			i = next(i);
			prev(i)=new_block;
		}
		
		return(TRUE);
	}
	else if(free_size >= nbytes)
	{
		status(i)=RESERVED;
		return(TRUE);
	}

	return(FALSE);

}/*end currentNodeAlloc*/
static MEM_ULONG my_alloc(MEM_ULONG nbytes)
{
	int ret;
	MEM_ULONG i;
 //printf("alloc %x\n",nbytes);
	nbytes = ((nbytes  + MIN_FREE_BYTES - 1)/ MIN_FREE_BYTES )  * MIN_FREE_BYTES;
	i=first;
	if(status(i)==FREE)
	{
		ret = currentNodeAlloc(i,nbytes);
		if(ret==TRUE)
		{
			max_mem_status(i + SIZE_HEADER + nbytes );
			//printf("alloc mem = 0x%x,size = %d\n",i+SIZE_HEADER,nbytes); 
			return(i+SIZE_HEADER);
		}
	}

	while(next(i)!=0)
	{
		i=next(i);
		if(status(i)==FREE)
		{
			ret = currentNodeAlloc(i,nbytes);
			if(ret==TRUE)
			{
				//printf("alloc mem = 0x%x,size = %d\n",i+SIZE_HEADER,nbytes); 
				max_mem_status(i + SIZE_HEADER + nbytes );
				return(i+SIZE_HEADER);
			}
		}
	}
	return 0;
}
MEM_ULONG alloc(MEM_ULONG nbytes)
{
	unsigned int sr;
	sr = spin_lock_irqsave();
	MEM_ULONG i = my_alloc(nbytes);
	if(i != 0)
		setalign(i,0);  
	spin_unlock_irqrestore(sr);
	return i;
}/*end alloc*/

MEM_ULONG alignAlloc(MEM_ULONG align,MEM_ULONG size)
{
	unsigned int sr;
	sr = spin_lock_irqsave();
	MEM_ULONG i2 = 0;
	MEM_ULONG i = my_alloc(size + align);
	if(i != 0)
	{
		i2 = (i +  align - 1) & ~(align - 1);
		setalign(i2, i2 - i);    
	}
	spin_unlock_irqrestore(sr);
	return i2;
}/*end alloc*/

static void my_free(MEM_ULONG address)
{
	MEM_ULONG block;
	MEM_ULONG lblock;
	MEM_ULONG rblock;
	block = address-SIZE_HEADER;
	lblock = prev(block);
	rblock = next(block);

	/*
	4 cases: FFF->F, FFR->FR, RFF->RF, RFR 
	always want to merge free blocks 
	*/

	if((lblock!=0)&&(rblock!=0)&&(status(lblock)==FREE)&&(status(rblock)==FREE))
	{
		next(lblock)=next(rblock);
		status(lblock)=FREE;
		if(next(rblock)!=0){ prev(next(rblock))=lblock; }
	}
	else if((lblock!=0)&&(status(lblock)==FREE))
	{
		next(lblock)=next(block);
		status(lblock)=FREE;
		if(next(block)!=0){ prev(next(block))=lblock; }
	}
	else if((rblock!=0)&&(status(rblock)==FREE))
	{
		next(block)=next(rblock);
		status(block)=FREE;
		if(next(rblock)!=0){ prev(next(rblock))=block; }
	}
	else{ status(block)=FREE; }
	
}
/*Note: disaster will strike if fed wrong address*/

void deAlloc(MEM_ULONG address)
{
	
    unsigned int sr;
	sr = spin_lock_irqsave();
	address -= getalign(address);
	my_free(address);
	spin_unlock_irqrestore(sr);
}/*end deAlloc*/


MEM_ULONG Drv_realloc(MEM_ULONG address,MEM_ULONG nbytes)
{
	unsigned int sr;
	sr = spin_lock_irqsave();
	MEM_ULONG rr,addr,oldsize;
	MEM_ULONG block,rblock,rrblock;
	MEM_ULONG bsize,rbsize,align;
	unsigned int len;
	oldsize = nbytes;
	
	nbytes = ((nbytes  + MIN_FREE_BYTES - 1)/ MIN_FREE_BYTES )  * MIN_FREE_BYTES;
	
	rr = address;
	if (nbytes == 0) {
			spin_unlock_irqrestore(sr);
			deAlloc(rr);
			return 0;
  }
	if (address == 0)
	{	
	    addr = my_alloc(nbytes);
	    if(addr != 0)
	    	setalign(addr,0);
	    spin_unlock_irqrestore(sr);
	    return addr;
	}
	align = getalign(address);
	len = (nbytes + align + MIN_FREE_BYTES - 1) &(~(MIN_FREE_BYTES - 1));
	address -= getalign(address);
	address -= SIZE_HEADER;    
	bsize = size(address);
	//printf("align = %d,address = %x  %d %d\n",align,address,nbytes,bsize);
	if(nbytes <= bsize-align)
	{
		spin_unlock_irqrestore(sr);
		return rr;
	}	
	
	rblock = next(address);
	if((rblock != 0) &&(status(rblock) == FREE) )
	{
		//printf("rblock = %x %d %d",rblock,status(rblock),size(rblock));
		bsize += size(rblock);
		if(bsize >= nbytes + align)
		{
			rrblock = next(rblock);
			next(address) = address + len + SIZE_HEADER;
			block = next(address);
			prev(block) = address;
			next(block) = rrblock;
			if(rrblock)
				prev(rrblock) = block; 
			status(block) = FREE;
			spin_unlock_irqrestore(sr);
			return rr;
	  }
	}   
	addr = my_alloc(len);
	//printf("realloc %x %x %x %x\n",addr,rr,nbytes,bsize);
	
	if(addr == 0)
	{
		spin_unlock_irqrestore(sr);
		return 0;
	}
  addr += align;		
	setalign(addr,align);	
	memcpy(addr,rr,bsize);
	spin_unlock_irqrestore(sr);
	deAlloc(rr);
  
  return addr;	
}
MEM_ULONG Drv_calloc(MEM_ULONG size,MEM_ULONG n)
{
	MEM_ULONG rr;
	unsigned int sr;
	sr = spin_lock_irqsave();
	rr = my_alloc(size * n);
	if(rr != 0)
	{
		setalign(rr,0);
		memset((void *)rr,0,size * n);
	}
	spin_unlock_irqrestore(sr);
	return rr;
}

