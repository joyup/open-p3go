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
 *  Maintain: 2008-12-01, by whxu
 *
 *******************************************************************************
 */

#ifndef __MMC_CONFIG__
#define __MMC_CONFIG__

#include <bsp.h>

#define MMC_DEBUG_LEVEL		1		/* Enable Debug: 0 - no debug */
#define MMC_MINIOS_EN		1

#define MMC_DMA_ENABLE		1

#define MMC_DMA_INTERRUPT	1
#define MMC_MSC_INTERRUPT	1

#define	MMC_4BIT_MODE		0x04
#define	MMC_1BIT_MODE		0x01

#define MMC1_DMA_CHANNEL	MSC_DMA_CHANNEL

#ifdef TF_SD_CARD
	#define CH	1	//MSC control
	#ifdef MMC1_POWER_PIN
		#define MMC_POWER_PIN			MMC1_POWER_PIN
	#endif
	#ifdef MMC1_SD_WP
		#define MMC_SD_WP				MMC1_SD_WP
	#endif

	#define MMC_CD_PIN				MMC1_CD_PIN
	#define	IRQ_MSC					IRQ_MSC1
//	#define	A_CPM_MSCCDR			A_CPM_MSC1CDR
	#define	CLKGR_STOP_MSC			CLKGR_STOP_MSC1
	#define	CARD_INSERT_VALUE		CARD_INSERT_VALUE1
	#define	DRT_MSC_TX				DRT_MSC1_TX
	#define	DRT_MSC_RX				DRT_MSC1_RX
#else
	ERROR
#endif

#endif /* __MMC_CONFIG__ */
