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


#ifndef __NAND_CFG_H__
#define __NAND_CFG_H__
static const NAND_INFO	g_NandInfo[] = 
{
//	{NandID,    NandExtID,	PlaneNum	Tals	Talh	Trp	Twp	Trhw	Twhr	dwPageSize,	dwBlockSize, dwPageAddressCycle,dwMinValidBlocks,dwMaxValidBlocks}
	{0xECD3,			0,		2,		15,		5,		15,	15,	60,		60,		2048,	256 * 1024, 		3,				3996,			4096    	}, //SAMSUNG_K9G8G08U0M 
	{0xECF1,			0,		1,		15,		5,		15,	15,	100,	60,		2048,	128 * 1024, 		3,				1004,			1024		},
//	{0xECD5,	0x00009510,		1,		15,		5,		15,	15,	100, 	60,		2048,	256 * 1024, 		3,				7992,			8192		},//SAMSUNG_K9F4G08U0A
//	{0xECD5,	0x0074b614,		1,		12,		5,		12,	12,	100, 	60,		4096,	512 * 1024, 		3,				3996,			4096		},//SAMSUNG_K9GAG08U0M
	{0xECD7,			0,		    1,		12,		5,		12,	12,	100, 	60,		4096,	512 * 1024,	    3,				7992,			8192		},//SAMSUNG_K9LBG08U0M
	{0xADF1,			0,		1,		5,		15,		25,	40,	 60, 	60,		2048,	128 * 1024,			3,				1004,			1024		},//HYNIX_HY27UF081G2M
	//{0xADD3,	 		0,		1,		12,		5,		12,	12,	100, 	80,		2048,	256 * 1024,			3,				3996,			4096		}//HYNIX_HY27UT088G2A
	{0xADD3,   0x0064A514,		2,		12,		5,		12,	12,	100, 	80,		2048,	256 * 1024,			3,				3996,			4096},	
	{0xECD5,   0x00682555,   0x800002,      12,             5,      12, 12, 60,    60,     2048,   256 * 1024,                 3,                          7992,                   8192            },
	{0xECD5,   0x0068a555,   0x800002,      15,		5,      15, 15, 100,    60,     2048,	256 * 1024,		    3,				7992,			8192		}
};
//Note: NandID = Manufacture code + device code; and if your chip is not mutil cs,you must set NandExtId = 0,whether it has or no.

#endif // __NAND_CFG_H__
