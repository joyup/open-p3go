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

#define MMC_BLOCKSIZE 		 512		/* MMC/SD Block Size */

#define MMC_OCR_ARG 		 0x00ff8000	/* Argument of OCR */

#include "mmc_protocol.h"
#include "mmc_api.h"
#include "mmc_core.h"
#include "os_api.h"
                     
#include <mmc_api.c> 
#include <mmc_protocol.c>
#include <mmc_core.c>
