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
 *  Author:  <xyzhang@ingenic.cn>
 *
 *  Create:   2008-10-09, by xyzhang
 *
 *            
 *
 *******************************************************************************
 */

/***************************************************************************
 * Define for DMA mode
 *
 */
#include "jz4755.h"
#include "dm.h"
#include "mmc_config.h"
#include "mmc_protocol.h"
#include "mmc_api.h"
#include "mmc_core.h"
#include "os_api.h"

static OS_EVENT *mmc_dma_sem;
static OS_EVENT *mmc_msc_irq_sem;

#define PHYSADDR(x) ((x) & 0x1fffffff)
static int rw = 0;

#define MMC_DEBUG	0
#ifndef MMC_DEBUG
#undef printf
#define printf(x,args...)
#endif
//#undef printf
/***************************************************************************
 * Define card detect and power function etc.
 * This may be modified for a different platform.
 */


void inline MMC_INIT_GPIO()
{
#ifdef TF_SD_CARD
	OUTREG32(A_GPIO_PXFUNS(1), 0xFC000000);
	OUTREG32(A_GPIO_PXSELS(1), 0xFC000000);
	OUTREG32(A_GPIO_PXTRGC(1), 0xFC000000);
	OUTREG32(A_GPIO_PXPES(1),  0xFC000000);
#else
	// Can NOT support MSC0 and NAND at the same time
	// And are you sure that do NOT use NANDFlash?
	ERROR
#endif

#ifdef MMC_POWER_PIN
	__gpio_as_output(MMC_POWER_PIN);
	__gpio_disable_pull(MMC_POWER_PIN);
	__gpio_set_pin(MMC_POWER_PIN);
#endif
#ifdef MMC_CD_PIN
	__gpio_as_input(MMC_CD_PIN);
	__gpio_disable_pull(MMC_CD_PIN);
#endif
#ifdef MMC_SD_WP
	__gpio_as_input(MMC_SD_WP);
	__gpio_disable_pull(MMC_SD_WP);
#endif

}

#ifdef MMC_POWER_PIN

#define MMC_POWER_OFF()				\
do {						\
      	__gpio_set_pin(MMC_POWER_PIN);		\
} while (0)

#define MMC_POWER_ON()				\
do {						\
      	__gpio_clear_pin(MMC_POWER_PIN);		\
} while (0)

#endif

#define MMC_RESET()					\
do { 								\
	OUTREG16(A_MSC_CTRL(CH),MSC_CTRL_RESET);			\
 	while (INREG32(A_MSC_STAT(CH)) & MSC_STAT_IS_RESETTING);		\
} while (0)

#define MMC_IRQ_MASK()				\
do {						\
      	OUTREG32(A_MSC_IMASK(CH),0xffff);\
      	OUTREG16(A_MSC_IREG(CH), 0xffff);\
} while (0)

/***********************************************************************
 *  MMC Events
 */
#define MMC_EVENT_NONE	        0x00	/* No events */
#define MMC_EVENT_RX_DATA_DONE	0x01	/* Rx data done */
#define MMC_EVENT_TX_DATA_DONE	0x02	/* Tx data done */
#define MMC_EVENT_PROG_DONE	0x04	/* Programming is done */

static int use_4bit;		/* Use 4-bit data bus */
extern int num_6;
extern int sd2_0;


static void printfDMA(int ch)
{
	printf("ch = %d \r\n",ch);
	printf("A_DMA_DSA = 0x%x\r\n",INREG32(A_DMA_DSA(ch)));
	printf("A_DMA_DTA = 0x%x\r\n",INREG32(A_DMA_DTA(ch)));
	printf("A_DMA_DTC = 0x%x\r\n",INREG32(A_DMA_DTC(ch)));
	printf("A_DMA_DRT = 0x%x\r\n",INREG32(A_DMA_DRT(ch)));
	printf("A_DMA_DCS = 0x%x\r\n",INREG32(A_DMA_DCS(ch)));
	printf("A_DMA_DCM = 0x%x\r\n",INREG32(A_DMA_DCM(ch)));
	printf("A_DMA_DDA = 0x%x\r\n",INREG32(A_DMA_DDA(ch)));
	printf("A_DMA_DSD = 0x%x\r\n",INREG32(A_DMA_DSD(ch)));
	printf("A_DMA_DMAC = 0x%x\r\n",INREG32(A_DMA_DMAC(ch / 6)));
	printf("A_DMA_DIRQP = 0x%x\r\n",INREG32(A_DMA_DIRQP(ch / 6)));
	printf("A_DMA_DDR = 0x%x\r\n",INREG32(A_DMA_DDR(ch / 6)));
	printf("A_DMA_DCKE = 0x%x\r\n",INREG32(A_DMA_DCKE(ch / 6)));
}


