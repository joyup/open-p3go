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
 

#include <os_api.h>
#include "threadprio.h"
#include "usb.h"
#include "udcbus.h"
#include "mass_storage.h"


#if 0
#define dprintf printf
#else
#define dprintf(x...)
#endif
#ifndef NULL
#define NULL
#endif

#define RECEIVE_CBW         0
#define RECEIVE_DATA        1
#define SENDING_CSW         2
#define SEND_DATA           3
#define RECEIVE_SERIES_DATA 4
#define SEND_SERIES_DATA    5
#define SENDED_CSW          6

//#define MUTI_THREAD


#ifdef MUTI_THREAD
volatile static u32 ssector;
volatile static u32 nsector;

#define MASS_TASK_PRIO  MASS_STORAGE_THREAD_PRIO //8
#define MASS_TASK_STK_SIZE	1024
OS_TASK_STACK MassTaskStack[MASS_TASK_STK_SIZE];
u8 err;
#endif

unsigned char mass_state = RECEIVE_CBW;
//unsigned char send_state = RECEIVE_CBW;
#define MAX_LUN_COUNT 4
PUDC_LUN pDev[MAX_LUN_COUNT] = { 0, 0, 0, 0};
unsigned char curLunCount = 0;

PIPE pipe[3] = {
	              {0,ENDPOINT_TYPE_CONTROL,8},
				  {0x81,ENDPOINT_TYPE_BULK,64},
				  {0x2,ENDPOINT_TYPE_BULK,64}
               };

unsigned int register_dev(PUDC_LUN pdev)
{
	if(pdev)
	{
		if(curLunCount > MAX_LUN_COUNT)
		{
			printf("warning:Max Logical Unit number is %d\r\n",MAX_LUN_COUNT);
			return -1;
		}
		pDev[curLunCount++] = pdev;
	}
	return 0;
} 
/*
 * USB mass storage class subclasses
 */
enum UMASS_SUBCLASS
{
	MASS_SUBCLASS_RBC = 1,	// Flash
	MASS_SUBCLASS_8020,	// CD-ROM
	MASS_SUBCLASS_QIC,	// Tape
	MASS_SUBCLASS_UFI,	// Floppy disk device
	MASS_SUBCLASS_8070,	// Floppy disk device
	MASS_SUBCLASS_SCSI	// Any device with a SCSI-defined command set
};


/*
 * USB mass storage class protocols
 */
enum UMASS_PROTOCOL
{
	MASS_PROTOCOL_CBI,	// Command/Bulk/Interrupt
	MASS_PROTOCOL_CBI_CCI,	// Command/Bulk/Interrupt
	MASS_PROTOCOL_BULK = 0x50	// Bulk-Only transport
};

static u32 epout, epin;

static USB_DeviceDescriptor devDesc = 
{
	sizeof(USB_DeviceDescriptor),
	DEVICE_DESCRIPTOR,	//1
	0x0200,     //Version 2.0
//	0x08,
//	0x01,
//	0x50,
	0x00,
	0x00,
	0x00,
	64,	/* Ep0 FIFO size */
	0x0525,
	0xa4a5,
	0x0320,
	0x01, 	//iManufacturer;
	0x02,   //iProduct;
	0x03,
	0x01
};

#define	CONFIG_DESCRIPTOR_LEN	(sizeof(USB_ConfigDescriptor) + \
				 sizeof(USB_InterfaceDescriptor) + \
				 sizeof(USB_EndPointDescriptor) * 2)

static struct {
	USB_ConfigDescriptor    configuration_descriptor;
	USB_InterfaceDescriptor interface_descritor;
	USB_EndPointDescriptor  endpoint_descriptor[2];
} __attribute__ ((packed)) confDesc = {
	{
		sizeof(USB_ConfigDescriptor),
		CONFIGURATION_DESCRIPTOR,
		CONFIG_DESCRIPTOR_LEN,
		0x01,
		0x01,
		0x04,
		0x80,	// Self Powered, no remote wakeup
		0xfa	// Maximum power consumption 200 mA
		//0x00
	},
	{
		sizeof(USB_InterfaceDescriptor),
		INTERFACE_DESCRIPTOR,
		0x00,
		0x00,
		0x02,	/* ep number */
		CLASS_MASS_STORAGE,
		MASS_SUBCLASS_SCSI,
		MASS_PROTOCOL_BULK,
		0x05
	},
	{
		{
			sizeof(USB_EndPointDescriptor),
			ENDPOINT_DESCRIPTOR,
			(1 << 7) | 1,// endpoint 2 is IN endpoint
			2, /* bulk */
			/* Transfer Type: Bulk;
			 * Synchronization Type: No Synchronization;
			 * Usage Type: Data endpoint
			 */
			//512, /* IN EP FIFO size */
			512,
			0
		},
		{
			sizeof(USB_EndPointDescriptor),
			ENDPOINT_DESCRIPTOR,
			(0 << 7) | 1,// endpoint 5 is OUT endpoint
			2, /* bulk */
			/* Transfer Type: Bulk;
			 * Synchronization Type: No Synchronization;
			 * Usage Type: Data endpoint
			 */
			512, /* OUT EP FIFO size */
			0
		}
	}
};
static USB_DeviceQualifierDescriptor devQualifyDesc =
{
	sizeof(USB_DeviceQualifierDescriptor),
	0x06,
	0x0200,
	0x00,
	0x00,
	0x00,
	0x40,
	0x01,
	0x00
};

