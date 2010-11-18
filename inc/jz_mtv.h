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
 *  Author:  <xyzhang@ingenic.cn>
 *
 *  Create:   2008-08-12, by xyzhang
 *            
 *
 *******************************************************************************
 */

/* 
 * IF101 Communication Layer Definition Header
 */

#ifndef __JZ_MTV_H__
#define __JZ_MTV_H__

#define MTV_CMD_HARD_RESET       0x01
#define MTV_CMD_SOFT_RESET       0x02
#define MTV_CMD_FLUSH_FIFO       0x03
#define MTV_CMD_QUERY_FIFO       0x04
#define MTV_CMD_QUERY_SIGN       0x05
#define MTV_CMD_POWER_DOWN       0x06
#define	MTV_CMD_CHANNEL_LENGTH	 0x07

#define MTV_COMMOM_CMD_NUM       0x07

#define MTV_CMD_SET_FREQUENCY       (0x01 + MTV_COMMOM_CMD_NUM)
#define MTV_CMD_SET_CHANNEL         (0x02 + MTV_COMMOM_CMD_NUM)

struct st_buf 
{
	unsigned char   *buf;
	int             pos;
	int             index;
	struct st_buf   *next;
};

struct mtv_sc 
{
	int             old_rear;
	int             timeout;
	int             num;
	struct st_buf	*front;
	struct st_buf	*rear;
	int             sfd;
};

struct mtv_dev_sc 
{
	unsigned char   open;
	int ch_id;
};
struct mtv_chan_length 
{
	unsigned int	chan_len_0;
	unsigned int	chan_len_1;
};

struct mtv_read_sc 
{
	int dummy_read;
	unsigned char   *buff;
	unsigned int	size;
};

#endif /*  __JZ_MTV_H__  */