/* Stop the MMC clock and wait while it happens */
static inline int jz_mmc_stop_clock(void)
{
	int timeout = 10000;
#if MMC_DEBUG
	printf("stop MMC clock\n");
#endif
	OUTREG16(A_MSC_CTRL(CH), MSC_CTRL_CLOCK_STOP);
	
	while (timeout && (INREG32(A_MSC_STAT(CH)) & MSC_STAT_CLK_EN)) 
	{
		timeout--;
		if (timeout == 0) 
		{
			printf("Timeout on stop clock waiting\n");
			return MMC_ERROR_TIMEOUT;
		}
		udelay(1);
	}
	//printf("clock off time is %d microsec\n", timeout);
	return MMC_NO_ERROR;
}

/* Start the MMC clock and operation */
static inline int jz_mmc_start_clock(void)
{
	OUTREG16(A_MSC_CTRL(CH), (MSC_CTRL_CLOCK_START | MSC_CTRL_START_OP));
	return MMC_NO_ERROR;
}

static inline unsigned int jz_mmc_calc_clkrt(int is_sd, unsigned int rate)
{
	unsigned int clkrt;
	unsigned int clk_src = is_sd ? 24000000 : 20000000;
	//unsigned int clk_src = is_sd ? 48000000 : 20000000;
		
	clkrt = 0;
	while (rate < clk_src) 
	{
		clkrt++;
		clk_src >>= 1;
	}
	return clkrt;
}

static int jz_mmc_check_status(struct mmc_request *request)
{
	unsigned int status = INREG32(A_MSC_STAT(CH));

	/* Checking for response or data timeout */
	if (status & (MSC_STAT_TIME_OUT_RES | MSC_STAT_TIME_OUT_READ)) 
	{
		printf("MMC/SD timeout, MMC_STAT 0x%x CMD %d\n", status,
		       request->cmd);
		return MMC_ERROR_TIMEOUT;
	}

	/* Checking for CRC error */
	if (status & (MSC_STAT_CRC_READ_ERR | MSC_STAT_CRC_WRITE_ERR |
	     MSC_STAT_CRC_RES_ERR)) 
	{
		printf("MMC/CD CRC error, MMC_STAT 0x%x\n", status);
		return MMC_ERROR_CRC;
	}

	return MMC_NO_ERROR;
}

/* Obtain response to the command and store it to response buffer */
static void jz_mmc_get_response(struct mmc_request *request)
{
	int i;
	unsigned char *buf;
	unsigned int data;
#if MMC_DEBUG
	printf("fetch response for request %d, cmd %d\n", request->rtype,
	      request->cmd);
#endif
	
	buf = request->response;
	request->result = MMC_NO_ERROR;

	switch (request->rtype) 
	{
	case RESPONSE_R1:
	case RESPONSE_R1B:
	case RESPONSE_R6:
	case RESPONSE_R3:
	case RESPONSE_R4:
	case RESPONSE_R5:
		{
			data = INREG16(A_MSC_RES(CH));
			buf[0] = (data >> 8) & 0xff;
			buf[1] = data & 0xff;
			data = INREG16(A_MSC_RES(CH));
			buf[2] = (data >> 8) & 0xff;
			buf[3] = data & 0xff;
			data = INREG16(A_MSC_RES(CH));
			buf[4] = data & 0xff;

			//printf("request %d, response [%02x %02x %02x %02x %02x]\n",
			  //    request->rtype, buf[0], buf[1], buf[2],
			    //  buf[3], buf[4]);
			break;
		}
	case RESPONSE_R2_CID:
	case RESPONSE_R2_CSD:
		{
			for (i = 0; i < 16; i += 2) 
			{
				data = INREG16(A_MSC_RES(CH));
				buf[i] = (data >> 8) & 0xff;
				buf[i + 1] = data & 0xff;
			}
			//printf("request %d, response [", request->rtype);
#if CONFIG_MMC_DEBUG_VERBOSE > 2
			if (g_mmc_debug >= 3) 
			{
				int n;
				for (n = 0; n < 17; n++)
					printk("%02x ", buf[n]);
				printk("]\n");
			}
#endif
			break;
		}
	case RESPONSE_NONE:
		printf("No response\n");
		break;

	default:
		printf("unhandled response type for request %d\n",
		      request->rtype);
		break;
	}
}