/*
void send_dev_desc_string(int size)
{
	u16 str_ret[13] = {
		   0x031a,//0x1a=26 byte
		   0x0041,
		   0x0030,
		   0x0030,
		   0x0041,
		   0x0030,
		   0x0030,
		   0x0041,
		   0x0030,
		   0x0030,
		   0x0041,
		   0x0030,
		   0x0030
		  };
	dprintf("send_dev_desc_string size = %d\r\n",size);
	if(size >= 26)
		size = 26;
	str_ret[0] = (0x0300 | size);
	HW_SendPKT(0, str_ret,size);
	
}*/



/*
 * Command Block Wrapper (CBW)
 */
#define	CBWSIGNATURE	0x43425355		// "CBSU"
#define	CBWFLAGS_OUT	0x00			// HOST-to-DEVICE
#define	CBWFLAGS_IN	0x80			// DEVICE-to-HOST
#define	CBWCDBLENGTH	16

typedef struct
{
	u32 dCBWSignature;
	u32 dCBWTag;
	s32 dCBWDataXferLength;
	u8 bmCBWFlags;
	/* The bits of this field are defined as follows:
	 *     Bit 7 Direction - the device shall ignore this bit if the
	 *                       dCBWDataTransferLength zero, otherwise:
	 *         0 = Data-Out from host to the device,
	 *         1 = Data-In from the device to the host.
	 *     Bit 6 Obsolete. The host shall set this bit to zero.
	 *     Bits 5..0 Reserved - the host shall set these bits to zero.
	 */
	u8 bCBWLUN : 4,
	   reserved0 : 4;

	u8 bCBWCBLength : 5,
	   reserved1    : 3;
	u8 CBWCB[CBWCDBLENGTH];
} __attribute__ ((packed)) CBW;


/*
 * Command Status Wrapper (CSW)
 */
#define	CSWSIGNATURE			0x53425355	// "SBSU"
#define	CSWSIGNATURE_IMAGINATION_DBX1	0x43425355	// "CBSU"
#define	CSWSIGNATURE_OLYMPUS_C1		0x55425355	// "UBSU"

#define	CSWSTATUS_GOOD			0x0
#define	CSWSTATUS_FAILED		0x1
#define	CSWSTATUS_PHASE_ERR		0x2

typedef struct
{
	u32 dCSWSignature;
	u32 dCSWTag;
	u32 dCSWDataResidue;
	u8 bCSWStatus;
	/* 00h Command Passed ("good status")
	 * 01h Command Failed
	 * 02h Phase Error
	 * 03h and 04h Reserved (Obsolete)
	 * 05h to FFh Reserved
	 */
} __attribute__ ((packed)) CSW;


/*
 * Required UFI Commands
 */
#define	UFI_FORMAT_UNIT			0x04	// output
#define	UFI_INQUIRY			0x12	// input
#define	UFI_MODE_SELECT			0x55	// output
#define	UFI_MODE_SENSE_6		0x1A	// input
#define	UFI_MODE_SENSE_10		0x5A	// input
#define	UFI_PREVENT_MEDIUM_REMOVAL	0x1E
#define	UFI_READ_10			0x28	// input
#define	UFI_READ_12			0xA8	// input
#define	UFI_READ_CAPACITY		0x25	// input
#define	UFI_READ_FORMAT_CAPACITY	0x23	// input
#define	UFI_REQUEST_SENSE		0x03	// input
#define	UFI_REZERO_UNIT			0x01
#define	UFI_SEEK_10			0x2B
#define	UFI_SEND_DIAGNOSTIC		0x1D
#define	UFI_START_UNIT			0x1B
#define	UFI_TEST_UNIT_READY		0x00
#define	UFI_VERIFY			0x2F
#define	UFI_WRITE_10			0x2A	// output
#define	UFI_WRITE_12			0xAA	// output
#define	UFI_WRITE_AND_VERIFY		0x2E	// output
#define	UFI_ALLOW_MEDIUM_REMOVAL	UFI_PREVENT_MEDIUM_REMOVAL
#define	UFI_STOP_UNIT			UFI_START_UNIT
static u32 mass_get_state = 0;
#undef memalign
#undef free
#define memalign(x,y) alignAlloc(x,y) 
#define free(x) deAlloc(x)

#ifdef MUTI_THREAD

#define UDC_FRAME_SIZE ( 512 *0x100 )

//static u32 massBuf0[UDC_FRAME_SIZE / 4] __attribute__ ((aligned (32)));
//static u32 massBuf1[UDC_FRAME_SIZE / 4] __attribute__ ((aligned (32)));
static u32 *massBuf0 = NULL;
static u32 *massBuf1 = NULL;
#define udc_mass_alloc() \
do {                     \
	if(massBuf0 == NULL)   \
		massBuf0 = (u32 *)memalign(32,UDC_FRAME_SIZE); \
	if(massBuf1 == NULL)   \
		massBuf1 = (u32 *)memalign(32,UDC_FRAME_SIZE); \
	}while(0)
#define udc_mass_free() \
do{                     \
	if(massBuf0){         \
			free(massBuf0);   \
			massBuf0 = NULL;  \
	}                     \
	if(massBuf1){         \
			free(massBuf1);   \
			massBuf1 = NULL;  \
	}                     \
}while(0)
unsigned char *massbuf = 0;
volatile u8 udc_trigger = 1 , mass_trigger = 1;      //first buffer 0 !
OS_EVENT *sem_buf0 = NULL, *sem_buf1 = NULL, *sem_mass = NULL, *sem_device = NULL ;
#else
#define UDC_FRAME_SIZE ( 1024 * 64 )
static u32 *massBuf_ptr = 0;
#define udc_mass_alloc() \
do {                     \
	if(massBuf_ptr == NULL)   \
		massBuf_ptr = (u32 *)memalign(32,UDC_FRAME_SIZE); \
	}while(0)
