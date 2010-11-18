/********************** BEGIN LICENSE BLOCK **********************************
 *
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co., Ltd. 2008. All rights reserved.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * http://www.ingenic.cn
 *
 ********************** END LICENSE BLOCK ************************************
 *
 *  Author:  <whxu@ingenic.cn>
 *
 *  Create:   2008-10-13, by whxu
 *
 ****************************************************************************/

#ifndef __BSP_H__
#define __BSP_H__

#if  defined   PLATFORM_CETUS
	#include <PlatformCfg/cetus_cfg.h>

#elif defined PLATFORM_SG
	#include <PlatformCfg/sg_cfg.h>

#else
	#error "error define PLATFORM_XXXX"
#endif

#endif	// __BSP_H__
