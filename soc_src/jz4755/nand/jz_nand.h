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
 *  Maintain: 090605 jlv         
 *
 *******************************************************************************
 */


#ifndef __JZ_NAND_H__
#define __JZ_NAND_H__


#define CONFIG_SSFDC_HW_RS_ECC 

#define CONFIG_SSFDC_NAND_ROW_CYCLE 2	//2010.10.08 danny changed for k9f1g08
#define CONFIG_SSFDC_WRITE_VERIFY 0
#define CONFIG_SSFDC_WEAR_ENABLE 1
#define JZ_WAIT_STAUTS_DELAY 30

 
//
//	NAND OP
//

#define NANDFLASH_CLE           0x00008000 //PA[15]
#define NANDFLASH_ALE           0x00010000 //PA[16]

#define NAND_IO_ADDR           *((volatile unsigned char *) NANDFLASH_BASE)
#define NAND_CMD_ADDR           *((volatile unsigned char *) (NANDFLASH_BASE + NANDFLASH_CLE))
#define NAND_ADDR_ADDR           *((volatile unsigned char *) (NANDFLASH_BASE + NANDFLASH_ALE))

#define REG_NAND_DATA  (*((volatile unsigned char *) NANDFLASH_BASE))
#define REG_NAND_CMD   (*((volatile unsigned char *) (NANDFLASH_BASE + NANDFLASH_CLE)))
#define REG_NAND_ADDR  (*((volatile unsigned char *) (NANDFLASH_BASE + NANDFLASH_ALE)))

#define JZ_NAND_SET_CLE    (NANDFLASH_BASE |=  NANDFLASH_CLE)
#define JZ_NAND_CLR_CLE    (NANDFLASH_BASE &= ~NANDFLASH_CLE)
#define JZ_NAND_SET_ALE    (NANDFLASH_BASE |=  NANDFLASH_ALE)
#define JZ_NAND_CLR_ALE    (NANDFLASH_BASE &= ~NANDFLASH_ALE)

#define P2_SEND_SECTOR_ADDR1(page)				\
do {											\
	unsigned int pagemsk = page & g_dwplanemask;\
	REG_NAND_CMD = NAND_CMD_PAGE_PROGRAM_START; \
	REG_NAND_ADDR = 0;							\
	REG_NAND_ADDR = 0;							\
	REG_NAND_ADDR = (unsigned char)(pagemsk & 0xff);				\
	REG_NAND_ADDR = (unsigned char)((pagemsk >> 8) & 0xff);		\
	if ( CONFIG_SSFDC_NAND_ROW_CYCLE >= 3 )		\
		REG_NAND_ADDR = (unsigned char)((pagemsk >> 16) & 0xff);\
} while (0)

#define P2_SEND_SECTOR_ADDR2(page)     \
do {                                  \
	REG_NAND_CMD = P2_2_CMD_PAGE_PROGRAM_START;\
	REG_NAND_ADDR = 0;							\
	REG_NAND_ADDR = 0;							\
	REG_NAND_ADDR = (unsigned char)(page & 0xff);		        \
	REG_NAND_ADDR = (unsigned char)(((page) >> 8) & 0xff);		\
	if (CONFIG_SSFDC_NAND_ROW_CYCLE >= 3 )		\
		REG_NAND_ADDR = (unsigned char)(((page) >> 16)& 0xff);	\
}while(0)

#define P2_SEND_ERASE_ADDR(page)     \
do{                                  \
	unsigned int pagemsk = page & g_dwplanemask;  \
	REG_NAND_ADDR = (unsigned char)(pagemsk & 0xff);				\
	REG_NAND_ADDR = (unsigned char)((pagemsk >> 8)& 0xff);		\
	if(CONFIG_SSFDC_NAND_ROW_CYCLE >= 3 )		\
		REG_NAND_ADDR = (unsigned char)((pagemsk >> 16)& 0xff);	\
	REG_NAND_CMD = NAND_CMD_BLOCK_ERASE_START;\
	pagemsk = page + (g_dwnandperblock / g_dwplanenum);  \
	REG_NAND_ADDR = (unsigned char)(pagemsk & 0xff);     			\
	REG_NAND_ADDR = (unsigned char)(((pagemsk) >> 8) & 0xFF);\
	if(CONFIG_SSFDC_NAND_ROW_CYCLE >= 3 )				\
		REG_NAND_ADDR = (unsigned char)(((pagemsk) >> 16) & 0xFF);\
}while(0)