#define udc_mass_free() \
do{                     \
	if(massBuf_ptr){         \
			free(massBuf_ptr);   \
			massBuf_ptr = NULL;  \
	}                    \
}while(0)
static unsigned char *massbuf_ptr = 0;

#endif

static CSW csw = {0};
static CBW cbw = {0};
unsigned int g_start_sector = 0;
unsigned short g_nr_sectors = 0;
unsigned int g_sector_size = 2048;
static u8 iscardin;

static u32 swap32(u32 n)
{
	return (((n & 0x000000ff) >> 0) << 24) |
	       (((n & 0x0000ff00) >> 8) << 16) |
	       (((n & 0x00ff0000) >> 16) << 8) |
	       (((n & 0xff000000) >> 24) << 0);
}

typedef struct _CAPACITY_DATA {
	u32 Blocks;
	u32 BlockLen;   
	
}CAPACITY_DATA;
typedef struct _READ_FORMAT_CAPACITY_DATA {
	u8 Reserve1[3];
	u8 CapacityListLen;
	CAPACITY_DATA CurMaxCapacity;
	CAPACITY_DATA CapacityData[30];
	
}READ_FORMAT_CAPACITY_DATA;

/*Interface */
unsigned short udc_device_state = 0;
#define UDC_HW_CONNECT    1
#define UDC_SW_CONNECT     0x10


#define WAIT_RECEIVECBW_FINISH    0
#define WAIT_SENDCSW_FINISH       1
#define WAIT_RECEIVE_FINISH       2
#define WAIT_SEND_FINISH          3

static void deinit_thread();
static void init_thread();


static inline unsigned int Handle_UFI_INQUIRY(unsigned handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	
	static const unsigned char inquiry[] = {
		0x00,	// Direct-access device (floppy)
		0x80,	// 0x80	// Removable Media
		0x00,
		0x00,	// UFI device
		0x29,
		0x00, 0x00, 0x00,
		'X', 'G', 'A', 'M', 'E', 'R', ' ', ' ',
		'P', '3', 'G', 'O', ' ', 'U', 'S', 'B',
		'-', 'D', 'I', 'S', 'K', ' ', ' ', ' ',
		'0', '1', '0', '0'
	};
	static const unsigned char inquiry_tf[] = {
		0x00,	// Direct-access device (floppy)
		0x80,	// 0x80	// Removable Media
		0x00,
		0x00,	// UFI device
		0x29,
		0x00, 0x00, 0x00,
		'X', 'G', 'A', 'M', 'E', 'R', ' ', ' ',
		'P', '3', 'G', 'O', ' ', 'T', 'F', ' ',
		'C', 'A', 'R', 'D', ' ', ' ', ' ', ' ',
		'0', '1', '0', '0'
	};
	u32 size = sizeof(inquiry);
	if(cbw.dCBWDataXferLength < sizeof(inquiry))
		size = cbw.dCBWDataXferLength;
	if(cbw.bCBWLUN)
		pBus->StartTransfer(handle,pipe[1].ep,(unsigned char *)inquiry_tf, size);
	else
		pBus->StartTransfer(handle,pipe[1].ep,(unsigned char *)inquiry, size);
	return 1;
	
}
static inline unsigned int Handle_UFI_REQUEST_SENSE(unsigned handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	static unsigned char sense[] = {
		0x70,
		0x00,
		0x06,
		0x00, 0x00, 0x00, 0x00,
		0x0a,
		0x00, 0x00, 0x00, 0x00,
		0x28,
		0x00,
		0x00,
		0x00, 0x00, 0x00
	};
	PUDC_LUN pdev = pDev[cbw.bCBWLUN];

	if ( !(pdev->CheckDevState(handle)) ) {
		sense[2] = 0x02;
		sense[12] = 0x3a;
		iscardin = 1;
	}
	else
	{
		sense[2] = 0x06;
		sense[12] = 0x28;
		iscardin = 0;
	}

	pBus->StartTransfer(handle,pipe[1].ep,(unsigned char *)sense,sizeof(sense));	
	return 1;
	
}

static inline unsigned int Handle_UFI_READ_CAPACITY(unsigned handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	DEVINFO devinfo;
	PUDC_LUN pdev = pDev[cbw.bCBWLUN]; 
	static u32 resp[2];
	pdev->GetDevInfo((unsigned int)pdev,&devinfo);
	pdev->sectorsize = devinfo.sectorsize;
	g_sector_size = pdev->sectorsize;
	
	resp[0] = swap32(devinfo.partsize-1); /* last sector */
	//resp[1] = swap32(512);		/* sector size */
	resp[1] = swap32(g_sector_size);		/* sector size */
	if ( !(pdev->CheckDevState(handle)) ) {
  		csw.dCSWDataResidue = cbw.dCBWDataXferLength;
		csw.bCSWStatus = CSWSTATUS_FAILED;
		//if sd out,shall send stall!!
		pBus->StartTransfer(handle,0xff,(unsigned char *)resp, sizeof(resp));
		return 0;
	}
	else
	{
		pBus->StartTransfer(handle,pipe[1].ep,(unsigned char *)resp, sizeof(resp));
	}
	return 1;
	
}

static inline unsigned int Handle_CSW(unsigned int handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	PUDC_LUN pdev = pDev[cbw.bCBWLUN];

	pBus->StartTransfer(handle,pipe[1].ep,(unsigned char *)&csw, sizeof(CSW));
	return 1;
	
}

