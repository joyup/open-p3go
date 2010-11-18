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
 *  Author:  <dsqiu@ingenic.cn>  <zzhng@ingenic.cn> 
 *
 *  Create:   2008-10-10, by zhang zhi
 *            
 *            
 *
 *******************************************************************************
 */
 

/*
 * Include file for Ingenic Semiconductor's JZ4740 CPU.
 */
#ifndef __JZ_MIPS_H__
#define __JZ_MIPS_H__

//#ifndef __ASSEMBLY__
#include <bsp.h>
#include <types.h>
#include <sysdefs.h>
#include <cacheops.h>

#define cache_unroll(base,op)	        	\
	__asm__ __volatile__("	         	\
		.set noreorder;		        \
		.set mips3;		        \
		cache %1, (%0);	                \
		.set mips0;			\
		.set reorder"			\
		:				\
		: "r" (base),			\
		  "i" (op));

static inline void jz_flush_dcache(void)
{
	unsigned long start;
	unsigned long end;

	start = KSEG0BASE;
	end = start + CFG_DCACHE_SIZE;
	while (start < end) {
		cache_unroll(start,Index_Writeback_Inv_D);
		start += CFG_CACHELINE_SIZE;
	}
}

static inline void jz_flush_icache(void)
{
	unsigned long start;
	unsigned long end;

	start = KSEG0BASE;
	end = start + CFG_ICACHE_SIZE;
	while(start < end) {
		cache_unroll(start,Index_Invalidate_I);
		start += CFG_CACHELINE_SIZE;
	}
}

/* cpu pipeline flush */

static inline void jz_sync(void)
{
	__asm__ volatile ("sync");
}

static inline void jz_writeb(unsigned int address, unsigned char value)
{
	*((volatile unsigned char *)address) = value;
}

static inline void jz_writew(unsigned int address, unsigned short value)
{
	*((volatile unsigned short *)address) = value;
}

static inline void jz_writel(unsigned int address, unsigned int value)
{
	*((volatile unsigned int *)address) = value;
}

static inline unsigned char jz_readb(unsigned int address)
{
	return *((volatile unsigned char *)address);
}

static inline unsigned short jz_readw(unsigned int address)
{
	return *((volatile unsigned short *)address);
}

static inline unsigned int jz_readl(unsigned int address)
{
	return *((volatile unsigned int *)address);
}

#endif /* __JZ_MIPS_H__ */
