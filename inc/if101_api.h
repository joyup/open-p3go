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
 * IF101 Operation Layer Definition Header
 */

#ifndef IF101_OPERATION_H
#define IF101_OPERATION_H

//#include "IF101_Communication.h"
typedef enum{
	INNO_NO_ERROR = 0,
	INNO_GENERAL_ERROR = 1,
	INNO_TIMEOUT_ERROR = 2,
	INNO_FW_OPERATION_ERROR = 3,
	INNO_PARAMETER_ERROR = 4,
}INNO_RET;

typedef struct 
{
	unsigned char select_ch;
	
	unsigned char enable_0;
	unsigned char ts_start_0;
	unsigned char ts_count_0;
	unsigned char modu_type_0;
	unsigned char rs_mode_0;
	unsigned char outer_mode_0;
	unsigned char ldpc_mode_0;
	
	unsigned char enable_1;
	unsigned char ts_start_1;
	unsigned char ts_count_1;
	unsigned char modu_type_1;
	unsigned char rs_mode_1;
	unsigned char outer_mode_1;
	unsigned char ldpc_mode_1;
}channel_config;

/*
 * Init and Deinit I2C interface 
 *
 * Parameter:
 *		enable <in>		: enable or disable I2C interface
 *
 * Example:
 *		INNO_I2C_Init(1);	// Enable I2C interface
 *		INNO_I2C_Init(0);	// Enable I2C interface
 *
 */
INNO_RET INNO_I2C_Init(int enable);


/*
 * Write One Byte to a I2C register of IF101 
 * IF101 is as slave in I2C communication and IF101 slave address is 0x10;
 *
 * Parameter:
 *		addr <in>		: Register address.
 *		data <in>		: The data want to write to addr.
 *
 */
INNO_RET INNO_I2C_Write(unsigned char addr, unsigned char data);



/*
 * Read one byte data from a I2C register of IF101 
 * IF101 is as slave in I2C communication and IF101 slave address is 0x10;
 *
 * Parameter:
 *		addr <in>		: Register address.
 *		data <out>		: The data want to write to addr.
 *
 */
INNO_RET INNO_I2C_Read(unsigned char addr, unsigned char *data);


/*
 * Init and Deinit SPI interface 
 *
 * Parameter:
 *		enable <in>		: enable or disable SPI interface
 *
 * Example:
 *		INNO_SPI_Init(1);	// Enable SPI interface
 *		INNO_SPI_Init(0);	// Enable SPI interface
 *
 */
INNO_RET INNO_SPI_Init(int enable);



/*
 * Write one byte to SPI bus 
 *
 * Parameter:
 *		data <in>		: the data write to SPI bus
 *
 */
INNO_RET INNO_SPI_Write_One_Byte(unsigned char data);



/*
 * Read one byte from SPI bus 
 *
 * Parameter:
 *		data <out>		: the data read from SPI bus 
 *
 */
INNO_RET INNO_SPI_Read_One_Byte(unsigned char *data);


/*
 * Read several bytes from SPI bus 
 *
 * Parameter:
 *		buffer <out>	: the buffer point to store data from SPI bus 
 *		len <in>		: how many bytes will be read from SPI 
 *
 * NOTE:
 *		Please make sure SPI chip select is active(low) in the whole process of read bytes from SPI 
 */
INNO_RET INNO_SPI_Read_Bytes(unsigned char *buffer, int len);


/**********************************************************************************************************
 *			I2C Address:
**********************************************************************************************************/
#define I2C_COMMUNICATION_PAGE		0x2
#define I2C_CMD_DATA_ADDR			0x98
#define I2C_CMD_STATUS_ADDR			0x99
#define I2C_RSP_DATA_ADDR			0x9A
#define I2C_RSP_STATUS_ADDR			0x9B

/**********************************************************************************************************
 *			I2C CMD and RSP status bit
**********************************************************************************************************/
#define CMD_CODE					0x84
#define CMD_PAYLOAD					0x80
#define RSP_DATA_VALID				0x80
#define CMD_BUSY					0x80

