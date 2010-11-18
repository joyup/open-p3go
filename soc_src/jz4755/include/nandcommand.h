//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __NAND_COMMAND_H__
#define __NAND_COMMAND_H__

//////////////////////////16 bit
#if NAND_opera_16BIT

#define NAND_CMD_READ1_00                		0x0000	//  Read0
#define NAND_CMD_READ1_01_16bit                 0x0101	//  Read1
#define NAND_CMD_READ2_16bit                    0x5050	//  Read2
#define NAND_CMD_READ_ID1_16bit                 0x9090	//  ReadID1
#define NAND_CMD_READ_ID2_16bit                 0x9191	//	ReadID2
#define NAND_CMD_RESET_16bit                    0xFFFF	//  Reset

#define NAND_CMD_PAGE_PROGRAM_START_16bit       0x8080	//  Write phase 1
#define P2_1_CMD_PAGE_PROGRAM_STOP_16bit        0x1111

#define P2_2_CMD_PAGE_PROGRAM_START_16bit	      0x8181
#define NAND_CMD_PAGE_PROGRAM_STOP_16bit        0x1010

#define NAND_CMD_PAGE_RANDOM_PROGRAM_START_16bit  0x8585
#define	NAND_CMD_RANDOM_READ_16bit				0x0505
#define	NAND_CMD_RANDOM_READ_CONFIRM_16bit		0xE0E0

#define NAND_CMD_BLOCK_ERASE_START_16bit        0x6060	//	Erase block
#define NAND_CMD_BLOCK_ERASE_CONFIRM_16bit      0xD0D0	//	Erase block confirm
#define NAND_CMD_READ_STATUS_16bit             0x7070	//  Read Status
#define NAND_CMD_READ_STATUS_MULTI_16bit		  0x7171	//	Read multi status

/* Extended commands for large page devices */
#define NAND_CMD_READSTART_16bit				0x3030
#define NAND_CMD_CACHEDPROG_16bit				0x1515

#else

#define NAND_CMD_READ1_00                 0x00	//  Read0
#define NAND_CMD_READ1_01                 0x01	//  Read1
#define NAND_CMD_READ2                    0x50	//  Read2
#define NAND_CMD_READ_ID1                 0x90	//  ReadID1
#define NAND_CMD_READ_ID2                 0x91	//	ReadID2
#define NAND_CMD_RESET                    0xFF	//  Reset

#define NAND_CMD_PAGE_PROGRAM_START       0x80	//  Write phase 1
#define P2_1_CMD_PAGE_PROGRAM_STOP        0x11

#define P2_2_CMD_PAGE_PROGRAM_START	      0x81
#define NAND_CMD_PAGE_PROGRAM_STOP        0x10

#define NAND_CMD_PAGE_RANDOM_PROGRAM_START 0x85
#define	NAND_CMD_RANDOM_READ				0x05
#define	NAND_CMD_RANDOM_READ_CONFIRM		0xE0

#define NAND_CMD_BLOCK_ERASE_START        0x60	//	Erase block
#define NAND_CMD_BLOCK_ERASE_CONFIRM      0xD0	//	Erase block confirm
#define NAND_CMD_READ_STATUS              0x70	//  Read Status
#define NAND_CMD_READ_STATUS_MULTI		  0x71	//	Read multi status

/* Extended commands for large page devices */
#define NAND_CMD_READSTART				0x30
#define NAND_CMD_CACHEDPROG				0x15


#endif

#endif // __NAND_COMMAND_H__