#define MAX_CHIP_NUM_RES 4
#define JZ_NAND_ERASE_TIME (30 * 1000)
#define RB_GPIO_GROUP		2
#define RB_GPIO_BIT			27
#define RB_GPIO_BIT_FLG		(1 << RB_GPIO_BIT)
#define RB_GPIO_PIN			(32 * RB_GPIO_GROUP + RB_GPIO_BIT)

#define	Nand_BCH_ENABLE()		OUTREG32( A_BCH_BHCSR, ( BCH_BCHE | BCH_BRST ))
#define	NAND_BCH_DISABLE()		OUTREG32( A_BCH_BHCCR, BCH_BCHE )

#if 0
#define NAND_BCH_ENCODE(count) 				\
do {												\
	OUTREG32( A_BCH_BHINTS, 0xFFFFFFFF); 			\
	OUTREG32( A_BCH_BHCSR, (bhcr_set | BCH_BCHE | BCH_BRST | BCH_ENCE));	\
	OUTREG32( A_BCH_BHCCR, bhcr_clr);				\
	OUTREG32( A_BCH_BHCNT, (count));				\
} while(0)

#define NAND_BCH_DECODE(count)  \
do{										\
	OUTREG32( A_BCH_BHINTS, 0xFFFFFFFF); 			\
	OUTREG32( A_BCH_BHCSR, ( BCH_BCHE | bhcr_set));\
	OUTREG32( A_BCH_BHCCR, ( BCH_DNCE | bhcr_clr));\
	OUTREG32( A_BCH_BHCSR, BCH_BRST);\
	OUTREG32( A_BCH_BHCNT, (count) << 16 );\
} while(0)

#define PAR_SIZE            parsize
#define ECC_POS				8
#define __nand_ecc_encode_sync()  while (!(INREG32( A_BCH_BHINTS ) & BCH_INTS_ENCF))
#else

#define NAND_BCH_ENCODE(count) 				\
do {												\
	OUTREG32( A_BCH_BHINTS, 0xFFFFFFFF); 			\
	OUTREG32( A_BCH_BHCSR, ( BCH_BCHE | BCH_BRST | BCH_BSEL | BCH_ENCE));\
	OUTREG32( A_BCH_BHCNT, (count));										\
} while(0)

#define NAND_BCH_DECODE(count)  \
do{										\
	OUTREG32( A_BCH_BHINTS, 0xFFFFFFFF); 			\
	OUTREG32( A_BCH_BHCSR, ( BCH_BCHE | BCH_BSEL));\
	OUTREG32( A_BCH_BHCCR, ( BCH_DNCE));\
	OUTREG32( A_BCH_BHCSR, BCH_BRST);\
	OUTREG32( A_BCH_BHCNT, (count) << 16 );\
} while(0)

#define PAR_SIZE            13
#define ECC_POS				8

#endif


typedef union
{
	unsigned int dwDES[8];
	struct 
	{
		unsigned int	dwDES0;
		unsigned int	dwDES1;
		unsigned int	dwDES2;
		unsigned int	dwDES3;
		unsigned int	dwDES4;
		unsigned int	dwDES5;
		unsigned int	dwDES6;
		unsigned int	dwDES7;
	}stDES;
}DMA_Descriptor,*PDMA_Descriptor;

typedef struct __DMA_BCH_DETARGET__
{
	unsigned int dwBHINT;
	unsigned int dwBCHERR[4];	
}DmaBchDeTarget,*PDmaBchDeTarget;

#endif /* __JZ_NAND_H__ */
