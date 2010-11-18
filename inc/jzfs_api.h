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
*/

#ifndef __JZFS_API_H__
#define __JZFS_API_H__

#ifdef __cplusplus
extern "C"{
#endif


// macros
typedef unsigned int                jzfs_size_t;
#define JZFS_DIRNAME_MAX            256

#define JZFS_ERR_OK                 (short)0x0000
#define JZFS_ERR_EOF                (short)0xfff0
#define JZFS_ERR_DISKFULL           (short)0xffe0
#define JZFS_ERR_INVALIDPAR         (short)0xffd0
#define JZFS_ERR_WRITEONLY          (short)0xffc0
#define JZFS_ERR_READONLY           (short)0xffb0
#define JZFS_ERR_READERROR          (short)0xffa0
#define JZFS_ERR_WRITEERROR         (short)0xff90
#define JZFS_ERR_DISKCHANGED        (short)0xff80
#define JZFS_ERR_CLOSE              (short)0xff70

#define JZFS_SEEK_CUR               1
#define JZFS_SEEK_END               2
#define JZFS_SEEK_SET               0

#define JZFS_CMD_FLUSH_CACHE        1000L
#define JZFS_CMD_CHK_DSKCHANGE      1010L
#define JZFS_CMD_READ_SECTOR        1100L
#define JZFS_CMD_WRITE_SECTOR       1110L
#define JZFS_CMD_FORMAT_MEDIA       2222L
#define JZFS_CMD_FORMAT_AUTO        2333L
#define JZFS_CMD_INC_BUSYCNT        3001L
#define JZFS_CMD_DEC_BUSYCNT        3002L
#define JZFS_CMD_GET_DISKFREE       4000L
#define JZFS_CMD_GET_DEVINFO        4011L
#define JZFS_CMD_FLASH_ERASE_CHIP   9001L
#define JZFS_CMD_MOUNT              9002L
#define JZFS_CMD_UNMOUNT            9003L
#define	JZFS_CMD_CHECKMBR						9004L
#define	JZFS_CMD_SELFCHECK          9005L

#define JZFS_FAT_ATTR_READ_ONLY     0x01
#define JZFS_FAT_ATTR_HIDDEN        0x02
#define JZFS_FAT_ATTR_SYSTEM        0x04
#define JZFS_FAT_ATTR_DB            0x06
#define JZFS_FAT_VOLUME_ID          0x08
#define JZFS_FAT_ATTR_ARCHIVE       0x20
#define JZFS_FAT_ATTR_DIRECTORY     0x10
#define JZFS_FAT_ATTR_LFNAME	    0x0F


// structs
typedef struct _jzfs_file
{
	unsigned int    dwFlags;
	unsigned char  *pBase;
	unsigned char  *pTail;
	unsigned char   sData[128];
    unsigned int  (*fnRead) (void *pData,
                             unsigned int dwSize,
                             unsigned int dwCount,
                             struct _jzfs_file *pstFile);
    unsigned int  (*fnWrite)(const void *pData,
                             unsigned int dwSize,
                             unsigned int dwCount,
                             struct _jzfs_file *pstFile);
	unsigned int    dwIdLo;
	unsigned int    dwIdHi;
	unsigned int    dwIdEx;
	int             nEOFClust;
	unsigned int    dwCurClust;
	int             nFilePos;
	int             nSize;
	int             nFreeSpace;
	int             nDeviceIndex;
	short           error;
	unsigned char   inuse;
	unsigned char   mode_r;
	unsigned char   mode_w;
	unsigned char   mode_a;
	unsigned char   mode_c;
	unsigned char   mode_b;
	unsigned short  wCreatTime;
	unsigned short  wCreatDate;
} JZFS_FILE;


typedef struct _jzfs_diskfree_t
{
	unsigned int    dwTotalClusters;
	unsigned int    dwAvailClusters;
	unsigned short  wSectorsPerCluster;
	unsigned short  wBytesPerSector;
} JZFS_DISKFREE_T;


typedef struct _jzfs_dirent
{
	int             nIno;
	char            sName[JZFS_DIRNAME_MAX];
	char            chFATDirAttr;
	char            nametype; //0 -short name 1- long name
} JZFS_DIRENT;


typedef struct _jzfs_dir
{
	JZFS_DIRENT     dirent;
	unsigned int    dwDirIdLo;
	unsigned int    dwDirIdHi;
	unsigned int    dwDirIdEx;
	int             nDirPos;
	int             nSize;
	int             nDevIndix;
	short           error;
	unsigned char   inuse;
} JZFS_DIR;


// functions
JZFS_FILE          *jzfs_Open            (const char       *sFileName,
                                          const char       *sMode);

void                jzfs_Close           (JZFS_FILE        *pstFile);

unsigned int         jzfs_Read            (void             *pData,
                                          unsigned int       dwSize,
                                          unsigned int       dwCount,
                                          JZFS_FILE        *pstFile);

unsigned int         jzfs_Write           (const void       *pData,
                                          unsigned int       dwSize,
                                          unsigned int       dwCount,
                                          JZFS_FILE        *pstFile);

int                 jzfs_Seek            (JZFS_FILE        *pstFile,
                                          int               nOffset,
                                          int               nWhence);

int                 jzfs_Tell            (JZFS_FILE        *pstFile);

short               jzfs_Error           (JZFS_FILE        *pstFile);

void                jzfs_ClearError      (JZFS_FILE        *pstFile);

int                 jzfs_Remove          (const char       *sFileName);

int                 jzfs_IoCtl           (const char       *sDevName,
                                          int               nCmd,
                                          int               nAux,
                                          void             *pBuffer);

JZFS_DIR           *jzfs_OpenDir         (const char       *sDirName);

int                 jzfs_CloseDir        (JZFS_DIR         *pstDir);

JZFS_DIRENT        *jzfs_ReadDir         (JZFS_DIR         *pstDir);

JZFS_DIRENT        *jzfs_ReadDirBackward (JZFS_DIR         *pstDir);

void                jzfs_RewindDir       (JZFS_DIR         *pstDir);

int                 jzfs_Mkdir           (const char       *sDirName);

int                 jzfs_RmDir           (const char       *sDirName);

int                 jzfs_Init            (void);

int                 jzfs_Exit            (void);


#ifdef __cplusplus
}
#endif

#endif  //__JZFS_API_H__