static inline unsigned int Handle_UFI_TEST_UNIT_READY(unsigned int handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	PUDC_LUN pdev = pDev[cbw.bCBWLUN]; 
	
	if ( cbw.bCBWLUN > 0 )
	{
		if ( iscardin == 0 ) {
			if ( !(pdev->CheckDevState(handle)) ) {
				csw.bCSWStatus = CSWSTATUS_FAILED;
			}
		}
		else
			csw.bCSWStatus = iscardin;
	}
	else 
		csw.bCSWStatus = CSWSTATUS_GOOD;
    Handle_CSW(handle);
	mass_state = SENDED_CSW;
	if(pdev->FlushDev)
		pdev->FlushDev(handle);

	return 1;
}

static inline unsigned int Handle_UFI_READ_10(unsigned handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	PUDC_LUN pdev = pDev[cbw.bCBWLUN];
	unsigned int sectors;
	g_start_sector =
		((unsigned int)cbw.CBWCB[2] << 24) |
		((unsigned int)cbw.CBWCB[3] << 16) |
		((unsigned int)cbw.CBWCB[4] << 8) |
		(unsigned int) cbw.CBWCB[5];
	
	g_nr_sectors =
		((unsigned short)cbw.CBWCB[7] << 8) | (unsigned short)cbw.CBWCB[8];
	g_sector_size = pdev->sectorsize;
	
	sectors = UDC_FRAME_SIZE / g_sector_size;
	if(sectors > g_nr_sectors)
		sectors = g_nr_sectors;
	
#ifdef MUTI_THREAD
	massbuf = (unsigned char *)((unsigned int)massBuf0 | 0xa0000000);
	//dprintf("read g_start_sector = %x\n g_nr_sectors = %x sectors = %x\n",g_start_sector,g_nr_sectors,sectors);
	os_SemaphorePend(sem_device, 0, &err);
#endif
	pdev->ReadDevSector((unsigned int)pdev,massbuf_ptr,g_start_sector,sectors);

#ifdef MUTI_THREAD
	os_SemaphorePost(sem_device);
#endif
	pBus->StartTransfer(handle,pipe[1].ep, (unsigned char *)massbuf_ptr,
						sectors * g_sector_size);
	g_start_sector += sectors;
	g_nr_sectors -= sectors;
	mass_state = SEND_DATA;
	return 1;
	
}

static inline unsigned int Handle_UFI_WRITE_10(unsigned handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	PUDC_LUN pdev = pDev[cbw.bCBWLUN]; 
	unsigned int sendcount;

	unsigned int sectors;
	
	g_start_sector =
		((u32)cbw.CBWCB[2] << 24) |
		((u32)cbw.CBWCB[3] << 16) |
		((u32)cbw.CBWCB[4] << 8) |
		(u32)cbw.CBWCB[5];
	g_nr_sectors   =
		((u16)cbw.CBWCB[7] << 8) | (u16)cbw.CBWCB[8];
	g_sector_size = pdev->sectorsize;
	
	sendcount = g_nr_sectors * g_sector_size;
	sectors = g_nr_sectors;
	
	if(sendcount > UDC_FRAME_SIZE)
	{
		sendcount = UDC_FRAME_SIZE;
		sectors = sendcount / g_sector_size;
		
	}
	
//	dprintf("write s:%x n:%x c:%x\n", g_start_sector, g_nr_sectors,sectors);

#ifdef MUTI_THREAD
	if ( udc_trigger == 1 )
	{
		massbuf = (unsigned char *)((unsigned int)massBuf1 | 0xa0000000);
		os_SemaphorePend(sem_buf1, 0, &err);
	}
	else 
	{
		massbuf = (unsigned char *)((unsigned int)massBuf0 | 0xa0000000);
		os_SemaphorePend(sem_buf0, 0, &err);
	}
//	printf("get here? \n");
#endif
	
	pBus->StartTransfer(handle,pipe[2].ep, massbuf_ptr,sendcount);
	if(g_nr_sectors)
		mass_state = RECEIVE_DATA;
	else
	{
		mass_state = SENDED_CSW;
		Handle_CSW(handle);
	}
	
	return 1;
	
}


static inline unsigned int Handle_UFI_READ_FORMAT_CAPACITY(unsigned handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	DEVINFO devinfo;
	PUDC_LUN pdev = pDev[cbw.bCBWLUN]; 
	READ_FORMAT_CAPACITY_DATA readfcd;
	if(pdev->CheckDevState(handle) == 0)
	{
		csw.dCSWDataResidue = cbw.dCBWDataXferLength;
		csw.bCSWStatus = CSWSTATUS_FAILED;
		pBus->StartTransfer(handle,0xff,0, 0);
		return 0;
	}	 
#if 0
  		csw.dCSWDataResidue = cbw.dCBWDataXferLength;
		csw.bCSWStatus = CSWSTATUS_FAILED;
		//if sd out,shall send stall!!
		pBus->StartTransfer(handle,0xff,0, 0);
		return 0;
#endif
	
	memset(&readfcd,0,sizeof(readfcd));
	readfcd.CapacityListLen = 0x8;                 //M by yliu
	pdev->GetDevInfo((unsigned int)pdev,&devinfo);
	readfcd.CapacityData[0].Blocks = devinfo.partsize-1;
	readfcd.CapacityData[0].BlockLen = devinfo.sectorsize;
	readfcd.CurMaxCapacity.Blocks = devinfo.partsize-1;
	
	pdev->sectorsize = devinfo.sectorsize;
	g_sector_size = pdev->sectorsize;
	
	//printf("flashinfo.dwFSTotalSectors =%d flashinfo.dwDataBytesPerSector=%d\n",devinfo.partsize,devinfo.sectorsize);
	//printf("%s:g_sector_size = %d\n",__FUNCTION__,g_sector_size);
	
	readfcd.CurMaxCapacity.BlockLen = g_sector_size;
	readfcd.CurMaxCapacity.BlockLen = (readfcd.CurMaxCapacity.BlockLen << 8) | 0x2;
    
	int size = cbw.bCBWCBLength;
	if(size > sizeof(READ_FORMAT_CAPACITY_DATA))
		size = sizeof(READ_FORMAT_CAPACITY_DATA);

	csw.bCSWStatus = CSWSTATUS_GOOD;
	csw.dCSWDataResidue = 0x0;
	csw.dCSWTag = cbw.dCBWTag;	
	pBus->StartTransfer(handle,pipe[1].ep,(u8 *) &readfcd, cbw.dCBWDataXferLength);
	return 1;
	
}

