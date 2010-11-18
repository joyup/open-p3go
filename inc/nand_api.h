#ifndef __NAND_API_h__
#define __NAND_API_h__
//------------------------------------------------------------------------------
//
// SSFDC PARAM
//
//------------------------------------------------------------------------------
#define SSFDC_BLOCK_STATUS_NORMAL		0xFFFFFFFF
#define SSFDC_BLOCK_VALID_ENTRY			0x80000000
#define SSFDC_BLOCK_ADDR_MASK				0x7FFFFFFF
#define SSFDC_BLOCK_TAG_FREE				0x01
#define SSFDC_BLOCK_TAG_BAD					0x02
#define SSFDC_ERROR									0x03

//------------------------------------------------------------------------------
//
// NAND
//
//------------------------------------------------------------------------------
#define	SSFDC_SKIP_DISK_CHECK		0x00000001

//------------------------------------------------------------------------------
//
// ERROR INDEX
//
//------------------------------------------------------------------------------
#define EIO				1
#define ENOMEM		2
#define ENODEV		3
#define EBUSY			4
#define EINVAL		1

#if STATUSPAGE
typedef struct __NAND_INFO__
{
	char 					*NandName;
	unsigned int	dwNandID;
  unsigned int  dwNandExtID;  //  32 - 24 bit is chipnumber
  unsigned int	dwPlaneNum;		//  Plane numbers to use
	// Essential Timing
	unsigned int	Tals;					// ALE Setup Time(Unit: ns)
	unsigned int	Talh;					// ALE Hold Time(Unit: ns)
	unsigned int	Trp;					// /RE Pulse Width(Unit: ns)
	unsigned int	Twp;					// /WE Pulse Width(Unit: ns)
	unsigned int	Trhw;					// /RE High to /WE Low(Unit: ns)
	unsigned int	Twhr;					// /WE High to /RE Low(Unit: ns)

	// Essential Nand Size
	unsigned int	dwPageSize;					// 512 or 2048
	unsigned int	dwBlockSize;				// 16K or 128K
	unsigned int	dwOOBSize;					// 64  or 128 or 218
	unsigned int	dwPageAddressCycle;	// 2 or 3

	unsigned int	dwMinValidBlocks;
	unsigned int	dwMaxValidBlocks;
} NAND_INFO;
typedef enum  __FLASH_TYPE__
{
	NANDFLASH,
	NORFLASH
} FLASH_TYPE;
typedef struct __NAND_INFO__ *PNAND_INFO;
typedef struct __FLASH_INFO__
{
	FLASH_TYPE		flashType;
	unsigned int	dwBytesPerBlock;
	unsigned int	dwSectorsPerBlock;
	unsigned int	dwDataBytesPerSector;

	unsigned int	dwBLStartBlock;
	unsigned int	dwBLTotalBlocks;
	unsigned int	dwBLTotalSectors;

	unsigned int	dwFSStartBlock;
	unsigned int	dwFSTotalBlocks;
	unsigned int	dwFSTotalSectors;

	unsigned int	dwTotalPhyBlocks;
	unsigned int	dwMaxBadBlocks;

	unsigned int	dwCylinders;
	unsigned int	dwHeads;
	unsigned int	dwSectors;
	unsigned int	dwFlags;
	PNAND_INFO pnandinfo;
} flash_info_t, *pflash_info_t;

typedef struct __NAND_STATUSPAGE__
{
	unsigned int	block_addr_field;
	unsigned int	lifetime;
	unsigned int	block_erase;
	unsigned char	reserved1[256 - 12];
	unsigned char	page_status[256];//mark one page status by one byte.Max 256 page,offset:256
	unsigned char	reserved;//page_size - 512
} nand_statuspage_t;


typedef struct __NAND_PAGE_INFO_
{
	unsigned int	block_status;
	unsigned int	reserved;
	unsigned char	ecc_field[220 - 8];
} nand_page_info_t;

int jz_nand_init(void);
int jz_nand_get_info(pflash_info_t pflashinfo);
int jz_nand_erase_block(void *dev_id,unsigned int block);

int jz_nand_read_page (void *dev_id, int page, unsigned char *data,nand_page_info_t *info);
int jz_nand_write_page(void *dev_id, int page, unsigned char *data,nand_page_info_t *info);

int jz_nand_read_page_info (void *dev_id, int page, nand_page_info_t *info);
int jz_nand_write_page_info(void *dev_id, unsigned int page, nand_page_info_t *info);

int jz_nand_multiwrite(	void *dev_id,unsigned int page,unsigned int	pagecount,
						unsigned char *mask,unsigned char *data,nand_statuspage_t *infopage);
int jz_nand_multiread(void *dev_id, unsigned int page,unsigned int pagecount,
					  unsigned char *mask,unsigned char *data,nand_statuspage_t *infopage);
#else

