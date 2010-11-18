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


#include <mips.h>

	.text

	.extern c_main

	.global	_start
	.type	_start,@function
	.set	noreorder
	.set	noat
_start:
    
    la	ra, _start
	//----------------------------------------------------
	// init cp0 registers.
	//----------------------------------------------------
	mtc0	zero, C0_WATCHLO
	mtc0	zero, C0_WATCHHI
	li	t1, 0x00800000
	mtc0	t1, C0_CAUSE

    li	    t0, 0x0040FC00
	mtc0	t0, C0_STATUS
	
	//----------------------------------------------------
	// init caches, assumes a 4way*128set*32byte i/d cache
	//----------------------------------------------------
	li	t0, 3		// enable cache for kseg0 accesses
	mtc0	t0, $16		// CONFIG reg
	la	t0, 0x80000000	// an idx op should use a unmappable address
	ori	t1, t0, 0x4000	// 16kB cache
	mtc0	zero, $28	// TAGLO reg
	mtc0	zero, $29	// TAGHI reg

_init_cache_loop:	
	cache	0x8, 0(t0)	// index store icache tag
	cache	0x9, 0(t0)	// index store dcache tag
	bne	t0, t1, _init_cache_loop
	addiu	t0, t0, 0x20	// 32 bytes per cache line
	nop

	//----------------------------------------------------
	// Invalidate BTB
	//----------------------------------------------------
	mfc0	t0, C0_CONFIG, 7
	nop
	ori	t0, 2
	mtc0	t0, C0_CONFIG, 7
	nop

	//----------------------------------------------------
	// clear BSS section
	//----------------------------------------------------
	la	t0, _fbss
	la	t1, _end
1:	sw	zero, 0(t0)
	bne	t0, t1, 1b
	addiu	t0, 4

	//----------------------------------------------------
	// setup stack and GP, jump to C code
	//----------------------------------------------------
	la	gp, _gp
	la	sp, g_stack
	addiu	sp, sp, 0x2000

	la	t0, c_main
	jr	t0
	nop

	.extern real_exception_entry
	.global except_common_entry
	.type	except_common_entry,@function
except_common_entry:
	la	k0, real_exception_entry
	jr	k0
	nop
	nop
	nop

	.extern OS_INTHandler
	.extern exception_handler
	.global real_exception_entry
	.type	real_exception_entry,@function
	.set noreorder
real_exception_entry:
	la	k1, (0x1f << 2)
	mfc0	k0, C0_CAUSE
	and	k0, k0, k1
	beq	zero, k0, intr_handler
	nop
	la       k1,(8 << 2)
	and      k0,k0,k1
	beq      zero,k0,excpt_handler
	nop	
excpt_syscall:
	la     k0,1
	beq    k0,v0,eOSStartHighRdy
	nop
	la     k0,2
	beq    k0,v0,eOSCtxSw
	nop
eother:	
	b excpt_handler
	nop
eOSStartHighRdy:
	la     k0,exptOSStartHighRdy
	jr     k0
	nop
eOSCtxSw:
	la     k0,exptOSCtxSw
	jr     k0
	nop
excpt_handler:
	la	k0, exception_handler
	jr	k0
	nop
intr_handler:
	la	k0, OS_INTHandler
	jr	k0
	nop
	.set reorder

//-------------------------------------------------------
	.global exception_handler
	.type	exception_handler,@function
	.set noreorder	
exception_handler:
	move    k0, sp
	la	sp, g_stack		// restore the SP register, and we
	addiu	sp, sp, 0x1000		// will panic in except handler.
	sw	$0, (4*0)(sp);
	sw	$1, (4*1)(sp);
	sw	$2, (4*2)(sp);
	sw	$3, (4*3)(sp);
	sw	$4, (4*4)(sp);
	sw	$5, (4*5)(sp);
	sw	$6, (4*6)(sp);
	sw	$7, (4*7)(sp);
	sw	$8, (4*8)(sp);
	sw	$9, (4*9)(sp);
	sw	$10, (4*10)(sp);
	sw	$11, (4*11)(sp);
	sw	$12, (4*12)(sp);
	sw	$13, (4*13)(sp);
	sw	$14, (4*14)(sp);
	sw	$15, (4*15)(sp);
	sw	$16, (4*16)(sp);
	sw	$17, (4*17)(sp);
	sw	$18, (4*18)(sp);
	sw	$19, (4*19)(sp);
	sw	$20, (4*20)(sp);
	sw	$21, (4*21)(sp);
	sw	$22, (4*22)(sp);
	sw	$23, (4*23)(sp);
	sw	$24, (4*24)(sp);
	sw	$25, (4*25)(sp);
	sw	$26, (4*26)(sp);
	sw	$27, (4*27)(sp);
	sw	$28, (4*28)(sp);
	sw	k0, (4*29)(sp);
	sw	$30, (4*30)(sp);
	sw	$31, (4*31)(sp);
	move	a0, sp
	la	k0, c_except_handler
	jr	k0
	nop
	.set reorder

	.global	enable_jz_mxu
	.type	enable_jz_mxu, @function
	.set noreorder
enable_jz_mxu:
li	$3,3
#APP
.word	0b01110000000000110000010000101111	#S32I2M XR16,$3
#NO_APP
j	$31
nop
	.set reorder

	