static inline unsigned int Handle_UFI_MODE_SENSE_6(unsigned handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;

	static unsigned char sensedata[8] = {
		0x00,0x06, // lenght
		0x00,      // default media
		0x00,      // bit 7 is write protect
        0x00,0x00,0x00,0x00 //reserved
	};
	pBus->StartTransfer(handle,pipe[1].ep,sensedata, sizeof(sensedata));
    return 1;
}

static inline unsigned int USB_HandleCBW(unsigned int handle)
{
	if (cbw.dCBWSignature != CBWSIGNATURE)
		return 0;
    
	csw.dCSWSignature = CSWSIGNATURE;
	csw.bCSWStatus = CSWSTATUS_GOOD;
	csw.dCSWTag = cbw.dCBWTag;
	csw.dCSWDataResidue = 0;
//	if (cbw.bCBWLUN != 0 || (cbw.CBWCB[1] & 0xe0 ) != 0 )
//		printf("AAAAAAAAAAAAA %d \n",cbw.bCBWLUN);

	//printf("cbw.Signature:%08x\n", cbw.dCBWSignature);
	//printf("cbw.dCBWTag:%08x\n", cbw.dCBWTag);
	//printf("cbw.dCBWDataXferLength:%x\n", cbw.dCBWDataXferLength);
	//printf("cbw.bmCBWFlags:%08x\n", cbw.bmCBWFlags);
	//printf("cbw.bCBWLUN:%d\n", cbw.bCBWLUN);
	//printf("cbw.bCBWCBLength:%d\n", cbw.bCBWCBLength);
	//printf("cbw.CBWCB[0]:%02x\n", cbw.CBWCB[0]);

	mass_state = SENDING_CSW;
	
	switch (cbw.CBWCB[0]) {
	case UFI_INQUIRY:
		Handle_UFI_INQUIRY(handle);
		break;
	case UFI_REQUEST_SENSE:
		Handle_UFI_REQUEST_SENSE(handle);
		break;
	case UFI_READ_CAPACITY:
		if ( !Handle_UFI_READ_CAPACITY(handle))
		{
			Handle_CSW(handle);
			mass_state = SENDED_CSW;
			csw.bCSWStatus = CSWSTATUS_GOOD;
		}
		break;
	case UFI_READ_10:
		Handle_UFI_READ_10(handle);
		break;
		
	case UFI_WRITE_10:
	case UFI_WRITE_AND_VERIFY:
		dprintf("UFI_WRITE_10\r\n");
		
		Handle_UFI_WRITE_10(handle);
		break;
	case UFI_READ_FORMAT_CAPACITY:
		if(!Handle_UFI_READ_FORMAT_CAPACITY(handle))
		{
			Handle_CSW(handle);
			mass_state = SENDED_CSW;
			csw.bCSWStatus = CSWSTATUS_GOOD;
		}	
//		Handle_CSW(handle);
//		mass_state = SENDED_CSW;

		break;
	case UFI_MODE_SENSE_10:
	case UFI_MODE_SENSE_6:
		Handle_UFI_MODE_SENSE_6(handle);
		break;
	case UFI_TEST_UNIT_READY:
		Handle_UFI_TEST_UNIT_READY(handle);
		break;
	default:
		Handle_CSW(handle);
		mass_state = SENDED_CSW;
		
		break;
	}

}


static unsigned int mass_detect(unsigned int handle,unsigned int stat)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	unsigned char i;
    // udc gpio detect insert
//	dprintf("mass_detect udc_device_state = %x\n",udc_device_state);
	if(stat == UDC_JUDGE)
	{	if((udc_device_state & UDC_HW_CONNECT) == 0)
		{
			
			pBus->EnableDevice(handle);
			udc_device_state |= UDC_HW_CONNECT;
			
		}
	}else if(stat == UDC_REMOVE) //udc gpio detect remove 
	{
		pBus->DisableDevice(handle);
		if(udc_device_state & UDC_HW_CONNECT)
		{
			udc_device_state &= ~UDC_HW_CONNECT;
		}
		
		if(udc_device_state & UDC_SW_CONNECT)
		{
			for(i = 0;i < curLunCount; i++)
			{
				if(pDev[i])
					pDev[i]->DeinitDev((unsigned int)pDev[i]);
			}
			
			udc_device_state &= ~UDC_SW_CONNECT;
		}
		
	}
	else
		return 0;
	return 1;	
}

static unsigned int mass_reset(unsigned int handle)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	unsigned char ret = 1, i = 0;
	printf("mass reset \r\n");
	init_thread();
#ifdef MUTI_THREAD
	unsigned char err;
	os_SemaphorePend(sem_device,0,&err);
#endif
	pBus->InitEndpointSuppost(handle,&(pipe[0].ep),pipe[0].ep_type,&(pipe[0].max_pkg));
	pBus->InitEndpointSuppost(handle,&(pipe[1].ep),pipe[1].ep_type,&(pipe[1].max_pkg));
	pBus->InitEndpointSuppost(handle,&(pipe[2].ep),pipe[2].ep_type,&(pipe[2].max_pkg));

	devDesc.bMaxPacketSize0 = pipe[0].max_pkg;
	confDesc.endpoint_descriptor[0].bEndpointAddress = pipe[1].ep;
	confDesc.endpoint_descriptor[0].wMaxPacketSize = pipe[1].max_pkg;
	
	confDesc.endpoint_descriptor[1].bEndpointAddress = pipe[2].ep;
	confDesc.endpoint_descriptor[1].wMaxPacketSize = pipe[2].max_pkg;

