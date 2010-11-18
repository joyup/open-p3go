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
 *  Author:   <xyzhang@ingenic.cn> 
 *
 *  Create:   2008-09-23, by xyzhang
 * 
 *
 *******************************************************************************
 */


#ifndef __JZ4755MSC_H__
#define __JZ4755MSC_H__


//--------------------------------------------------------------------------
// MMC/SD CE-ATA Controller (MSC) Address Definition
//--------------------------------------------------------------------------
#define MSC0_PHYS_ADDR				( 0x10021000 )
#define MSC0_BASE_U_VIRTUAL			( 0xB0021000 )
#define MSC1_PHYS_ADDR				( 0x10022000 )
#define MSC1_BASE_U_VIRTUAL			( 0xB0022000 )

/*
#if	TF_SD_CARD
#define MSC_PHYS_ADDR				MSC1_PHYS_ADDR
#define MSC_BASE_U_VIRTUAL			MSC1_BASE_U_VIRTUAL
#else
#define MSC_PHYS_ADDR				MSC0_PHYS_ADDR
#define MSC_BASE_U_VIRTUAL			MSC0_BASE_U_VIRTUAL
#endif
*/
#define MSC_PHYS_ADDR(x)				MSC0_PHYS_ADDR + x * 0x1000
#define MSC_BASE_U_VIRTUAL(x)			MSC0_BASE_U_VIRTUAL + x * 0x1000

//--------------------------------------------------------------------------
// MSC Registers Offset Definition
//--------------------------------------------------------------------------
#define MSC_CTRL_OFFSET				( 0x00 )	//  W,  16, 0x000, MSC Control register
#define MSC_STAT_OFFSET				( 0x04 )	//  R,  32, 0x00000040, MSC Status register
#define MSC_CLKRT_OFFSET			( 0x08 )	//  RW, 16, 0x0000, MSC Clock Rate register
#define MSC_CMDAT_OFFSET			( 0x0C )	//  RW, 32, 0x00000000, MSC Command and Data Control register
#define MSC_RESTO_OFFSET			( 0x10 )	//  RW, 16, 0x0040, MSC Response Time Out register
#define MSC_RDTO_OFFSET				( 0x14 )	//  RW, 16, 0xFFFF, MSC Read Time Out register
#define MSC_BLKLEN_OFFSET			( 0x18 )	//  RW, 16, 0x0000, MSC Block Length register
#define MSC_NOB_OFFSET				( 0x1C )	//  RW, 16, 0x0000, MSC Number of Block register
#define MSC_SNOB_OFFSET				( 0x20 )	//  R,  16, 0x????, MSC Number of Successfully-transferred Blocks register
#define MSC_IMASK_OFFSET			( 0x24 )	//  RW, 32, 0x000000FF, MSC Interrupt Mask register
#define MSC_IREG_OFFSET				( 0x28 )	//  RW, 16, 0x2000, MSC Interrupt register
#define MSC_CMD_OFFSET				( 0x2C )	//  RW,  8, 0x00, MSC Command Index register
#define MSC_ARG_OFFSET				( 0x30 )	//  RW, 32, 0x00000000, MSC Command Argument register
#define MSC_RES_OFFSET				( 0x34 )	//  R,  16, 0x????, MSC Response FIFO register
#define MSC_RXFIFO_OFFSET			( 0x38 )	//  R,  32, 0x????????, MSC Receive Data FIFO register
#define MSC_TXFIFO_OFFSET			( 0x3C )	//  W,  32, 0x????????, MSC Transmit Data FIFO register
#define MSC_LPM_OFFSET				( 0x40 )	//  RW, 32, 0x00000000, MSC Low Power Mode register


