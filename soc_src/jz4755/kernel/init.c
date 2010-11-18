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

#include <archdefs.h>
#include <mipsregs.h>
#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>
unsigned int g_stack[2049];

extern void except_common_entry(void);

void consol_send_ch(unsigned char ch)
{
	serial_putc(ch);
}

void consol_get_char(unsigned char *ch)
{
	int r;
	r = serial_getc();
	if (r > 0)
		*ch = (unsigned char)r;
	else
		*ch = 0;
}

typedef void (*pfunc)(void);

extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];

void c_main(void)
{
	pfunc *p;
	write_c0_status(0x10000400);
	
	memcpy((void *)A_K0BASE, except_common_entry, 0x20);
	memcpy((void *)(A_K0BASE + 0x180), except_common_entry, 0x20);
	memcpy((void *)(A_K0BASE + 0x200), except_common_entry, 0x20);

	__dcache_writeback_all();
	__icache_invalidate_all();
	serial_init();
	
	intc_init();
	CLRREG32(A_CPM_CLKGR, 0xFFFFFFFF);
	
	
	sys_pll_init(0);
	//.word	0b01110000000000110000010000101111	#S32I2M XR16,$3
	enable_jz_mxu();
	printf("\n OSC_CLK = %d\n PLL_CLK = %d\n CPU_CLK = %d\r\n",
			OSC_CLOCK, GetCurrentPLLClock(0), GetCommonClock(CPM_CDIV));
	gpio_init();
	#if 0
	int i;
	//__asm__ __volatile__ (".long 0x7000003f");
	__write_32bit_c0_register($5, 0, 0);
	printf("sel 3\n");
	for(i = 0; i < 32 ;i ++)
	{
		__write_32bit_c0_register($10, 0, (0xc0000 + i*4 )<< 12);
		__write_32bit_c0_register($0, 0, i);
		__asm__ __volatile__ ("TLBP");
		if(read_c0_index() < 0)
		{
			printf("no match!\n");
			serial_waitfinish();
			__write_32bit_c0_register($0, 0, i);
		}
		printf("sel 0\n");
		serial_waitfinish();
		__write_32bit_c0_register($2, 0, 0);
		printf("sel 1\n");
		serial_waitfinish();
		__write_32bit_c0_register($3, 0, 0);
		printf("sel 2\n");
		serial_waitfinish();
		__write_32bit_c0_register($0, 0, i);
		__asm__ __volatile__ ("TLBWI");
		printf("TLBWI\n");
		serial_waitfinish();
	}
	#endif

#if 0
	//设置总线优先级
	OUTREG32(A_AHB_RPIOR & 0x9FFFFFFF, 1);
	OUTREG32(A_AHB_CLKL & 0x9FFFFFFF, 0);
	OUTREG32(A_AHB_EVENT0L & 0x9FFFFFFF, 0x00000000);
	OUTREG32(A_AHB_EVENT1L & 0x9FFFFFFF, 0x00000000);
	OUTREG32(A_AHB_EVENTTH & 0x9FFFFFFF, 0x00000000);
	OUTREG32((A_AHB_EVENTTH + 0x4) & 0x9FFFFFFF, 0x00000000);
	dma_cache_wback_inv((unsigned int)0x93000000, 32);
#endif
	extern OSBTaskBSPHook jz4755Bsp;
	os_SetBspInterface(&jz4755Bsp);
	os_Init();

    /* Invoke constroctor functions when needed. */
#if 1
	for (p=&__CTOR_END__[-1]; p >= __CTOR_LIST__; p--)
	{
		printf("create class %08x\n",p);

		(*p)();
    }

#endif

#ifdef MOBILE_TV
	Mobile_TV_Main();
#else
	APP_vMain();
#endif
	while(1);
}