#if MMC_DMA_ENABLE
static int jz_mmc_receive_data_pre_dma(struct mmc_request *req)
{
	int ch = MMC1_DMA_CHANNEL;
	unsigned int size = req->block_len * req->nob;
	unsigned char err = 0;
#if MMC_DEBUG	
	printf("%s() ++\n", __FUNCTION__);
#endif	
	if ((unsigned long) req->buffer < 0xa0000000)
		dma_cache_wback_inv((unsigned long) req->buffer, size);
	
	OUTREG32(A_DMA_DSA(ch), PHYSADDR(A_MSC_RXFIFO(CH)));/* DMA source addr */
	OUTREG32(A_DMA_DTA(ch), PHYSADDR((unsigned long) req->buffer));/* DMA dest addr */
	OUTREG32(A_DMA_DTC(ch), (size / 4));/* DMA transfer count */
	OUTREG32(A_DMA_DRT(ch), DRT_MSC_RX);/* DMA request type */

#if MMC_DMA_INTERRUPT
	OUTREG32(A_DMA_DCM(ch), (DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT
								| DCM_TSZ_32BIT | DCM_TRANS_INTR_EN));
#else
	OUTREG32(A_DMA_DCM(ch), (DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT| DCM_TSZ_32BIT));
#endif
	    
	
}
static int jz_mmc_receive_data_dma(struct mmc_request *req)
{
	int ch = MMC1_DMA_CHANNEL;
	unsigned char err = 0;
#if MMC_DEBUG	
	printf("%s() ++\n", __FUNCTION__);
#endif	
	/* setup dma channel */
	SETREG32(A_DMA_DCS(ch), DCS_CTE | DCS_NDES);
	
#if MMC_DMA_INTERRUPT
	os_SemaphorePend(mmc_dma_sem, 0, &err);
	if(err)
	{
		printf("%s():timeout err!!!\n", __FUNCTION__);
		printfDMA(MMC1_DMA_CHANNEL);
	}
#else
	while (	INREG32(A_DMA_DTC(ch)));
#endif
/* clear status and disable channel */
	CLRREG32(A_DMA_DCS(ch),(DCS_CTE | DCS_TT));
	
#if MMC_DMA_INTERRUPT
	return (err == OS_NO_ERROR);
#else
	return 0;
#endif
}

static int jz_mmc_transmit_data_dma(struct mmc_request *req)
{
	int ch = MMC1_DMA_CHANNEL;
	unsigned int size = req->block_len * req->nob;
	unsigned char err = 0;
#if MMC_DEBUG
	printf("%s() ++\n", __FUNCTION__);
#endif
	
	/* flush dcache */
	if ((unsigned long) req->buffer < 0xa0000000)
		dma_cache_wback_inv((unsigned long) req->buffer, size);
	
	OUTREG32(A_DMA_DSA(ch), PHYSADDR((unsigned long) req->buffer));/* DMA source addr */
	OUTREG32(A_DMA_DTA(ch), PHYSADDR(A_MSC_TXFIFO(CH)));/* DMA dest addr */
	OUTREG32(A_DMA_DTC(ch), (size / 4));/* DMA transfer count */
	OUTREG32(A_DMA_DRT(ch), DRT_MSC_TX);/* DMA request type */

#if MMC_DMA_INTERRUPT
	OUTREG32(A_DMA_DCM(ch), (DCM_SAI | DCM_SP_32BIT | DCM_DP_32BIT
				| DCM_TSZ_32BIT | DCM_TRANS_INTR_EN));

#else
	OUTREG32(A_DMA_DCM(ch), (DCM_SAI | DCM_SP_32BIT | DCM_DP_32BIT
				| DCM_TSZ_32BIT));
#endif

	/* Enable the dma */
	SETREG32(A_DMA_DCS(ch), DCS_CTE | DCS_NDES);
	
#if MMC_DMA_INTERRUPT

	os_SemaphorePend(mmc_dma_sem, 0, &err);
	if(err)
	{
		printf("%s():timeout err!!!\n", __FUNCTION__);
		printfDMA(MMC1_DMA_CHANNEL);
	}
	
	/* clear status and disable channel */
	CLRREG32(A_DMA_DCS(ch),(DCS_CTE | DCS_TT));

	return (err == OS_NO_ERROR);

#else
	/* wait for dma completion */
	while (	INREG32(A_DMA_DTC(ch)));
	/* clear status and disable channel */
	CLRREG32(A_DMA_DCS(ch),(DCS_CTE | DCS_TT));
	return 0;
#endif

}