//--------------------------------------------------------------------------
// MSC Registers Address Definition
/*/--------------------------------------------------------------------------
 
#define A_MSC_CTRL	    		( MSC_BASE_U_VIRTUAL + MSC_CTRL_OFFSET  )
#define A_MSC_STAT	    		( MSC_BASE_U_VIRTUAL + MSC_STAT_OFFSET	)
#define A_MSC_CLKRT   			( MSC_BASE_U_VIRTUAL + MSC_CLKRT_OFFSET	)
#define A_MSC_CMDAT				( MSC_BASE_U_VIRTUAL + MSC_CMDAT_OFFSET	)
#define A_MSC_RESTO	   			( MSC_BASE_U_VIRTUAL + MSC_RESTO_OFFSET )
#define A_MSC_RDTO	   			( MSC_BASE_U_VIRTUAL + MSC_RDTO_OFFSET  )
#define A_MSC_BLKLEN   			( MSC_BASE_U_VIRTUAL + MSC_BLKLEN_OFFSET)
#define A_MSC_NOB	   			( MSC_BASE_U_VIRTUAL + MSC_NOB_OFFSET   )
#define A_MSC_SNOB	    		( MSC_BASE_U_VIRTUAL + MSC_SNOB_OFFSET	)
#define A_MSC_IMASK   			( MSC_BASE_U_VIRTUAL + MSC_IMASK_OFFSET )
#define A_MSC_IREG	    		( MSC_BASE_U_VIRTUAL + MSC_IREG_OFFSET  )
#define A_MSC_CMD	    		( MSC_BASE_U_VIRTUAL + MSC_CMD_OFFSET   )
#define A_MSC_ARG	    		( MSC_BASE_U_VIRTUAL + MSC_ARG_OFFSET   )
#define A_MSC_RES	    		( MSC_BASE_U_VIRTUAL + MSC_RES_OFFSET   )
#define A_MSC_RXFIFO    		( MSC_BASE_U_VIRTUAL + MSC_RXFIFO_OFFSET)
#define A_MSC_TXFIFO    		( MSC_BASE_U_VIRTUAL + MSC_TXFIFO_OFFSET)
#define A_MSC_LPM    			( MSC_BASE_U_VIRTUAL + MSC_LPM_OFFSET   )
*/
#define A_MSC_CTRL(x)	    		( MSC_BASE_U_VIRTUAL(x) + MSC_CTRL_OFFSET  )
#define A_MSC_STAT(x)	    		( MSC_BASE_U_VIRTUAL(x) + MSC_STAT_OFFSET	)
#define A_MSC_CLKRT(x)   			( MSC_BASE_U_VIRTUAL(x) + MSC_CLKRT_OFFSET	)
#define A_MSC_CMDAT(x)				( MSC_BASE_U_VIRTUAL(x) + MSC_CMDAT_OFFSET	)
#define A_MSC_RESTO(x)	   			( MSC_BASE_U_VIRTUAL(x) + MSC_RESTO_OFFSET )
#define A_MSC_RDTO(x)	   			( MSC_BASE_U_VIRTUAL(x) + MSC_RDTO_OFFSET  )
#define A_MSC_BLKLEN(x)   			( MSC_BASE_U_VIRTUAL(x) + MSC_BLKLEN_OFFSET)
#define A_MSC_NOB(x)	   			( MSC_BASE_U_VIRTUAL(x) + MSC_NOB_OFFSET   )
#define A_MSC_SNOB(x)	    		( MSC_BASE_U_VIRTUAL(x) + MSC_SNOB_OFFSET	)
#define A_MSC_IMASK(x)   			( MSC_BASE_U_VIRTUAL(x) + MSC_IMASK_OFFSET )
#define A_MSC_IREG(x)	    		( MSC_BASE_U_VIRTUAL(x) + MSC_IREG_OFFSET  )
#define A_MSC_CMD(x)	    		( MSC_BASE_U_VIRTUAL(x) + MSC_CMD_OFFSET   )
#define A_MSC_ARG(x)	    		( MSC_BASE_U_VIRTUAL(x) + MSC_ARG_OFFSET   )
#define A_MSC_RES(x)	    		( MSC_BASE_U_VIRTUAL(x) + MSC_RES_OFFSET   )
#define A_MSC_RXFIFO(x)    			( MSC_BASE_U_VIRTUAL(x) + MSC_RXFIFO_OFFSET)
#define A_MSC_TXFIFO(x)    			( MSC_BASE_U_VIRTUAL(x) + MSC_TXFIFO_OFFSET)
#define A_MSC_LPM(x)   				( MSC_BASE_U_VIRTUAL(x) + MSC_LPM_OFFSET   )