/**********************************************************************************************************
*			I2C CMD Type
**********************************************************************************************************/
#define CMD_GET_CHANNEL_CONFIG		0x1
#define CMD_SET_PM					0x2
#define CMD_GET_FW_VER				0x4
#define CMD_SET_CHANNEL_CONFIG		0x5
#define CMD_SET_FREQUENCY			0x6
#define CMD_GET_FW_CHECK_SUM		0xB
#define CMD_UPDATE_FW_FLASH			0x81


/**********************************************************************************************************
*			SPI CMD Type
**********************************************************************************************************/
#define READ_INT_STATUS				0x0A	

#define READ_LG0_LEN_LOW			0x15
#define READ_LG0_LEN_MID			0x16
#define READ_LG0_LEN_HIGH			0x17

#define READ_LG1_LEN_LOW			0x18
#define READ_LG1_LEN_MID			0x19
#define READ_LG1_LEN_HIGH			0x1A

#define FETCH_LG0_DATA				0x99
#define FETCH_LG1_DATA				0x9B

// INT0_STATUS Bit Mask
#define LG0_DATA_RDY				(0x01)
#define LG1_DATA_RDY				(0x02)


/************************* Structure Definition ****************************/

typedef enum{
	CHANNEL_0 = 0,
	CHANNEL_1 = 1,
}CHANNEL_ID;

typedef enum{
	PM_PAUSE = 0x80,
	PM_RESUME = 0x40,
}PM_MODE;

typedef struct{
	int sync;						// sync status.
	unsigned int current_frequency;		// current Tuner working frequency
	int signal_strength;				// signal strength
	int ldpc_err_percent;				// LDPC error percentage
}SYS_STATUS;

typedef struct{
	unsigned int ldpc_total_count;
	unsigned int ldpc_error_count;
	unsigned int rs_total_count;
	unsigned int rs_error_count;
}ERR_INFO;

/************************* Function Definition ****************************/

/*
 * Init SPI, I2C and test communication
 *
 * Parameter:
 *		void
 *
 * NOTE:
 *			
 *
 */
INNO_RET INNO_Init(void);


/*
 * Deinit SPI, I2C
 *
 * Parameter:
 *		void
 *
 */
INNO_RET INNO_Deinit(void);


/* 
 * Read Data
 *
 * Parameter:
 *		channel_id <in>			: read data from a certain channel
 *		buffer <out>			: buffer point. Malloc the buffer larger than 0xFC00 bytes.
 *		read_size <out>			: how many bytes are read
 *
 * Example:
 *		INNO_ReadChannelData(CHANNEL_0, buffer_0, return_size);
 *		INNO_ReadChannelData(CHANNEL_1, buffer_1, return_size);
 *
 * Note:
 *		This API can only read data from ONE logic channel. 
 *		If user needs to read data from both logic channel, please call this API twice.
 *
 */
INNO_RET INNO_ReadChannelData(CHANNEL_ID channel_id, unsigned char *buffer, int *read_size);


/*
 * Get Firmware Version 
 *
 * Parameter:
 *		major_version <out>	: major version
 *		minor_version <out>	: minor version
 *
 */
INNO_RET INNO_GetFirmwareVersion(unsigned char *major_version, unsigned char *minor_version);


/*
 * Set Tuner Frequency 
 *
 * Parameter:
 *		freq_dot <in>					: the Tuner freq dot.
 *
 *
 * Example:
 *		INNO_SetTunerFrequency(20);		//Set to 20 frequency dot. meaning 530000KHZ 	
 *		INNO_SetTunerFreqency(43);		//Set to 43 frequency dot. meaning 754000KHZ 	
 *
 */
extern INNO_RET INNO_SetTunerFrequency(unsigned char freq_dot);


/*
 * Set Power Management  
 *
 * Parameter:
 *		PM_MODE <in>	:		PM mode 
 *
 * Example:
 *		INNO_SetPowerManagement(PM_PAUSE);		//Pause IF101 and Tuner
 *		INNO_SetPowerManagement(PM_RESUME);		//Resume IF101 and Tuner	
 *
 */