#endif				/* MMC_DMA_ENABLE */

static int jz_mmc_receive_data(struct mmc_request *req)
{
	unsigned int nob = req->nob;
	unsigned int wblocklen = (unsigned int) (req->block_len + 3) >> 2;	/* length in word */
	unsigned char *buf = req->buffer;
	unsigned int *wbuf = (unsigned int *) buf;
	unsigned int waligned = (((unsigned int) buf & 0x3) == 0);	/* word aligned ? */
	unsigned int stat, timeout, data, cnt;

	for (nob; nob >= 1; nob--) 
	{
		timeout = 0x3FFFFFF;

		while (timeout) 
		{
			timeout--;
			stat = INREG32(A_MSC_STAT(CH));

			if (stat & MSC_STAT_TIME_OUT_READ)
				return MMC_ERROR_TIMEOUT;
			else if (stat & MSC_STAT_CRC_READ_ERR)
				return MMC_ERROR_CRC;
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)
				 || (stat & MSC_STAT_DATA_FIFO_AFULL)) {
				/* Ready to read data */
				break;
			}

			udelay(1);
		}

		if (!timeout)
			return MMC_ERROR_TIMEOUT;

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
//		printf("Receice Data = %d\r\n", wblocklen);
		cnt = wblocklen;
		while (cnt) 
		{
			data = INREG32(A_MSC_RXFIFO(CH));
			if (waligned) 
			{
				*wbuf++ = data;
			}
			else
			{
				*buf++ = (unsigned char) (data >> 0);
				*buf++ = (unsigned char) (data >> 8);
				*buf++ = (unsigned char) (data >> 16);
				*buf++ = (unsigned char) (data >> 24);
			}
			cnt--;
			while (cnt && (INREG32(A_MSC_STAT(CH)) & MSC_STAT_DATA_FIFO_EMPTY));
		}
	}

	return MMC_NO_ERROR;
}

static int jz_mmc_transmit_data(struct mmc_request *req)
{
	unsigned int nob = req->nob;
	unsigned int wblocklen = (unsigned int) (req->block_len + 3) >> 2;	/* length in word */
	unsigned char *buf = req->buffer;
	unsigned int *wbuf = (unsigned int *) buf;
	unsigned int waligned = (((unsigned int) buf & 0x3) == 0);	/* word aligned ? */
	unsigned int stat, timeout, data, cnt;

	for (nob; nob >= 1; nob--) 
	{
		timeout = 0x3FFFFFF;

		while (timeout) 
		{
			timeout--;
			stat = INREG32(A_MSC_STAT(CH));

			if (stat & (MSC_STAT_CRC_WRITE_ERR | MSC_STAT_CRC_WRITE_NO_STATUS))
			{
				return MMC_ERROR_CRC;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_FULL)) 
			{
				/* Ready to write data */
				break;
			}

			udelay(1);
		}

		if (!timeout)
			return MMC_ERROR_TIMEOUT;

		/* Write data to TXFIFO */
		cnt = wblocklen;
		while (cnt) 
		{
			while (INREG32(A_MSC_STAT(CH)) & MSC_STAT_DATA_FIFO_FULL);

			if (waligned)
			{
				OUTREG32(A_MSC_TXFIFO(CH), *wbuf++);
			} 
			else
			{
				data = *buf++ | (*buf++ << 8) | (*buf++ << 16)
				    	| (*buf++ << 24);
				OUTREG32(A_MSC_TXFIFO(CH), data);
			}

			cnt--;
		}
	}

	return MMC_NO_ERROR;
}