//--------------------------------------------------------------------------
// MMC/SD Control Register field descriptions (MSC_CTRL)
//--------------------------------------------------------------------------

#define MSC_CTRL_CLOCK_CONTROL_MASK		( 3 << 0 )
#define MSC_CTRL_CLOCK_DONOTHING		( 0 << 0 )
#define MSC_CTRL_CLOCK_STOP				( 1 << 0 )
#define MSC_CTRL_CLOCK_START			( 2 << 0 )

#define MSC_CTRL_START_OP				( 1 << 2 )
#define MSC_CTRL_RESET					( 1 << 3 )
#define MSC_CTRL_STOP_RDWAIT			( 1 << 4 )
#define MSC_CTRL_START_RDWAIT			( 1 << 5 )
#define MSC_CTRL_EXIT_TRANSFER			( 1 << 6 )
#define MSC_CTRL_EXIT_MULTIPLE			( 1 << 7 )
#define MSC_CTRL_SEND_AS_CCSD			( 1 << 14 )
#define MSC_CTRL_SEND_CCSD				( 1 << 15 )

//--------------------------------------------------------------------------
// MSC Status Register field descriptions (MSC_STAT)
//--------------------------------------------------------------------------

#define MSC_STAT_TIME_OUT_READ			( 1 << 0 ) 
#define MSC_STAT_TIME_OUT_RES			( 1 << 1 ) 

#define MSC_STAT_CRC_WRITE_ERR_MASK		( 3 << 2 )
#define MSC_STAT_CRC_WRITE_NO_ERR		( 0 << 2 )
#define MSC_STAT_CRC_WRITE_ERR			( 1 << 2 )
#define MSC_STAT_CRC_WRITE_NO_STATUS	( 2 << 2 )

#define MSC_STAT_CRC_READ_ERR			( 1 << 4 ) 
#define MSC_STAT_CRC_RES_ERR			( 1 << 5 ) 
#define MSC_STAT_DATA_FIFO_EMPTY		( 1 << 6 ) 
#define MSC_STAT_DATA_FIFO_FULL			( 1 << 7 ) 
#define MSC_STAT_CLK_EN					( 1 << 8 ) 
#define MSC_STAT_IS_READWAIT			( 1 << 9 ) 
#define MSC_STAT_DATA_FIFO_AFULL		( 1 << 10 ) 
#define MSC_STAT_END_CMD_RES			( 1 << 11 ) 
#define MSC_STAT_DATA_TRAN_DONE			( 1 << 12 ) 
#define MSC_STAT_PRG_DONE				( 1 << 13 ) 
#define MSC_STAT_SDIO_INT_ACTIVE		( 1 << 14 ) 
#define MSC_STAT_IS_RESETTING			( 1 << 15 ) 
#define MSC_STAT_AUTO_CMD_DONE			( 1 << 31 ) 


//--------------------------------------------------------------------------
//MMC/SD Command and Data Control Register field descriptions (MSC_CMDAT)
//--------------------------------------------------------------------------
#define MSC_CMDAT_RESP_FORMAT_MASK		( 7 << 0 )
#define MSC_CMDAT_RESPONSE_NONE			( 0 << 0 )/* No response */
#define MSC_CMDAT_RESPONSE_R1			( 1 << 0 )/* Format R1 and R1b */
#define MSC_CMDAT_RESPONSE_R2			( 2 << 0 )/* Format R2 */
#define MSC_CMDAT_RESPONSE_R3			( 3 << 0 )/* Format R3 */
#define MSC_CMDAT_RESPONSE_R4			( 4 << 0 )/* Format R4 */
#define MSC_CMDAT_RESPONSE_R5			( 5 << 0 )/* Format R5 */
#define MSC_CMDAT_RESPONSE_R6			( 6 << 0 )/* Format R6 */
#define MSC_CMDAT_RESPONSE_R7			( 7 << 0 )/* Format R7 */

#define MSC_CMDAT_DATA_EN				( 1 << 3 )
#define MSC_CMDAT_WRRD_MASK				( 1 << 4 )
#define	MSC_CMDAT_WRITE					( 1 << 4 )
#define	MSC_CMDAT_READ					( 0 << 4 )
#define MSC_CMDAT_STREAM_BLOCK			( 1 << 5 )
#define MSC_CMDAT_BUSY					( 1 << 6 )
#define MSC_CMDAT_INIT					( 1 << 7 )
#define MSC_CMDAT_DMA_EN				( 1 << 8 )

