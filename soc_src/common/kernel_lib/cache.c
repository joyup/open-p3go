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
 
 
#define u32	unsigned int

#define Index_Invalidate_I      0x00
#define Index_Writeback_Inv_D   0x01
#define Index_Load_Tag_I	0x04
#define Index_Load_Tag_D	0x05
#define Index_Store_Tag_I	0x08
#define Index_Store_Tag_D	0x09
#define Hit_Invalidate_I	0x10
#define Hit_Invalidate_D	0x11
#define Hit_Writeback_Inv_D	0x15
#define Hit_Writeback_I		0x18

#define Hit_Writeback_D		0x19

#define CACHE_SIZE		16*1024
#define CACHE_LINE_SIZE		32
#define KSEG0			0x80000000

#define K0_TO_K1()				\
do {						\
	unsigned long __k0_addr;		\
						\
	__asm__ __volatile__(			\
	"la %0, 1f\n\t"				\
	"or	%0, %0, %1\n\t"			\
	"jr	%0\n\t"				\
	"nop\n\t"				\
	"1: nop\n"				\
	: "=&r"(__k0_addr)			\
	: "r" (0x20000000) );			\
} while(0)

#define K1_TO_K0()				\
do {						\
	unsigned long __k0_addr;		\
	__asm__ __volatile__(			\
	"nop;nop;nop;nop;nop;nop;nop\n\t"	\
	"la %0, 1f\n\t"				\
	"jr	%0\n\t"				\
	"nop\n\t"				\
	"1:	nop\n"				\
	: "=&r" (__k0_addr));			\
} while (0)

#define INVALIDATE_BTB()			\
do {						\
	unsigned long tmp;			\
	__asm__ __volatile__(			\
	".set mips32\n\t"			\
	"mfc0 %0, $16, 7\n\t"			\
	"nop\n\t"				\
	"ori %0, 2\n\t"				\
	"mtc0 %0, $16, 7\n\t"			\
	"nop\n\t"				\
	".set mips2\n\t"			\
	: "=&r" (tmp));				\
} while (0)

#define SYNC_WB() __asm__ __volatile__ ("sync")

#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	noreorder		\n"			\
	"	.set	mips32\n\t		\n"			\
	"	cache	%0, %1			\n"			\
	"	.set	mips0			\n"			\
	"	.set	reorder"					\
	:										\
	: "i" (op), "m" (*(unsigned char *)(addr)))

void __flush_dcache_line(unsigned long addr)
{
	cache_op(Hit_Writeback_Inv_D, addr);
	SYNC_WB();
}

void __icache_invalidate_all(void)
{
	u32 i;

	K0_TO_K1();

	asm volatile (".set noreorder\n"
		      ".set mips32\n\t"
		      "mtc0\t$0,$28\n\t"
		      "mtc0\t$0,$29\n"
		      ".set mips0\n"
		      ".set reorder\n");
	for (i=KSEG0;i<KSEG0+CACHE_SIZE;i+=CACHE_LINE_SIZE)
		cache_op(Index_Store_Tag_I, i);

	K1_TO_K0();

	INVALIDATE_BTB();
}

void __dcache_invalidate_all(void)
{
	u32 i;

	asm volatile (".set noreorder\n"
		      ".set mips32\n\t"
		      "mtc0\t$0,$28\n\t"
		      "mtc0\t$0,$29\n"
		      ".set mips0\n"
		      ".set reorder\n");
	for (i=KSEG0;i<KSEG0+CACHE_SIZE;i+=CACHE_LINE_SIZE)
		cache_op(Index_Store_Tag_D, i);
}

void __dcache_writeback_all(void)
{
	u32 i;
	for (i=KSEG0;i<KSEG0+CACHE_SIZE;i+=CACHE_LINE_SIZE)
		cache_op(Index_Writeback_Inv_D, i);
	SYNC_WB();
}
void __dcache_hitinv(unsigned int addr,unsigned int size)
{
	unsigned long dc_lsize = CACHE_LINE_SIZE;
	unsigned long end, a;

	a = addr & ~(dc_lsize - 1);
	end = (addr + size - 1) & ~(dc_lsize - 1);
	while (1) {
		cache_op(Hit_Invalidate_D, a);
		if (a == end)
			break;
		a += dc_lsize;
	}
	SYNC_WB();
}       
void __dcache_hitwback(unsigned long addr, unsigned long size)
{
	unsigned long dc_lsize = CACHE_LINE_SIZE;
	unsigned long end, a;
	
	a = addr & ~(dc_lsize - 1);
	end = (addr + size - 1) & ~(dc_lsize - 1);
	while (1) {
		__flush_dcache_line(a);	/* Hit_Writeback_Inv_D */
		if (a == end)
			break;
		a += dc_lsize;
	}	
}
void dma_cache_wback_inv(unsigned long addr, unsigned long size)
{
	unsigned long end, a;

	if (size >= CACHE_SIZE) {
		__dcache_writeback_all();
	}
	else {
		unsigned long dc_lsize = CACHE_LINE_SIZE;

		a = addr & ~(dc_lsize - 1);
		end = (addr + size - 1) & ~(dc_lsize - 1);
		while (1) {
			__flush_dcache_line(a);	/* Hit_Writeback_Inv_D */
			if (a == end)
				break;
			a += dc_lsize;
		}
	}
}