/********************************************************************************************************************
** Name:	  int jz_mmc_exec_cmd()
** Function:      send command to the card, and get a response
** Input:	  struct mmc_request *req	: MMC/SD request
** Output:	  0:  right		>0:  error code
********************************************************************************************************************/
int jz_mmc_exec_cmd(struct mmc_request *request)
{
	unsigned int cmdat = 0, events = 0;
	int retval, timeout = 0x3fffff;
	unsigned char err = 0;
	
	/* Indicate we have no result yet */
	request->result = MMC_NO_RESPONSE;
	
	if (request->cmd == MMC_CIM_RESET) 
	{
		printf("jz_mmc_exec_cmd: reset MMC card\n");	
		
		/* On reset, 1-bit bus width */
		use_4bit = 0;

		/* Reset MMC/SD controller */
		MMC_RESET();

		/* On reset, drop MMC clock down */
//		jz_mmc_set_clock(0, MMC_CLOCK_SLOW);
		jz_mmc_set_clock(0, 200000);
		
		/* On reset, stop MMC clock */
		jz_mmc_stop_clock();
	}
	if (request->cmd == MMC_SEND_OP_COND) 
	{
		//printf("Have an MMC card %d\n", mmc_check_status());
		/* always use 1bit for MMC */
		use_4bit = 0;
	}
	if (request->cmd == SET_BUS_WIDTH) 
	{
		if (request->arg == 0x2) 
		{
			printf("Use 4-bit bus width\n");
			use_4bit = 1;
		}
		else 
		{
			printf("Use 1-bit bus width\n");
			use_4bit = 0;
		}
	}

	/* stop clock */
	jz_mmc_stop_clock();

	/* clear status */	
	OUTREG16(A_MSC_IREG(CH), 0xFFFF);
	
	/*open interrupt */
	CLRREG32(A_MSC_IMASK(CH),(MSC_DATA_TRAN_DONE | MSC_PRG_DONE | MSC_END_CMD_RES));
	
	/* use 4-bit bus width when possible */
	if (use_4bit)
		cmdat |= MSC_CMDAT_BUS_WIDTH_4BIT;

	/* Set command type and events */
	switch (request->cmd) 
	{
		/* MMC core extra command */
	case MMC_CIM_RESET:
		cmdat |= MSC_CMDAT_INIT;	/* Initialization sequence sent prior to command */
		break;

		/* bc - broadcast - no response */
	case MMC_GO_IDLE_STATE:
	case MMC_SET_DSR:
		break;

		/* bcr - broadcast with response */
	case MMC_SEND_OP_COND:
	case MMC_ALL_SEND_CID:

	case MMC_GO_IRQ_STATE:
		break;

		/* adtc - addressed with data transfer */
	case MMC_READ_DAT_UNTIL_STOP:
	case MMC_READ_SINGLE_BLOCK:
	case MMC_READ_MULTIPLE_BLOCK:
	case SEND_SCR:
		cmdat &= ~(MSC_CMDAT_WRRD_MASK);
#if MMC_DMA_ENABLE
		cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_READ | MSC_CMDAT_DMA_EN;
#else
		cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_READ;
#endif
		events = MMC_EVENT_RX_DATA_DONE;
		break;

	case 6:
		if (num_6 < 2) 
		{
			cmdat &= ~(MSC_CMDAT_WRRD_MASK);
#if MMC_DMA_ENABLE
			cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_READ |
			    MSC_CMDAT_DMA_EN;
#else
			cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_READ;
#endif
			events = MMC_EVENT_RX_DATA_DONE;
		}
		break;

	case MMC_WRITE_DAT_UNTIL_STOP:
	case MMC_WRITE_BLOCK:
	case MMC_WRITE_MULTIPLE_BLOCK:
	case MMC_PROGRAM_CID:
	case MMC_PROGRAM_CSD:
	case MMC_SEND_WRITE_PROT:
	case MMC_GEN_CMD:
	case MMC_LOCK_UNLOCK:
		cmdat &= ~(MSC_CMDAT_WRRD_MASK);
#if MMC_DMA_ENABLE
		cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_WRITE | MSC_CMDAT_DMA_EN;
#else
		cmdat |= MSC_CMDAT_DATA_EN | MSC_CMDAT_WRITE;
#endif
		events = MMC_EVENT_TX_DATA_DONE | MMC_EVENT_PROG_DONE;

		break;

	case MMC_STOP_TRANSMISSION:
		events = MMC_EVENT_PROG_DONE;
		break;

		/* ac - no data transfer */
	default:
		break;
	}

	/* Set response type */
	cmdat &= ~(MSC_CMDAT_RESP_FORMAT_MASK);
	switch (request->rtype) 
	{
	case RESPONSE_NONE:
		break;

	case RESPONSE_R1B:
		cmdat |= MSC_CMDAT_BUSY;
	 /*FALLTHRU*/ 
	case RESPONSE_R1:
		cmdat |= MSC_CMDAT_RESPONSE_R1;
		break;
	case RESPONSE_R2_CID:
	case RESPONSE_R2_CSD:
		cmdat |= MSC_CMDAT_RESPONSE_R2;
		break;
	case RESPONSE_R3:
		cmdat |= MSC_CMDAT_RESPONSE_R3;
		break;
	case RESPONSE_R4:
		cmdat |= MSC_CMDAT_RESPONSE_R4;
		break;
	case RESPONSE_R5:
		cmdat |= MSC_CMDAT_RESPONSE_R5;
		break;
	case RESPONSE_R6:
		cmdat |= MSC_CMDAT_RESPONSE_R6;
		break;
	default:
		break;
	}
	
	if ((request->cmd == 17) || (request->cmd == 18))
		rw = 0;
	
	if ((request->cmd == 24) || (request->cmd == 25))
		rw = 1;	
	
	/* Set command index */
	if (request->cmd == MMC_CIM_RESET) 
	{
		OUTREG8(A_MSC_CMD(CH),MMC_GO_IDLE_STATE);
	} 
	else 
	{
		OUTREG8(A_MSC_CMD(CH),request->cmd);
	}

	/* Set argument */
	OUTREG32(A_MSC_ARG(CH),request->arg);
	/* Set block length and nob */
	if (request->cmd == SEND_SCR) 
	{	/* get SCR from DataFIFO */
		OUTREG16(A_MSC_BLKLEN(CH),8);
		OUTREG16(A_MSC_NOB(CH),1);
	}
	else 
	{
		OUTREG16(A_MSC_BLKLEN(CH),request->block_len);
		OUTREG16(A_MSC_NOB(CH),request->nob);
	}

	/* Set command */
	OUTREG32(A_MSC_CMDAT(CH),cmdat);

//	printf("jz_mmc_exec_cmd:Send cmd %d cmdat: %x arg: %x resp %d\n", request->cmd,
	//      cmdat, request->arg, request->rtype);

	if (events & (MMC_EVENT_RX_DATA_DONE | MMC_EVENT_TX_DATA_DONE)) 
	{
		if (events & MMC_EVENT_RX_DATA_DONE) 
		{
			if (request->cmd == SEND_SCR) 
			{
				/* SD card returns SCR register as data. 
				   MMC core expect it in the response buffer, 
				   after normal response. */
				request->buffer =
				    (unsigned char *) ((unsigned int) request->response + 5);
			}
			
#if MMC_DMA_ENABLE
		jz_mmc_receive_data_pre_dma(request);		
#endif
		}
	}
	/* Start MMC/SD clock and send command to card */
	
	jz_mmc_start_clock();
	
	/* Wait for command completion */
#if MMC_MSC_INTERRUPT
	if(!(INREG16(A_MSC_IREG(CH)) & MSC_END_CMD_RES))
	{
		os_SemaphoreSet(mmc_msc_irq_sem,0,&err);
		OUTREG32(A_INTC_ICMCR,IRQ_MSC);
		CLRREG32(A_MSC_IMASK(CH),MSC_END_CMD_RES);
		os_SemaphorePend(mmc_msc_irq_sem, 100, &err);
		if(err)
		{
			printf("%s():Wait MSC_END_CMD_RES timeout err!!!\n", __FUNCTION__);
		}
	}
	OUTREG16(A_MSC_IREG(CH), MSC_END_CMD_RES);
#else
	while (timeout-- && !(INREG32(A_MSC_STAT(CH)) & MSC_STAT_END_CMD_RES));
	if (timeout == 0)
		return MMC_ERROR_TIMEOUT;
#endif

	/* clear flag */
	OUTREG16(A_MSC_IREG(CH), MSC_END_CMD_RES);
	
	/* Check for status */ 
	retval = jz_mmc_check_status(request);
	if (retval) 
	{
		return retval;
	}

	/* Complete command with no response */
	if (request->rtype == RESPONSE_NONE) 
	{
		return MMC_NO_ERROR;
	}

	/* Get response */
	jz_mmc_get_response(request);

	/* Start data operation */
	if (events & (MMC_EVENT_RX_DATA_DONE | MMC_EVENT_TX_DATA_DONE)) 
	{
		if (events & MMC_EVENT_RX_DATA_DONE) 
		{
			if (request->cmd == SEND_SCR) 
			{
				/* SD card returns SCR register as data. 
				   MMC core expect it in the response buffer, 
				   after normal response. */
				request->buffer = (unsigned char *) ((unsigned int) request->response + 5);
			}
#if MMC_DMA_ENABLE
		jz_mmc_receive_data_dma(request);
#else
		jz_mmc_receive_data(request);
#endif
		}
		if(events & MMC_EVENT_TX_DATA_DONE)
		{
#if MMC_DMA_ENABLE
			jz_mmc_transmit_data_dma(request);
#else
			jz_mmc_transmit_data(request);
#endif
		}
#if MMC_MSC_INTERRUPT
		if(!(INREG16(A_MSC_IREG(CH)) & MSC_DATA_TRAN_DONE))
		{
			os_SemaphoreSet(mmc_msc_irq_sem,0,&err);
			OUTREG32(A_INTC_ICMCR,IRQ_MSC);
			CLRREG32(A_MSC_IMASK(CH),MSC_DATA_TRAN_DONE);
			os_SemaphorePend(mmc_msc_irq_sem, 100, &err);
			if(err)
			{
				printf("%s():Wait MSC_DATA_TRAN_DONE timeout err!!!\n", __FUNCTION__);
			}
		}
		
		OUTREG16(A_MSC_IREG(CH), MSC_DATA_TRAN_DONE);
#else
		/* Wait for Data Done */
		timeout = 0x3fffff;
		while ((!(INREG16(A_MSC_IREG(CH)) & MSC_DATA_TRAN_DONE)) && timeout--);
		/* clear status */
		OUTREG16(A_MSC_IREG(CH), MSC_DATA_TRAN_DONE);
#endif
	}

	/* Wait for Prog Done event */
	if (events & MMC_EVENT_PROG_DONE) 
	{
#if MMC_MSC_INTERRUPT
		if(!(INREG16(A_MSC_IREG(CH)) & MSC_PRG_DONE))
		{
			os_SemaphoreSet(mmc_msc_irq_sem,0,&err);
			OUTREG32(A_INTC_ICMCR,IRQ_MSC);
			CLRREG32(A_MSC_IMASK(CH),MSC_PRG_DONE);
			os_SemaphorePend(mmc_msc_irq_sem, 100, &err);
			if(err)
			{
				printf("%s():Wait MSC_PRG_DONE timeout err!!!\n", __FUNCTION__);
			}
		}
		/* clear status */
		OUTREG16(A_MSC_IREG(CH), MSC_PRG_DONE);
#else
		timeout = 0x3fffff;
		while ((!(INREG16(A_MSC_IREG(CH)) & MSC_PRG_DONE)) && timeout--);
		/* clear status */
		OUTREG16(A_MSC_IREG(CH), MSC_PRG_DONE);
#endif		
	}

	/* Command completed */

	return MMC_NO_ERROR;	/* return successfully */
}


