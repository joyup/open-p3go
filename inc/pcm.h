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



#ifndef __PCM_H__
#define __PCM_H__

/* Audio Sample Format */
#define	AFMT_U8			8
#define AFMT_S16_LE		16

/* PCM ioctl command */
#define PCM_SET_SAMPLE_RATE	0
#define PCM_SET_CHANNEL		1
#define PCM_SET_FORMAT		2
#define PCM_SET_VOL		3
#define PCM_GET_VOL		4
#define PCM_SET_HP_VOL		6
#define PCM_GET_HP_VOL		7
#define PCM_GET_SPACE           5
#define PCM_SET_PAUSE		8
#define PCM_SET_PLAY		9
#define PCM_RESET		10
#define PCM_SET_REPLAY		11
#define PCM_SET_RECORD		12
#define PCM_SET_MUTE		13

#define PCM_SET_EQPLAY  14
#define PCM_SET_VOLFUNCTION 15

#define PCM_GET_SAMPLE_MAX 16
#define PCM_SET_RECORD_FM	17 
#define PCM_REINIT          18
#define PCM_DATA_FINISH     19
#define PCM_POWER_ON  20
#define PCM_POWER_OFF 21
#endif /* __PCM_H__ */