typedef struct __NAND_INFO__
{
	char 					*NandName;
	unsigned int	dwNandID;
  unsigned int  dwNandExtID;  //  32 - 24 bit is chipnumber
  unsigned int	dwPlaneNum;		//  Plane numbers to use
	// Essential Timing
	unsigned int	Tals;					// ALE Setup Time(Unit: ns)
	unsigned int	Talh;					// ALE Hold Time(Unit: ns)
	unsigned int	Trp;					// /RE Pulse Width(Unit: ns)
	unsigned int	Twp;					// /WE Pulse Width(Unit: ns)
	unsigned int	Trhw;					// /RE High to /WE Low(Unit: ns)
	unsigned int	Twhr;					// /WE High to /RE Low(Unit: ns)

	// Essential Nand Size
	unsigned int	dwPageSize;					// 512 or 2048
	unsigned int	dwBlockSize;				// 16K or 128K
	unsigned int	dwOOBSize;					// 64  or 128 or 218
	unsigned int	dwPageAddressCycle;	// 2 or 3
	unsigned int	dwMinValidBlocks;
	unsigned int	dwMaxValidBlocks;
} NAND_INFO;
typedef enum  __FLASH_TYPE__
{
	NANDFLASH,
	NORFLASH
} FLASH_TYPE;
typedef struct __NAND_INFO__ *PNAND_INFO;
typedef struct __FLASH_INFO__
{
	FLASH_TYPE		flashType;
	unsigned int	dwBytesPerBlock;
	unsigned int	dwSectorsPerBlock;
	unsigned int	dwDataBytesPerSector;

	unsigned int	dwBLStartBlock;
	unsigned int	dwBLTotalBlocks;
	unsigned int	dwBLTotalSectors;

	unsigned int	dwFSStartBlock;
	unsigned int	dwFSTotalBlocks;
	unsigned int	dwFSTotalSectors;

	unsigned int	dwTotalPhyBlocks;
	unsigned int	dwMaxBadBlocks;

	unsigned int	dwCylinders;
	unsigned int	dwHeads;
	unsigned int	dwSectors;
	unsigned int	dwFlags;
	PNAND_INFO pnandinfo;
} flash_info_t, *pflash_info_t;

#ifdef BCH8BIT

typedef struct __NAND_PAGE_INFO__
{
	unsigned int	block_status;
	unsigned int	block_addr_field;
	unsigned int	lifetime;
	unsigned int	page_status;
	unsigned int	block_erase;
	unsigned int	reserver;
	unsigned char	ecc_field[128 - 24];
} nand_page_info_t;


typedef struct __NAND_STATUSPAGE__
{
	unsigned int	pagestatus[4];
	unsigned int	pageinfoecc[2];
	unsigned char	ecc_field[128 - 24];
} nand_statuspage_info_t;//page1(&page2 for two plane) use this struct

typedef struct __NAND_ENDPAGE__
{
	unsigned int	block_status;
	unsigned int	block_addr_field;
	unsigned int	lifetime;
	unsigned int	block_erase;
	unsigned int	pageinfoecc[2];
	unsigned char	ecc_field[128 - 24];
} nand_endpage_info_t;//The end page uses this struct

#else
typedef struct __NAND_PAGE_INFO__
{
	unsigned int	block_status;
	unsigned int	block_addr_field;
	unsigned int	lifetime;
	unsigned int	page_status;
	unsigned int	block_erase;
	unsigned int	reserver[2];
	unsigned char	ecc_field[128 - 28];
} nand_page_info_t;


typedef struct __NAND_STATUSPAGE__
{
	unsigned int	pagestatus[4];
	unsigned int	reserver;
	unsigned int	pageinfoecc[2];
	unsigned char	ecc_field[128 - 28];
} nand_statuspage_info_t;//page1(&page2 for two plane) use this struct

typedef struct __NAND_ENDPAGE__
{
	unsigned int	block_status;
	unsigned int	block_addr_field;
	unsigned int	lifetime;
	unsigned int	page_status;
	unsigned int	block_erase;
	unsigned int	pageinfoecc[2];
	unsigned char	ecc_field[128 - 28];
} nand_endpage_info_t;//The end page uses this struct

#endif //end BCH8BIT


typedef struct __NAND_GROUP__
{
	unsigned int	page_addr;
	unsigned int	page_count;
	unsigned int	nextgroup;
} nand_group, *pnand_group;

int jz_nand_init(void);
int jz_nand_get_info(pflash_info_t pflashinfo);
int jz_nand_erase_block(void *dev_id,unsigned int block);

int jz_nand_read_page (void *dev_id, int page, unsigned char *data,nand_page_info_t *info);
int jz_nand_write_page(void *dev_id, int page, unsigned char *data, nand_page_info_t *info);
int jz_nand_read_page_info (void *dev_id, int page, nand_page_info_t *info);
int jz_nand_write_page_info(void *dev_id, unsigned int page, nand_page_info_t *info);

int jz_nand_multiwrite(	void *dev_id,unsigned int page,unsigned int	pagecount,
						unsigned char	*mask,unsigned char	*data,nand_page_info_t *info);
int jz_nand_multiread(void *dev_id, unsigned int page,unsigned int pagecount,
					  unsigned char *mask,unsigned char *data,nand_page_info_t *info);
#endif //end STATUSPAGE
#endif