static void CpmSelectMscHSClk(int sd)
{
	unsigned int clock,div = 0;
	
	clock = GetCurrentPLLClock(1);
	
	if (sd)
	{
		div = clock / 24000000;
	}
	else 
	{
		div = clock / 16000000;
	}
	
	OUTREG32(A_CPM_MSCCDR,(div -1));
	//printf("CpmSelectMscHSClk:clock = %d,div = %d,A_CPM_MSCCDR[0x%x]\n",clock,(div-1),INREG32(A_CPM_MSCCDR));
}

/* Set the MMC clock frequency */
void jz_mmc_set_clock(int sd, unsigned int rate)
{
	int clkrt = 0;

	sd = sd ? 1 : 0;

	jz_mmc_stop_clock();

	if (sd2_0) 
	{
		CpmSelectMscHSClk(sd);	/* select clock source from CPM */
		SETREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);
		OUTREG16(A_MSC_CLKRT(CH),0);
	}
	else
	{
		CpmSelectMscHSClk(sd);	/* select clock source from CPM */
		SETREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);
		clkrt = jz_mmc_calc_clkrt(sd, rate);
		OUTREG16(A_MSC_CLKRT(CH),clkrt);
	}
	
	//printf("set clock to %u Hz is_sd=%d clkrt=%d A_MSC_CLKRT[0x%x]\n", rate, sd, clkrt,INREG16(A_MSC_CLKRT));
}