INNO_RET INNO_SetPowerManagement(PM_MODE pm);


/*
 * Get IF101 System Status
 *
 * Parameter:
 *		sys_status <out>
 *			sys_status.sync						//sync status
 *			sys_status.current_frequency		//current frequency dot
 *			sys_status.signal_strength			//signal strength. unit:-dBm
 *			sys_status.ldpc_error				//LDPC error. unit:%
 *
*/
INNO_RET INNO_GetSystemStatus(SYS_STATUS *sys_status);


/*
 * Get Error Information
 *
 * Parameter:
 *		err_info <out>
 *			err_info.ldpc_total_count				//ldpc total block count
 *			err_info.ldpc_error_count				//ldpc error block count
 *			err_info.rs_total_count					//rs total block count
 *			err_info.rs_error_count					//rs error block count			
 *
*/
INNO_RET INNO_GetErrorInformation(ERR_INFO *err_info);


/*
 * Set Channel Configuration 
 *
 * Parameter:
 *		ts_start_0 <in>:		channel 0 start ts number
 *		ts_count_0 <in>:		channel 0 ts count
 *		demod_0 <in>:			channel 0 demod config 
 *		ts_start_1 <in>:		channel 1 start ts number
 *		ts_count_1 <in>:		channel 1 ts count
 *		demod_1 <in>:			channel 1 demod config
 *      
 * Example:
 *		INNO_SetDemodConfig(7,4,0x54,0,0,0);
 *		//Set logic channel 0 to receive timeslot 7,8,9,10, demod config: QPSK, RS(240,224),Mode1,1/2LDPC
 *		//Set logic channel 1 close
 *
 *
 * NOTE: 
 *	demod config = 
 *		|	Bit7	Bit6    	|	Bit5	Bit4     	|	Bit3	Bit2    	|	Bit1	Bit0
 *		|	Modulate Type	        |	RS Mode			|	Outer Mode		|	LDPC Mode
 *	00	|	BPSK			|	RS(240,240)		|	Reserved		|	1/2 LDPC
 *	01	|	QPSK			|	RS(240,224)		|	Mode 1			|	3/4 LDPC
 *	10	|	16QAM			|	RS(240,192)		|	Mode 2			|	Reserved
 *	11	|	Reserved		|	RS(240,176)		|	Mode 3			|	Reserved
 *
 */
INNO_RET INNO_SetChannelConfig(unsigned char ts_start_0, unsigned char ts_count_0, unsigned char demod_0,
							unsigned char ts_start_1, unsigned char ts_count_1, unsigned char demod_1);


/*
 * Get IF101 Channel Configuration
 *
 * Parameter:
 *		*ts_start_0 <out>:		channel 0 start ts number
 *		*ts_count_0 <out>:		channel 0 ts count
 *		*demod_0 <out>:			channel 0 demod config 
 *		*ts_start_1 <out>:		channel 1 start ts number
 *		*ts_count_1 <out>:		channel 1 ts count
 *		*demod_1 <out>:			channel 1 demod config
 *
 */
INNO_RET INNO_GetChannelConfig(unsigned char *ts_start_0, unsigned char *ts_count_0, unsigned char *demod_0,
									   unsigned char *ts_start_1, unsigned char *ts_count_1, unsigned char *demod_1);


/*
 * check downloading FW successfully or not 
 *
 * Parameter:
 *		buffer<in>:				FW buffer pointer
 *		len<in>:				FW len
 *		pass<out>:				check FW pass or not
 *
*/
INNO_RET INNO_CheckFWDownload(unsigned char *buffer, int len, int *pass);


/*
 * Update FW by I2C
 *
 * Parameter:
 *		buffer<in>:			FW buffer pointer
 *		len<in>:			FW len
 *
*/
INNO_RET INNO_UpdateFWFlash(unsigned char *buffer, int len);


#endif


