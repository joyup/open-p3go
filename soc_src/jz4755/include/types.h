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
 
 
#ifndef _ASM_TYPES_H
#define _ASM_TYPES_H

typedef unsigned short umode_t;

typedef unsigned int	HANDLE;
/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */
#ifndef __s8
typedef __signed__ char __s8;
#endif
#ifndef __u8
typedef unsigned char __u8;
#endif
#ifndef __s16
typedef __signed__ short __s16;
#endif
#ifndef __u16
typedef unsigned short __u16;
#endif
#ifndef __s32
typedef __signed__ int __s32;
#endif
#ifndef __u32
typedef unsigned int __u32;
#endif
#if (_MIPS_SZLONG == 64)

#ifndef __s64
typedef __signed__ long __s64;
#endif 
#ifndef __u64
typedef unsigned long __u64;
#endif
#else

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#ifndef __s64
typedef __signed__ long long __s64;
#endif
#ifndef __u64
typedef unsigned long long __u64;
#endif
#endif

#endif

/*
 * These aren't exported outside the kernel to avoid name space clashes
 */
#ifdef __KERNEL__
#ifndef s8
typedef __signed char s8;
#endif 
#ifndef u8
typedef unsigned char u8;
#endif
#ifndef s16
typedef __signed short s16;
#endif
#ifndef u16
typedef unsigned short u16;
#endif
#ifndef s32
typedef __signed int s32;
#endif
#ifndef u32
typedef unsigned int u32;
#endif
#if (_MIPS_SZLONG == 64)
#ifndef s64
typedef __signed__ long s64;
#endif
#ifndef u64
typedef unsigned long u64;
#endif
#else

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#ifndef s64
typedef __signed__ long long s64;
#endif
#ifndef u64
typedef unsigned long long u64;
#endif
#endif

#endif

#define BITS_PER_LONG _MIPS_SZLONG

typedef unsigned long dma_addr_t;

#endif /* __KERNEL__ */

#endif /* _ASM_TYPES_H */