#if MMC_DMA_INTERRUPT

static void jz_mmc_dma_handler(unsigned int arg)
{
	unsigned int dwDmaDCS = INREG32(A_DMA_DCS(arg));
	if (dwDmaDCS & DCS_AR)
	{
		printf("%s: DMAC address error.\n", __FUNCTION__);
		CLRREG32(A_DMA_DCS(arg), (DCS_AR | DCS_CTE | DCS_TT));
	}
	else if (dwDmaDCS & DCS_TT)
	{
		CLRREG32(A_DMA_DCS(arg),(DCS_CTE | DCS_TT));
		os_SemaphorePost(mmc_dma_sem);
	}
	else
		printf("%s: DMAC interrupt error.\n", __FUNCTION__);
}

#endif
#if MMC_MSC_INTERRUPT
static void jz_mmc_irq_handler(unsigned int arg)
{
	OUTREG32(A_INTC_ICMSR,IRQ_MSC);
	//printf("%s: A_MSC_IMASK(1) = 0x%x.\n", __FUNCTION__,A_MSC_IMASK(CH));
	OUTREG32(A_MSC_IMASK(CH), 0xFFFF);
	os_SemaphorePost(mmc_msc_irq_sem);
}
#endif
/*******************************************************************************************************************
** Name:	  void mmc_hardware_init()
** Function:      initialize the hardware condiction that access sd card
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
static int init_jz_mmc_hardware_init = 0;

static void MMCInitDMA(void)
{
	int ch = MMC1_DMA_CHANNEL;
	/* setup dma channel */
	
	OUTREG32(A_DMA_DCKE(ch / 6),(1 << (ch % 6)));//Open channel clock
	CLRREG32(A_DMA_DMAC(ch / 6), (DMAC_HALT | DMAC_ADDR_ERR));//Ensure DMAC.AR = 0,DMAC.HLT = 0
	
	CLRREG32(A_DMA_DCS(ch), DCS_AR | DCS_HLT | DCS_TT | DCS_INV); // Ensure DCSn.AR = 0, DCSn.HLT = 0, DCSn.TT = 0, DCSn.INV = 0
	OUTREG32(A_DMA_DTC(ch),0);//DTCn = 0
	CLRREG32(A_DMA_DCS(ch), DCS_CTE | DCS_NDES);
}