#ifndef MUTI_THREAD
	massbuf_ptr = (unsigned char *)((unsigned int)massBuf_ptr | 0xa0000000);
#else
//	udc_trigger = 0;
//	mass_trigger = 0;
/*
	sem_buf0 = os_SemaphoreCreate(1);
	sem_buf1 = os_SemaphoreCreate(1);
	sem_mass = os_SemaphoreCreate(0);
	sem_device = os_SemaphoreCreate(1);
*/
#endif

	if(curLunCount == 0)
		return;
	ret = curLunCount;
	
	if(ret)
	{
		for(i = 0;i < curLunCount; i++)
		{
			if(pDev[i])
				pDev[i]->InitDev((unsigned int)pDev[i]);
			
		}
		
		devDesc.iSerialNumber = 3;	
		
	}
	udc_device_state |= UDC_SW_CONNECT;
	mass_state = RECEIVE_CBW;

	pBus->StartTransfer(handle,pipe[2].ep,(unsigned char *)&cbw,sizeof(cbw));	
	dprintf("iSerialNumber = %d\n",devDesc.iSerialNumber);
#if MUTI_THREAD
	os_SemaphorePost(sem_device);
#endif
	return (unsigned int )ret;
	
}
static unsigned int mass_suspend(unsigned int handle)
{
	unsigned char i;
	printf("mass_suspend!\n");
	deinit_thread();
	if(udc_device_state & UDC_SW_CONNECT)
	{
		for(i = 0;i < curLunCount; i++)
			if(pDev[i])
				pDev[i]->DeinitDev((unsigned int)pDev[i]);
		udc_device_state &= ~UDC_SW_CONNECT;
	}
	return 1;
}
static unsigned int set_udc_speed(unsigned stat)
{
	switch(stat)
	{
	case UDC_HIGHSPEED:
		printf("UDC HIGHSPEED\r\n");
		break;
		
	case UDC_FULLSPEED:
		printf("UDC FULLSPEED\r\n");
		break;
		
	}
	return 1;
}

static inline void get_dev_descriptor(unsigned int handle,USB_DeviceRequest *dreq)
{
    PUDC_BUS pBus = (PUDC_BUS)handle;
    unsigned short size = dreq->wLength;
	
	if(size < sizeof(devDesc))
	{
		devDesc.bLength = size;
		pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)&devDesc, size);
	}
	
	else
	{
		devDesc.bLength = sizeof(devDesc);
		pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)&devDesc, sizeof(devDesc));
	}
	
}

static inline void get_dev_configuration(unsigned int handle,USB_DeviceRequest *dreq)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	switch (dreq->wLength) {
	case 9:
		pBus->StartTransfer(handle,pipe[0].ep, (unsigned char *)&confDesc, 9);
		break;
	case 8:
		pBus->StartTransfer(handle,pipe[0].ep, (unsigned char *)&confDesc, 8);
		break;
	default:
		pBus->StartTransfer(handle,pipe[0].ep, (unsigned char *)&confDesc, sizeof(confDesc));
		break;
	}
}

static inline void get_dev_qualify_descitptor(unsigned int handle,USB_DeviceRequest *dreq)
{
    PUDC_BUS pBus = (PUDC_BUS)handle;
    pBus->StartTransfer(handle,pipe[0].ep, (unsigned char *)&devQualifyDesc, sizeof(devQualifyDesc));	
}


static inline unsigned int usbHandleClassDevReq(unsigned int handle,unsigned char *buf)
{
	u8 scsiLUN = 0;
	PUDC_BUS pBus = (PUDC_BUS)handle;
	switch (buf[1]) {
	case 0xfe:
		scsiLUN = curLunCount - 1;
		dprintf("Get max lun %d %x\n",scsiLUN,handle,pipe[0].ep);
		if (buf[0] == 0xa1)
			pBus->StartTransfer(handle,pipe[0].ep, (unsigned char *)&scsiLUN, 1);
		break;
	case 0xff:
		dprintf("Mass storage reset\n");
		break;
	}
	return 1;
	
}
static inline void get_dev_descriptor_string(unsigned int handle,USB_DeviceRequest *dreq)
{
	int size = dreq->wLength;
	PUDC_BUS pBus = (PUDC_BUS)handle;
	static u16 str_ret[] = {
		0x0336,    //0x1a=26 byte
		'F',
		'i',
		'l',
		'e',
		'-',
		'b',
		'a',
		'c',
		'k',
		'e',
		'd',
		'S',

		't',
		'o',
		'r',
		'a',
		'g',
		'e',
		'G',

		'a',
		'd',
		'g',
		'e',
		't'
	};
	static u16 str_lang[] = {
		0x0304,
		0x0409
	};

	static u16 str_isernum[] = {
		0x031a,
		'3',
		'2',
		'3',
		'8',
		'2',
		'0',
		'4',
		'7',
		'4',
		'0',
		'7',
		'7'
	};

	printf("send_dev_desc_string size = %d type %d \r\n",size,dreq->wValue & 0xff);

	switch ( dreq->wValue & 0xff )
	{
	case 0:       //land ids
		if ( size > sizeof(str_lang) )
			pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)str_lang,sizeof(str_lang));
		else
			pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)str_lang,size);
		return;
		break;
	case 1:       //iserialnumber
		if(size >= sizeof(str_isernum))
			size = sizeof(str_isernum);
		pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)str_isernum,size);
		break;

	case 2:       //iproduct
		if(size >= 36)
			size = 36;
		str_ret[0] = (0x0300 | size);
		pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)str_ret,size);
		break;
	case 3:       //iserialnumber
		if(size >= sizeof(str_isernum))
			size = sizeof(str_isernum);
		pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)str_isernum,size);
		break;
	case 0xee:    //microsoft OS!
		str_isernum[0] = (0x0300 | size);
		pBus->StartTransfer(handle,pipe[0].ep,(unsigned char *)str_isernum,size);
		break;
	}
}

