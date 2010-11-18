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

#ifndef __NANDPDD_H__
#define __NANDPDD_H__

// export nandpdd function
int jz_nand_init(void);
int jz_nand_get_info(pflash_info_t pflashinfo);

int jz_nand_get_block_status(unsigned int block);
int jz_nand_set_block_status(unsigned int block, nand_page_info_t info);

int jz_nand_read_page_info(void *dev_id, int page, nand_page_info_t *info);
int jz_nand_read_page(void *dev_id, int page, unsigned char *data, nand_page_info_t *info);

int jz_nand_write_page_info(void *dev_id, unsigned int page, nand_page_info_t *info);
int jz_nand_write_page(void *dev_id, int page, unsigned char *data, nand_page_info_t *info);

int jz_nand_erase_block(void *dev_id, unsigned int block);

int jz_nand_multiread(	void				*dev_id,
						unsigned int		page,
						unsigned int		pagecount,
						unsigned char		*mask,
						unsigned char		*data,
						nand_page_info_t	*info);

int jz_nand_multiwrite(	void				*dev_id,
						unsigned int		page,
						unsigned int		pagecount,
						unsigned char		*mask,
						unsigned char		*data,
						nand_page_info_t	*info);


#endif	//__NANDPDD_H__