int jz_mmc_hardware_init(void)
{
	if(init_jz_mmc_hardware_init) return 0;
    init_jz_mmc_hardware_init = 1;
    CLRREG32(A_CPM_CLKGR, CLKGR_STOP_MSC);
    MMC_INIT_GPIO();	/* init GPIO */
#ifdef MMC_POWER_PIN
	MMC_POWER_ON();		/* turn on power of card */
#endif

	MMC_RESET();		/* reset mmc/sd controller */
	MMC_IRQ_MASK();		/* mask all IRQs */
	jz_mmc_stop_clock();	/* stop MMC/SD clock */

#if MMC_DMA_ENABLE
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_DMAC);//__cpm_start_dmac();
	SETREG32(A_DMA_DMAC(MMC1_DMA_CHANNEL / 6),(DMAC_FAST_MSC | DMAC_DMA_EN));//__dmac_enable_module();
	MMCInitDMA();
	
#if MMC_DMA_INTERRUPT
	mmc_dma_sem = os_SemaphoreCreate(0);
	OUTREG32(A_DMA_DCS(MMC1_DMA_CHANNEL), 0);
	request_irq(EIRQ_DMA_BASE + MMC1_DMA_CHANNEL, jz_mmc_dma_handler,MMC1_DMA_CHANNEL);
	OUTREG32(A_INTC_ICMCR,IRQ_DMA_B);
#endif

#endif

#if MMC_MSC_INTERRUPT
	mmc_msc_irq_sem = os_SemaphoreCreate(0);
	OUTREG32(A_INTC_ICMSR,IRQ_MSC);
	request_irq(IRQ_MSC, jz_mmc_irq_handler, 0);
#else
	OUTREG32(A_INTC_ICMSR,IRQ_MSC);
#endif
	return 0;
}

#if (DM==1)
int mmc_preconvert(PPLL_OPT pllopt)
{
	unsigned int clock,div = 0,pcs;
	//printf("mmc convert clock\n");
	clock = pllopt->cpuclock;//pll output
	
	pcs = INREG32(A_CPM_CPCCR) & CPCCR_PLLCLK_DIV2;
	if (pcs == 0)
		clock >>= 1;
		
	div = clock / 16000000;
	if(div > 0) div--;
	
	return div;
}
int mmc_convert(unsigned int div)
{
	OUTREG32(A_CPM_MSCCDR,(div -1));
}

static struct dm_jz4740_t mmc_dm;
	
void mng_init_mmc(void)
{
	mmc_dm.name = "MMC driver";
	mmc_dm.convert = mmc_convert;
	mmc_dm.preconvert = mmc_preconvert;
	dm_register(1, &mmc_dm);
}
#endif