static unsigned int udc_setup_handle(unsigned int handle,unsigned int stat,unsigned char *bufaddr,unsigned int len)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)bufaddr;
	if(len != 8)
		return 0;
	if(dreq->bmRequestType == 0xa1)
	{
		if(dreq->bRequest == 0xfe)
		{
			return usbHandleClassDevReq(handle,bufaddr);
			
		}
		return;
		
		
	}
	if(dreq->bmRequestType == 0x21)
	{
		if(dreq->bRequest == 0xff)
		{
			return usbHandleClassDevReq(handle,bufaddr);
		}
		
	}
	
	switch (dreq->bRequest) {
	case GET_DESCRIPTOR:
		if (dreq->bmRequestType == 0x80)	/* Dev2Host */
			switch(dreq->wValue >> 8) 
			{
			case DEVICE_DESCRIPTOR:
				dprintf("get device!\n");
				get_dev_descriptor(handle,dreq);
				break;
			case CONFIGURATION_DESCRIPTOR:
				dprintf("get config!\n");
				get_dev_configuration(handle,dreq);
				break;
			case STRING_DESCRIPTOR:
				dprintf("get string!\n");
				get_dev_descriptor_string(handle,dreq);
				break;
			case DEVICE_QUALIFIER_DESCRIPTOR:
				dprintf("get qualify!\n");
				get_dev_qualify_descitptor(handle,dreq);
				break;
				
			}
		
		break;
	case SET_ADDRESS:
		dprintf("\nSET_ADDRESS!");
		pBus->SetAddress(handle,dreq->wValue);
		break;
		
	case GET_STATUS:
		switch (dreq->bmRequestType) {
		case 80:	/* device */
		  mass_get_state = 0x1;
			pBus->StartTransfer(handle,pipe[0].ep,(u8*)&mass_get_state,2);			
			break;
		case 81:	/* interface */
		case 82:	/* ep */
			mass_get_state = 0;
			pBus->StartTransfer(handle,pipe[0].ep,(u8*)&mass_get_state,2);			
			break;
		}

		break;
   
	case CLEAR_FEATURE:
		printf("CLEAR_FEATURE!\r\n");
		break;
		
	case SET_CONFIGURATION:
		printf("SET_CONFIGURATION!\r\n");
//		pBus->StartTransfer(handle,pipe[0].ep,massbuf,0);			

		break;
		
	case SET_INTERFACE:
		printf("SET_INTERFACE!\r\n");
		break;
	case SET_FEATURE:
		printf("SET_FEATURE!\r\n");
		break;
	default:	
        printf("protal isn't surporst\r\n");
		
	}
	return 1;
}


static unsigned int handle_receive_data(unsigned handle,unsigned char *buf,unsigned int len)
{
	PUDC_LUN pdev = pDev[cbw.bCBWLUN]; 
	PUDC_BUS pBus = (PUDC_BUS)handle;
	unsigned int ret;
	unsigned int sectors;
	g_sector_size = pdev->sectorsize;
	
	sectors = len / g_sector_size;
   
//	ret = pdev->WriteDevSector((unsigned int)pdev,buf,g_start_sector,sectors);
#ifndef MUTI_THREAD
	ret = pdev->WriteDevSector((unsigned int)pdev,buf,g_start_sector,sectors);
#else
	if ( udc_trigger == 1 )
	{
		udc_trigger = 0;
		os_SemaphorePost(sem_buf1);
	}
	else 
	{
		udc_trigger = 1;
		os_SemaphorePost(sem_buf0);
	}
	
	os_SemaphorePend(sem_device, 0, &err);    //lock device!
	ssector = g_start_sector;
	nsector = sectors;
	os_SemaphorePost(sem_mass);                //wake mass task
#endif

	g_start_sector += sectors;
	g_nr_sectors -= sectors;
	if(g_nr_sectors)
	{
		sectors = g_nr_sectors * g_sector_size;
#ifdef MUTI_THREAD
		if ( udc_trigger == 1 )
		{
			massbuf = (unsigned char *)((unsigned int)massBuf1 | 0xa0000000);
			os_SemaphorePend(sem_buf1, 0, &err);
		}
		else 
		{
			massbuf = (unsigned char *)((unsigned int)massBuf0 | 0xa0000000);
			os_SemaphorePend(sem_buf0, 0, &err);
		}
#endif
		if(sectors > UDC_FRAME_SIZE)
			pBus->StartTransfer(handle,pipe[2].ep,massbuf_ptr,UDC_FRAME_SIZE);
		else
			pBus->StartTransfer(handle,pipe[2].ep,massbuf_ptr,sectors);
		
	}else
	{
		mass_state = SENDED_CSW;
		//send csw data
		Handle_CSW(handle);
		
		
	}
	return ret;
}