#define MSC_CMDAT_BUS_WIDTH_MASK		( 3 << 9 )
#define MSC_CMDAT_BUS_WIDTH_1BIT		( 0 << 9 )
#define MSC_CMDAT_BUS_WIDTH_4BIT		( 2 << 9 )
#define MSC_CMDAT_BUS_WIDTH_8BIT		( 3 << 9 )

#define MSC_CMDAT_STOP_ABORT			( 1 << 11 )

#define MSC_CMDAT_TTRG_MASK				( 3 << 12 )
#define MSC_CMDAT_TTRG_08				( 0 << 12 )
#define MSC_CMDAT_TTRG_16				( 1 << 12 )
#define MSC_CMDAT_TTRG_24				( 2 << 12 )

#define MSC_CMDAT_RTRG_MASK				( 3 << 14 )
#define MSC_CMDAT_RTRG_08				( 0 << 14 )
#define MSC_CMDAT_RTRG_16				( 1 << 14 )
#define MSC_CMDAT_RTRG_24				( 2 << 14 )

#define MSC_CMDAT_SEND_AS_STOP			( 1 << 16 )
#define MSC_CMDAT_SDIO_PRDT				( 1 << 17 )
#define MSC_CMDAT_READ_CEATA			( 1 << 30 )
#define MSC_CMDAT_CCS_EXPECTED			( 1 << 31 )

//--------------------------------------------------------------------------
// IRQ Number descriptions(can used for MSC_IMASK,MSC_IREG)
//--------------------------------------------------------------------------
#define MSC_DATA_TRAN_DONE		( 1 << 0 )
#define MSC_PRG_DONE			( 1 << 1 )
#define MSC_END_CMD_RES			( 1 << 2 )
#define MSC_RXFIFO_RD_REQ		( 1 << 5 )
#define MSC_TXFIFO_WR_REQ		( 1 << 6 )
#define MSC_SDIO				( 1 << 7 )
#define MSC_TIME_OUT_READ		( 1 << 8 )
#define MSC_TIME_OUT_RES		( 1 << 9 )
#define MSC_CRC_WRITE_ERR		( 1 << 10 )
#define MSC_CRC_READ_ERR		( 1 << 11 )
#define MSC_CRC_RES_ERR			( 1 << 12 )
#define MSC_DATA_FIFO_EMP		( 1 << 13 )
#define MSC_DATA_FIFO_FULL		( 1 << 14 )
#define MSC_AUTO_CMD_DONE		( 1 << 15 )

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{   
	unsigned short	CTRL;		//0x00
	unsigned char	RESERVE1[2];//0x02
	unsigned int	STAT;	    //0x04
	unsigned short	CLKRT;      //0x08
	unsigned char	RESERVE2[2];//0x0A
	unsigned int	CMDAT;	    //0x0C
	unsigned short	RESTO;	    //0x10
	unsigned char	RESERVE3[2];//0x12
	unsigned short	RDTO;	    //0x14
	unsigned char	RESERVE4[2];//0x16
	unsigned short	BLKLEN;	    //0x18
	unsigned char	RESERVE5[2];//0x1A
	unsigned short	NOB;	    //0x1C
	unsigned char	RESERVE6[2];//0x1E
	unsigned short	SNOB;	    //0x20
	unsigned char	RESERVE7[2];//0x22
	unsigned int	IMASK;	    //0x24
	unsigned short	IREG;	    //0x28
	unsigned char	RESERVE8[2];//0x2A
	unsigned char	CMD;       //0x2C
	unsigned char	RESERVE9[3];//0x2D
	unsigned int	ARG;	    //0x30
	unsigned short	RES;	    //0x34
	unsigned char	RESERVE10[2];//0x36
	unsigned int	RXFIFO;	    //0x38
	unsigned int	TXFIFO;     //0x3C
	unsigned int	LPM;        //0x40

} JZ4755_MSC, *PJZ4755_MSC;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755MSC_H__