static unsigned int handle_receive(unsigned handle,unsigned char *buf,unsigned int len)
{
	//printf("mass_state = %d,RECEIVE_CBW = %d\n",mass_state,RECEIVE_CBW);
	
	if(mass_state == RECEIVE_CBW)
	{
		return USB_HandleCBW(handle);
		
	}
	if(mass_state == RECEIVE_DATA) 
	{
		return handle_receive_data(handle,buf,len);
	}
	
	return 1;
}
static unsigned int handle_send_data(unsigned handle,unsigned char *buf,unsigned int len)
{
	PUDC_LUN pdev = pDev[cbw.bCBWLUN]; 
	PUDC_BUS pBus = (PUDC_BUS)handle;
	unsigned short sectors;
	
	g_sector_size = pdev->sectorsize; 
	sectors = UDC_FRAME_SIZE / g_sector_size;
	if(g_nr_sectors)
	{
		
		if(sectors > g_nr_sectors)
			sectors = g_nr_sectors;
		
		
		pdev->ReadDevSector((unsigned int)pdev,massbuf_ptr,g_start_sector,sectors);
		pBus->StartTransfer(handle,pipe[1].ep, (unsigned char *)massbuf_ptr,
							sectors * g_sector_size);
		g_start_sector += sectors;	
		g_nr_sectors -= sectors;
	
	}else
	{
		mass_state = SENDED_CSW;
		//send csw data
		Handle_CSW(handle);
			
	}
	
	return 1;
}

static unsigned int handle_send(unsigned handle,unsigned char *buf,unsigned int len)
{
	PUDC_BUS pBus = (PUDC_BUS)handle;
	if(mass_state == SEND_DATA)
	{
		return handle_send_data(handle,buf,len);
		
	}
	if(mass_state == SENDING_CSW)
	{
		mass_state = SENDED_CSW;
		Handle_CSW(handle);
		
	}
	
	if(mass_state == SENDED_CSW)
	{
		mass_state = RECEIVE_CBW;
		pBus->StartTransfer(handle,pipe[2].ep,(unsigned char *)&cbw,sizeof(cbw));
	}
	
	return 1;
}

static unsigned int notify(unsigned int handle,unsigned int stat,unsigned char *bufaddr,unsigned int len)
{
//	printf("Bus notify stat :%x \n",stat);
	
	if((stat < 0x200) && (stat >=0x100))
	   return mass_detect(handle,stat);
	if(stat == UDC_RESET)
		return mass_reset(handle);
	if(stat == UDC_SUSPEND)
		return mass_suspend(handle);
	if((stat >= UDC_FULLSPEED) && (stat <= UDC_HIGHSPEED))
		return set_udc_speed(stat);
	if(stat == UDC_SETUP_PKG_FINISH)
		return udc_setup_handle(handle,stat,bufaddr,len);
	if(stat == UDC_PROTAL_RECEIVE_FINISH)
		return handle_receive(handle,bufaddr,len);
	if(stat == UDC_PROTAL_SEND_FINISH)
		return handle_send(handle,bufaddr,len);
	return 0;
}

#ifdef MUTI_THREAD

static void mass_task(void *arg)
{
	PUDC_LUN pdev ; 
//	MASS_ARGS *p ;
	u8 *Massbuf;
	u8 err;
	while (1) {
		os_SemaphorePend(sem_mass, 0, &err);
		pdev = pDev[cbw.bCBWLUN];
		g_sector_size = pdev->sectorsize; 
		
		if ( mass_trigger == 0)
		{
			Massbuf = (unsigned char *)((unsigned int)massBuf0 | 0xa0000000);
			os_SemaphorePend(sem_buf0, 0, &err);
			pdev->WriteDevSector((unsigned int)pdev,Massbuf,ssector,nsector);
			mass_trigger = 1;
			os_SemaphorePost(sem_buf0);
		}
		else
		{
			Massbuf = (unsigned char *)((unsigned int)massBuf1 | 0xa0000000);
			os_SemaphorePend(sem_buf1, 0, &err);
			pdev->WriteDevSector((unsigned int)pdev,Massbuf,ssector,nsector);
			mass_trigger = 0;
			os_SemaphorePost(sem_buf1);
		}
		os_SemaphorePost(sem_device);
	}
	
}
#endif
static unsigned char FInitThread = 0;
static void init_thread()
{
	if(FInitThread == 0)
	{
		#ifdef MUTI_THREAD
		udc_mass_alloc();
		sem_buf0 = os_SemaphoreCreate(1);
		sem_buf1 = os_SemaphoreCreate(1);
		sem_mass = os_SemaphoreCreate(0);
		sem_device = os_SemaphoreCreate(1);
		    
		os_TaskCreate(mass_task, (void *)0,
			     (void *)&MassTaskStack[MASS_TASK_STK_SIZE - 1],
			     MASS_TASK_PRIO);
		#else
    udc_mass_alloc();
    #endif

	}
	FInitThread = 1;	
}
static void deinit_thread()
{
	if(FInitThread)
	{
		#ifdef MUTI_THREAD
		unsigned char err;
		os_TaskDelete(MASS_TASK_PRIO);
		os_SemaphoreDelete(sem_buf0,OS_DELETE_ALWAYS,&err);
		sem_buf0 = NULL;
		os_SemaphoreDelete(sem_buf1,OS_DELETE_ALWAYS,&err);
		sem_buf1 = NULL;
		os_SemaphoreDelete(sem_mass,OS_DELETE_ALWAYS,&err);
		sem_mass = NULL;
		os_SemaphoreDelete(sem_device,OS_DELETE_ALWAYS,&err);
		sem_device = NULL;
		udc_mass_free();
		#else
		udc_mass_free();
		#endif
		
	}
	FInitThread = 0;
}
static unsigned char InitMassStorage = 0;

void init_mass_storage()
{

	if(InitMassStorage == 0)
	{
		CreateDevice(notify);

	}
	InitMassStorage = 1;
}
