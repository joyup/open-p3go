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
 *  Author:  <dsqiu@ingenic.cn>  <zzhang@ingenic.cn> 
 *
 *  Create:   2008-10-13, by zzhang zhi
 *            
 *            
 *
 *******************************************************************************
 */


#include <os_api.h>
#include <jz4755.h>
#include <kernel.h>
#include <bsp.h>
#include "usb.h"

#include "udc.h"
#include "udcbus.h"
#include "threadprio.h"

#ifdef USE_MIDWARE
#include "midware.h"
#endif

typedef int BOOL;

#ifndef FALSE
#define FALSE             0
#endif

#ifndef TRUE
#define TRUE              1
#endif

#define EP1_INTR_BIT 2
#define EP_FIFO_NOEMPTY 2

#define IRQ_GPIO_UDC_DETE	(EIRQ_GPIO_BASE + GPIO_UDC_DETE_PIN)
#define MAX_CABLE_DETE  3
#define CABLE_DETE_TIME  500                //wait time in ms
#define CABLE_CONNECTED          0
#define CABLE_DISCONNECT         1

#define DEBUGMSG 0
#if DEBUGMSG
#define dprintf printf
#else
#undef  dprintf
#define dprintf(x...)
#endif

#define IS_CACHE(x) (x < 0xa0000000)

#ifdef USE_MIDWARE
volatile static MIDSRC udcsrc;
volatile static unsigned int udcid;

volatile static MIDSRCDTA res;
#endif

volatile static unsigned int cable_stat;
volatile static unsigned int protocool_stat;

static unsigned char err;

typedef struct
{
	unsigned char ep;
	unsigned char state;
	unsigned short fifosize;	
	unsigned int curlen;
	unsigned int totallen;
	unsigned int data_addr;
	unsigned int fifo_addr;
}EPSTATE,*PEPSTATE;

#define CPU_READ     0
#define CPU_WRITE     1

#define DMA_READ     2
#define DMA_WRITE     3


#define WRITE_FINISH  4
#define READ_FINISH  5

#define TXFIFOEP0 A_UDC_FIFO_EP0

EPSTATE epstat[3] = {
	{0x00, 0,0,0,0,0,TXFIFOEP0 + 0},
	{0x81, 0,0,0,0,0,TXFIFOEP0 + 4},
	{0x01, 0,0,0,0,0,TXFIFOEP0 + 4}
};

#define GetEpState(x) (&epstat[x])

unsigned int	g_timeout = 0;


static OS_EVENT *udcEvent;
unsigned char USB_Version;
volatile static unsigned char udc_irq_type = 0;

//void (*tx_done)(void) = NULL;

#define UDC_TASK_PRIO	UDC_THREAD_PRIO  //7
#define UDC_TASK_STK_SIZE	1024 * 2
static OS_TASK_STACK udcTaskStack[UDC_TASK_STK_SIZE];

#define PHYADDR(x)		(x & 0x1fffffff)
#define	UDC_BURST_MODE	(0 << 9)

static void dma_receieve_data(PEPSTATE pep)
{
	unsigned int size;
	size = pep->totallen - (pep->totallen % pep->fifosize);
	if(IS_CACHE(pep->data_addr))
		dma_cache_wback_inv(pep->data_addr, pep->totallen);
	//IntrOutMask = 0x0;
	
	
	OUTREG32(A_UDC_ADDR2,PHYADDR(pep->data_addr));
	OUTREG32(A_UDC_COUNT2,size);
//	OUTREG32(A_UDC_CNTL2,0x061d);
//	OUTREG32(A_UDC_CNTL2,0x021d);
	OUTREG32(A_UDC_CNTL2,0x001d | UDC_BURST_MODE);

	OUTREG8(A_UDC_INDEX,1);	
	SETREG16(A_UDC_OUTCSR,0xa000);
	//printf("dma receive %x\n",size);    
	pep->curlen = size;
	
	
}
static void dma_send_data(PEPSTATE pep)
{
	  unsigned int size;
	  
	  size = pep->totallen - (pep->totallen % pep->fifosize);
	  
	  if(IS_CACHE(pep->data_addr))
		  dma_cache_wback_inv(pep->data_addr, size);
	  OUTREG32(A_UDC_ADDR1,PHYADDR(pep->data_addr));
	  OUTREG32(A_UDC_COUNT1,size);//fifosize[ep]);
//	  OUTREG32(A_UDC_CNTL1,0x061f);
//	  OUTREG32(A_UDC_CNTL1,0x021f);
	  OUTREG32(A_UDC_CNTL1,0x001f | UDC_BURST_MODE);

	  OUTREG8(A_UDC_INDEX, 1);
	  SETREG16(A_UDC_INCSR,0x9400);
	  
	  //printf("dma send %x\n",size);
	  pep->curlen = size;
}
static inline void dma_send_data_Finish()
{
	OUTREG32(A_UDC_CNTL1,0x0);
	OUTREG8(A_UDC_INDEX,1);
	CLRREG16(A_UDC_INCSR,0x9400);
	
}

static void dma_receieve_data_finish()
{
	dprintf("\n Disable DMA!");
	
	OUTREG32(A_UDC_CNTL2,0x0);
	OUTREG8(A_UDC_INDEX,1);
	CLRREG16(A_UDC_OUTCSR,0xa000);
}


//----------------------------------------------------------------------------
#ifdef USE_MIDWARE

bool BAT_GetDCDetect(void)
{
#ifndef DC_DETE_PIN
	return (__gpio_get_pin(GPIO_UDC_DETE_PIN) == 1);
#else
	return (__gpio_get_pin(DC_DETE_PIN) == DC_DETE_LEVEL);
#endif
}

//----------------------------------------------------------------------------

unsigned int MidCheckBatState(void)
{
	if (!BAT_GetDCDetect())
		return (BAT_NOT_CHARGING);
#ifdef CHARG_STAT_PIN
	if (__gpio_get_pin(CHARG_STAT_PIN) == CHARG_STAT_LEVEL)
		return (BAT_CHARGING);
	else
		return (BAT_FULL);
#else
	int bat_volt = read_battery();
	
	if (bat_volt >= BAT_MAX_VOLT){
		return BAT_FULL;	
	}else {
		return BAT_CHARGING;	
	}
#endif

}
#endif
//----------------------------------------------------------------------------

static void udc_reset(unsigned int handle)
{
	unsigned char byte;
	PEPSTATE pep;
	
	OUTREG16(A_UDC_INTRINE,0);
	OUTREG16(A_UDC_INTROUTE, 0);
	OUTREG8(A_UDC_INTRUSBE, 0);

	OUTREG8(A_UDC_FADDR,0);
//OUTREG8(A_UDC_POWER,0x60);   //High speed

	OUTREG8(A_UDC_INDEX,0);
	OUTREG8(A_UDC_CSR0,0xc0);

	OUTREG8(A_UDC_INDEX,1);
	OUTREG16(A_UDC_INCSR,0x2048);
	OUTREG16(A_UDC_INMAXP,512);
	
	OUTREG8(A_UDC_INDEX,1);
	OUTREG16(A_UDC_OUTCSR,0x0090);
	OUTREG16(A_UDC_OUTMAXP,512);
	byte = INREG8(A_UDC_POWER);
	dprintf("REG_POWER: %02x\r\n",byte);
	epstat[0].fifosize = 64;
	
	if(epstat[1].state != WRITE_FINISH)
		INREG16(A_UDC_INTRIN);
	if(epstat[2].state != READ_FINISH)
		INREG16(A_UDC_INTROUT);
	
	epstat[0].state = 0;
	epstat[1].state = WRITE_FINISH;
	epstat[2].state = READ_FINISH;
	epstat[0].curlen = 0;
	epstat[1].curlen = 0;
	epstat[2].curlen = 0;
	epstat[0].totallen = 0;
	epstat[1].totallen = 0;
	epstat[2].totallen = 0;
	
	if ((byte & 0x10)==0) 
	{
		OUTREG8(A_UDC_INDEX,1);
		OUTREG16(A_UDC_INMAXP,64);
		OUTREG16(A_UDC_INCSR,0x2048);
		OUTREG8(A_UDC_INDEX,1);
		OUTREG16(A_UDC_OUTMAXP,64);
		OUTREG16(A_UDC_OUTCSR,0x0090);
		USB_Version=USB_FS;

		//fifosize[1]=64;
      
		epstat[1].fifosize = 64;
		epstat[2].fifosize = 64;
		printf("usb1.1\n");
		

	}
	else
	{
		OUTREG8(A_UDC_INDEX,1);
		OUTREG16(A_UDC_INMAXP,512);
		OUTREG16(A_UDC_INCSR,0x2048);
		OUTREG8(A_UDC_INDEX,1);
		OUTREG16(A_UDC_OUTMAXP,512);
		OUTREG16(A_UDC_OUTCSR,0x0090);
		USB_Version=USB_HS;
		epstat[1].fifosize = 512;
		epstat[2].fifosize = 512;
		printf("usb2.0\n");
		
	}
//DMA USE
	OUTREG32(A_UDC_CNTL1,0);
	
	OUTREG32(A_UDC_ADDR1,0);
	OUTREG32(A_UDC_COUNT1,0);//fifosize[ep]);

	OUTREG32(A_UDC_CNTL2,0);
	OUTREG32(A_UDC_ADDR2,0);
	OUTREG32(A_UDC_COUNT2,0);//fifosize[ep]);

	INREG8(A_UDC_INTR); // clear dma interrupt
	OUTREG16(A_UDC_INTRINE,0x1);   //enable ep0 intr
	OUTREG8(A_UDC_INTRUSBE,0x6);
}


static void udc_read_fifo(PEPSTATE pep, int size)
{
	unsigned int *ptr = (unsigned int *)(pep->data_addr + pep->curlen);
	unsigned int fifo = pep->fifo_addr;
	unsigned char *c;
	int s = size / 4;
	unsigned int x;
	if(((unsigned int )ptr & 3) == 0)
	{
		while(s--)
			*ptr++ = INREG32(fifo);
			
	}
	else
	{
		while(s--)
		{
			x = INREG32(fifo);
			*ptr++ = (x >> 0)& 0x0ff;
			*ptr++ = (x >> 8)  & 0x0ff;
			*ptr++ = (x >> 16) & 0x0ff;
			*ptr++ = (x >> 24) & 0xff;
		}
	}
	
	s = size & 3;
	c = (unsigned char *)ptr;
	while(s--)
		*c++ = INREG8(fifo);
	pep->curlen += size;
	
	
#if 0

	c = (unsigned char *)(pep->data_addr + pep->curlen - size);
	dprintf("recv:(%d)", size);
	for (s=0;s<size;s++) {
		if (s % 16 == 0)
			dprintf("\n");
		dprintf(" %02x", *(c+s));
	}
	dprintf("\n");
#endif
}

static void udc_write_fifo(PEPSTATE pep, int size)
{
	unsigned int *d = (unsigned int *)(pep->data_addr + pep->curlen);
	unsigned int fifo = pep->fifo_addr;
	unsigned char *c;
	int s, q;

#if 0
	unsigned char *ptr =(unsigned char *)d;
	
	dprintf("send:fifo(%x) = (%d)",fifo, size);
	for (s=0;s<size;s++) {
		if (s % 16 == 0)
			dprintf("\n");
		dprintf(" %02x", ptr[s]);
	}
	dprintf("\n");
#endif
	
	if (size > 0) {
		s = size >> 2;
		while (s--)
			OUTREG32(fifo,*d++);
		q = size & 3;
		if (q) {
			c = (unsigned char *)d;
			while (q--)
				OUTREG8(fifo,*c++);
				
		}
	} 
	pep->curlen += size;
}
void ep0_handler(unsigned int handle)
{
    PEPSTATE pep = GetEpState(0);
	unsigned char byCSR0;

/* Read CSR0 */
	OUTREG8(A_UDC_INDEX, 0);
	byCSR0 = INREG8(A_UDC_CSR0);
   
//	printf("EP0 CSR = %x\n",byCSR0);
    if(byCSR0 == 0)
		return;
	
/* Check for SentStall 
   if sendtall is set ,clear the sendstall bit*/
	if (byCSR0 & USB_CSR0_SENTSTALL) 
	{
		OUTREG8(A_UDC_CSR0, (byCSR0 & ~USB_CSR0_SENDSTALL));
		pep->state = CPU_READ;
		
		printf("\nSentstall!");
		return;
	}
/* Check for SetupEnd */
	if (byCSR0 & USB_CSR0_SETUPEND) 
	{
		OUTREG8(A_UDC_CSR0, (byCSR0 | USB_CSR0_SVDSETUPEND));
		pep->state = CPU_READ;
		printf("\nSetupend!");
		//return;
	}
/* Call relevant routines for endpoint 0 state */
	if (pep->state == CPU_READ) 
	{
		if (byCSR0 & USB_CSR0_OUTPKTRDY)   //There are datas in fifo
		{
            USB_DeviceRequest dreq;
			g_timeout = 0;
            *(unsigned int *) &dreq =  INREG32(pep->fifo_addr);
			*(unsigned int *) ((unsigned int)&dreq + 4)=  INREG32(pep->fifo_addr);
		
			
			dprintf("\nbmRequestType:%02x\nbRequest:%02x\n"
					"wValue:%04x\nwIndex:%04x\n"
					"wLength:%04x\n",
					dreq.bmRequestType,
					dreq.bRequest,
					dreq.wValue,
					dreq.wIndex,
					dreq.wLength);

			if ( dreq.bRequest == SET_ADDRESS || dreq.bRequest == SET_CONFIGURATION || dreq.bRequest == CLEAR_FEATURE )
				SETREG8(A_UDC_CSR0, 0x40 | USB_CSR0_DATAEND);//clear OUTRD bit and DATAEND
			else
				SETREG8(A_UDC_CSR0, 0x40);//clear OUTRD bit

			BusNotify(handle,UDC_SETUP_PKG_FINISH,(unsigned char *)&dreq,8);
		}
		else 
		{
			dprintf("0:R DATA\n");
			
		}
		
	}
	
	if (pep->state == CPU_WRITE) 
	{
		int sendsize;
		sendsize = pep->totallen - pep->curlen;
//		printf("send size = %d\r\n",sendsize);
		
		if (sendsize < 64) 
		{
			udc_write_fifo(pep,sendsize);
			pep->curlen = pep->totallen;
			SETREG8(A_UDC_CSR0, USB_CSR0_INPKTRDY | USB_CSR0_DATAEND);
			pep->state = CPU_READ;
			
		} else 
		{
			if(sendsize)
			{
				udc_write_fifo(pep, 64);
				SETREG8(A_UDC_CSR0, USB_CSR0_INPKTRDY);
				pep->curlen += 64;
				
			}else
				pep->state = CPU_READ;
			
		}
	}
//	printf("pep state = %d %d\r\n",CPU_WRITE,pep->state);

	return;
}

void ep_in_handler(unsigned int handle,PEPSTATE pep)
{
	unsigned int size;
   
	
	//fifo = fifoaddr[EP];
	size = pep->totallen - pep->curlen;
//	printf("pep->totallen = %d size = %d \n",pep->totallen,size);
	if(size == 0)
	{
		
		pep->state = WRITE_FINISH;
		CLRREG16(A_UDC_INTRINE,EP1_INTR_BIT);  // close ep1 in intr
		BusNotify(handle,UDC_PROTAL_SEND_FINISH,(unsigned char *)pep->data_addr,pep->curlen);
//		printf("Send finish \n");
		return;
		
	}
	
	if(size < pep->fifosize)
	{
		udc_write_fifo(pep,size);
	}else
		udc_write_fifo(pep,pep->fifosize);
	SETREG8(A_UDC_INCSR, USB_INCSR_INPKTRDY);
	
}

void ep_out_handler(unsigned int handle,PEPSTATE pep)
{
    unsigned int size;
	
	OUTREG8(A_UDC_INDEX, 1);

	size = INREG16(A_UDC_OUTCOUNT);	
 	udc_read_fifo(pep, size);
	CLRREG8(A_UDC_OUTCSR,USB_OUTCSR_OUTPKTRDY);
	pep->state = CPU_READ;
	dprintf("EPOUT: totallen = %d curlen = %d",pep->totallen,pep->curlen);
	
	if(pep->totallen == pep->curlen)
	{
		pep->state = READ_FINISH;
		CLRREG16(A_UDC_INTROUTE,EP1_INTR_BIT);
		BusNotify(handle,UDC_PROTAL_RECEIVE_FINISH,
			  (unsigned char *)pep->data_addr,pep->curlen);
		
		
	}
	
	//	USB_HandleUFICmd();
	dprintf("\nEPOUT_handle return!");
}

static int check_gpio(unsigned int pin)
{
	int i ,j  = 0,k = 0;
	for ( i = 0 ; i < 5; i ++ )
	{
		os_TimeDelay(1);
		if (__gpio_get_pin(pin))
			j ++;
		else 
			k ++;
	}
	
	if ( j >= k )
		return 1;
	else 
		return 0;
}

void udc_intrb_handler(unsigned int handle,unsigned char val)
{
	unsigned char byte;
	
	if (val & USB_INTR_RESET) 
	{
		printf("UDC reset interrupt!\r\n");  

		udc_reset(handle);
		if (protocool_stat == CABLE_CONNECTED)
		{
			if ((INREG8(A_UDC_POWER)&0x10) ==0 ) 
			{
				BusNotify(handle,UDC_FULLSPEED,0,0);
			} else
				BusNotify(handle,UDC_HIGHSPEED,0,0);
		
			SETREG8(A_UDC_INTRUSBE, USB_INTR_SUSPEND);
			BusNotify(handle,UDC_RESET,0,0);
		}
	}
	if(val & USB_INTR_SUSPEND)
	{
		BusNotify(handle,UDC_SUSPEND,0,0);
		
		byte = INREG8(A_UDC_INTRUSBE);
		OUTREG8(A_UDC_INTRUSBE,(byte & (~USB_INTR_SUSPEND) & 7));
		printf("udc suspend %x\n",byte);
#ifdef USE_MIDWARE
		if ( protocool_stat == CABLE_CONNECTED )
		{
			printf("USB uninstall ! %d \n",protocool_stat);
			udcsrc.Src = SRC_UDC;
			udcsrc.Event = EVENT_UNINSTALL;
			protocool_stat = CABLE_DISCONNECT;
			os_QueuePost(udcsrc.CurEvent1 , (void *)&udcid);
			os_SemaphorePost(udcsrc.CurEvent);
			os_SemaphorePend(udcsrc.CurEvent2, 0 , &err);

#ifndef DC_DETE_PIN
			/*此处延时影响拔除USB  后面画刷新速度
  			    并间接导致插拔过程出现电池充电图
  			    标*/
			//os_TimeDelay(350);			
			if ( check_gpio(GPIO_UDC_DETE_PIN) )   //cable have not disconnect
			{
				/*Ensure GPIO interrupt triger in corrent status. xfli 08-25*/
				gpio_intr_init(GITT_F_EDGE, GPIO_UDC_DETE_PIN);	
				
				printf("Reas power cable insert! \n");
				udcsrc.Src = SRC_UDC;
				udcsrc.Event = EVENT_POWER_IN;
				os_QueuePost(udcsrc.CurEvent1 , (void *)&udcid);
				os_SemaphorePost(udcsrc.CurEvent);
				os_SemaphorePend(udcsrc.CurEvent2, 0 , &err);
			}
#endif	// DC_DETE_PIN

		}
		else 
			OUTREG8(A_UDC_POWER,0x60);   //High speed	
#endif
	}
	if(val & 2)
	{
		printf("udc resume\n");
	}
}

void udc4755_proc (unsigned int handle)
{
	unsigned char	IntrUSB = 0;
	unsigned short	IntrIn = 0;
	unsigned short	IntrOut = 0;
	unsigned short  IntrDMA = 0;
	PEPSTATE pep;
	//cli();
/* Read interrupt regiters */
	IntrUSB = INREG8(A_UDC_INTRUSB);
/* Check for resume from suspend mode */
	if(IntrUSB != 8)
		udc_intrb_handler(handle,IntrUSB);
	/* Check for endpoint 0 interrupt */
	
	IntrIn  = INREG16(A_UDC_INTRIN);
	IntrOut = INREG16(A_UDC_INTROUT);

	dprintf("IntrIn = %x IntrOut = %x\n",IntrIn,IntrOut);
	
	if (IntrIn & USB_INTR_EP0) 
	{
		if ( (protocool_stat == CABLE_DISCONNECT)
			&& (cable_stat == CABLE_CONNECTED) )
		{
			if (check_gpio(GPIO_UDC_DETE_PIN))
			{
				printf("USB cable insert! \n");
				#ifdef USE_MIDWARE
				udcsrc.Src = SRC_UDC;
				udcsrc.Event = EVENT_USB_IN;
				os_QueuePost(udcsrc.CurEvent1 , (void *)&udcid);
				os_SemaphorePost(udcsrc.CurEvent);
				os_SemaphorePend(udcsrc.CurEvent2, 0, &err);
				if ( res.Val == 1 )  //|| res.Val == 0xffff) //up layer said yes!
				{
					printf("UDC_JUDGE\n");
					protocool_stat = CABLE_CONNECTED;
					
					{// Init udc_bus
						if ((INREG8(A_UDC_POWER)&0x10) ==0 ) 
						{
							BusNotify(handle,UDC_FULLSPEED,0,0);
						}else
							BusNotify(handle,UDC_HIGHSPEED,0,0);
		
						SETREG8(A_UDC_INTRUSBE, USB_INTR_SUSPEND);
						BusNotify(handle,UDC_RESET,0,0);
					}
				}
				else           //up layer said no!
				{
					BusNotify(handle, UDC_REMOVE, 0, 0);
					printf("As power cable insert! \n");
					/*To check usb detect status again and avoid woddle*/
					if (!check_gpio(GPIO_UDC_DETE_PIN))
					{
						/*Ensure GPIO interrupt triger in corrent status.*/
						gpio_intr_init(GITT_R_EDGE, GPIO_UDC_DETE_PIN);
						enable_irq(IRQ_GPIO_UDC_DETE);
						
						udcsrc.Src = SRC_UDC;
						udcsrc.Event = EVENT_POWER_OUT;
						os_QueuePost(udcsrc.CurEvent1, (void *)&udcid);
						os_SemaphorePost(udcsrc.CurEvent);
						//os_SemaphorePend(udcsrc.CurEvent2, 0, &err);

						protocool_stat = CABLE_DISCONNECT;
					}
					return;
				}
				#else
				protocool_stat = CABLE_CONNECTED;
				#endif
			}
		}
		dprintf("\nUDC EP0 operations!\n");
		ep0_handler(handle);
	}	

	pep = GetEpState(1);
	if(pep->state == CPU_WRITE)
	{
		if (IntrIn & 2) 
		{
			dprintf("\nUDC EP1 IN operation!");
			ep_in_handler(handle,pep);
			//return;
		}
	}
	
	IntrDMA = INREG8(A_UDC_INTR);
	//printf("IntrDMA = %x\n",IntrDMA);
	
	if(pep->state == DMA_WRITE)
	{
		dprintf("dma write intr = %x\n",IntrDMA);

				
		if(IntrDMA & 1)
		{
			
			dprintf("addr %x,count %x,cntl %x\r\n",
				   INREG32(A_UDC_ADDR1),INREG32(A_UDC_COUNT1),INREG32(A_UDC_CNTL1));
			dma_send_data_Finish();
			
			if(pep->curlen != pep->totallen)
			{
				/*??????????????*/
				printf("cur_len %d,totallen %d\r\n",pep->curlen,pep->totallen);
				SETREG16(A_UDC_INTRINE,EP1_INTR_BIT);  // open ep1 in intr
				pep->state = CPU_WRITE;
				ep_in_handler(handle,pep);
				
			   
                 //OUTREG8(A_UDC_INDEX, 1);
				//SETREG8(A_UDC_INCSR, USB_INCSR_INPKTRDY);
			}else 
			{
				pep->state = WRITE_FINISH;
				CLRREG16(A_UDC_INTRINE,EP1_INTR_BIT);  // close ep1 in intr
				BusNotify(handle,UDC_PROTAL_SEND_FINISH,
						   (unsigned char *)(pep->data_addr),pep->curlen);
				
				
			}
			//return;
		}
		

	}
	
	pep = GetEpState(2);
	if(pep->state == CPU_READ)
	{
		
		if ((IntrOut /*& IntrOutMask*/ ) & 2) 
		{
			dprintf("UDC EP1 OUT operation!\n");
			ep_out_handler(handle,pep);
			//return;		
		}
	}
	if(pep->state == DMA_READ)
	{
		if(IntrDMA == 0)
			IntrDMA = INREG8(A_UDC_INTR);

		dprintf("\nDMA_REA intrDMA = %x\n",IntrDMA);
		
		if (IntrDMA & 0x2)     //channel 2 :OUT
		{
			dprintf("\n INTR 2!");
			dma_receieve_data_finish();
			if((pep->totallen % pep->fifosize) != 0)
			{
				SETREG16(A_UDC_INTROUTE,EP1_INTR_BIT);	//Enable Ep Out
				pep->state = CPU_READ;
				ep_out_handler(handle,pep);
			}else
			{

				pep->state = READ_FINISH;
				BusNotify(handle,UDC_PROTAL_RECEIVE_FINISH,
						  (unsigned char *)(pep->data_addr),pep->curlen);
			}
			//return;
			

		}
	}
	//sti();
	//dprintf("\n UDCProc finish!\n");
	
	//return;
}

void gpio_handler(unsigned int arg)
{
	disable_irq(IRQ_GPIO_UDC_DETE);
	udc_irq_type |= 0x10;
	os_SemaphorePost(udcEvent);
}

static void gpio_ist(void * arg)
{
	PUDC_BUS pBus = (PUDC_BUS)arg;

	// Delay to check gpio
	os_TimeDelay(5);
	if (check_gpio(GPIO_UDC_DETE_PIN))
	{
		printf("UDC Detect IN!\n");
		gpio_intr_init(GITT_F_EDGE, GPIO_UDC_DETE_PIN);
		if (GetCommonClock(CPM_H0DIV) <= 48000000)
		{
			printf("H0CLK = %d\n", GetCommonClock(CPM_H0DIV));
			int d = 2;
			kernel_ioctl(&d, KERNEL_PM_CLRAUDIOCONTROL);
		}
	}
	else
	{
		printf("UDC Detect OUT!\n");
		gpio_intr_init(GITT_R_EDGE, GPIO_UDC_DETE_PIN);
	}

	if (cable_stat == CABLE_CONNECTED)
	{
		if (check_gpio(GPIO_UDC_DETE_PIN))
		{
			/*Ensure GPIO interrupt triger in corrent status. xfli 08-25*/
			gpio_intr_init(GITT_F_EDGE, GPIO_UDC_DETE_PIN);
			return;
		}	

		cable_stat = CABLE_DISCONNECT;

		if (protocool_stat == CABLE_CONNECTED)
		{
			printf("USB Proto Uninstall! \n");
			protocool_stat = CABLE_DISCONNECT;

#ifdef USE_MIDWARE
			udcsrc.Src = SRC_UDC;
			udcsrc.Event = EVENT_UNINSTALL;			
			os_QueuePost(udcsrc.CurEvent1, (void *)&udcid);
			os_SemaphorePost(udcsrc.CurEvent);
			os_SemaphorePend(udcsrc.CurEvent2, 0, &err);
#endif	// USE_MIDWARE
		}

#ifdef USE_MIDWARE
	#ifndef DC_DETE_PIN
		udcsrc.Src = SRC_UDC;
		udcsrc.Event = EVENT_POWER_OUT;
		os_QueuePost(udcsrc.CurEvent1, (void *)&udcid);
		os_SemaphorePost(udcsrc.CurEvent);
		os_SemaphorePend(udcsrc.CurEvent2, 0, &err);
	#endif	// DC_DETE_PIN
#endif	// USE_MIDWARE

		BusNotify((unsigned int)pBus, UDC_REMOVE, 0, 0);
	}
	else                                    //disconnect!
	{
		if (!check_gpio(GPIO_UDC_DETE_PIN))  //false connect
		{
			/*Ensure GPIO interrupt triger in corrent status. xfli 08-25*/
			gpio_intr_init(GITT_R_EDGE, GPIO_UDC_DETE_PIN);
			return;
		}	

		cable_stat = CABLE_CONNECTED;
		BusNotify((unsigned int)pBus, UDC_JUDGE, 0, 0);
		g_timeout = 300;

		return;
	}
}

static void udc_intr_handler(unsigned int arg)
{
//	__intc_mask_irq(IRQ_UDC);
	disable_irq(IRQ_UDC);
	udc_irq_type |= 0x1;
	os_SemaphorePost(udcEvent);
}
void udc_disable_device_directly_global(void)
{
    udc_irq_type |= 0x20;
	os_SemaphorePost(udcEvent);
}

#if 0
static int check_gpio_new(unsigned int pin, unsigned int level)
{
	unsigned int i, j = 0, k = 0;

	for ( i = 0; i < 3; i ++ )
	{
		os_TimeDelay(1);
		if (__gpio_get_pin(pin))
			j ++;
		else k ++;
	}

	return ((j < k) ^ level);
}
#endif

static void udc_disable_device_directly(void *arg);
static void udc_task_entry(void *arg)
{
	unsigned char err;

	while (1) {
		os_SemaphorePend(udcEvent, g_timeout, &err);
		
		if(err)
		{
			printf("Power cable insert! \n");
#ifdef USE_MIDWARE
	#ifndef DC_DETE_PIN
			udcsrc.Src = SRC_UDC;
			udcsrc.Event = EVENT_POWER_IN;
			os_QueuePost(udcsrc.CurEvent1, (void *)&udcid);
			os_SemaphorePost(udcsrc.CurEvent);
			os_SemaphorePend(udcsrc.CurEvent2, 0 ,&err);
	#else
			udcsrc.Src = SRC_UDC;
			udcsrc.Event = EVENT_USB_OUT;
			os_QueuePost(udcsrc.CurEvent1, (void *)&udcid);
			os_SemaphorePost(udcsrc.CurEvent);
			os_SemaphorePend(udcsrc.CurEvent2, 0, &err);	
	#endif // DC_DETE_PIN
#endif
			g_timeout = 0;
			printf("UDC wait timeout!\n");
		}

		if(udc_irq_type & 0x20){
		    udc_disable_device_directly(arg);
		}
		if(udc_irq_type & 0x10)
		{
			udc_irq_type &= ~0x10;
			gpio_ist(arg);
			enable_irq(IRQ_GPIO_UDC_DETE);
		}
		if(udc_irq_type & 1)
		{
			udc_irq_type &= ~0x1;
			udc4755_proc((unsigned int)arg);
			enable_irq(IRQ_UDC);
		}
	}
}

static void udc_disable_device_directly(void *arg)
{
    PUDC_BUS	pBus = (PUDC_BUS)arg;

    if ( cable_stat == CABLE_CONNECTED )    //connected!
    {
    	if ( protocool_stat == CABLE_CONNECTED )  //info suspend
    	{
    		printf("USB Proto Uninstall! \n");
    		protocool_stat = CABLE_DISCONNECT;
	    	BusNotify((unsigned int)pBus, UDC_REMOVE, 0, 0);
	    	udc_irq_type &= ~0x20;
#ifdef USE_MIDWARE
    		udcsrc.Src = SRC_UDC;
    		udcsrc.Event = EVENT_UNINSTALL;
    		os_QueuePost(udcsrc.CurEvent1, (void *)&udcid);
    		os_SemaphorePost(udcsrc.CurEvent);
    		os_SemaphorePend(udcsrc.CurEvent2, 0, &err);
#endif // USE_MIDWARE
    	}
    
#ifdef USE_MIDWARE
	#ifndef DC_DETE_PIN
    	udcsrc.Src = SRC_UDC;
    	udcsrc.Event = EVENT_POWER_IN;
    	os_QueuePost(udcsrc.CurEvent1, (void *)&udcid);
    	os_SemaphorePost(udcsrc.CurEvent);
    	os_SemaphorePend(udcsrc.CurEvent2, 0, &err);
    #endif // DC_DETE_PIN
#endif // USE_MIDWARE
    }
}

/*   interface   */
static void set_address(unsigned int handle,unsigned short value)
{
	unsigned char	faddr = 0;

	printf("Set address %d\r\n",value);
	OUTREG8(A_UDC_FADDR, value);
	do {
		faddr = INREG8(A_UDC_FADDR);
	} while (faddr & 0x80);
	printf("-Set address OK\r\n");
}

void connect_udc(unsigned char conn)
{
	if(conn)
		OUTREG8(A_UDC_POWER, 0x60);     //enable sofeconnect
	else
		OUTREG8(A_UDC_POWER, 0x00);
}

void enable_device(unsigned int handle)
{
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_UDC);
	enable_irq(IRQ_UDC);
	SETREG32(A_CPM_OPCR, SCR_NO_SUSPEND_UDC);
	//2010.10.18 danny disable and will enable at boot.c by connect_udc function
	//OUTREG8(A_UDC_POWER, 0x60);     //enable sofeconnect

	printf("Enable USB Phy!\r\n");	
}

void disable_device(unsigned int handle)
{
	unsigned char err;
  	
	OUTREG8(A_UDC_POWER, 0x00);      //disable sofeconnet!
	INREG8(A_UDC_INTRUSB);
	INREG16(A_UDC_INTRIN);
	INREG16(A_UDC_INTROUT);
	udc_irq_type = 0;
	os_SemaphoreSet(udcEvent, 0, &err);
	SETREG32(A_CPM_CLKGR, CLKGR_STOP_UDC);
	disable_irq(IRQ_UDC);
	CLRREG32(A_CPM_OPCR, SCR_NO_SUSPEND_UDC);

	printf("Disable USB Phy!\r\n");
}

void start_transfer(unsigned int handle,unsigned char ep,unsigned char *buf,unsigned int len)
{
	PEPSTATE pep;
	dprintf("start_transfer ep = %x buf:%08x len = %d\r\n",ep,buf,len);
	unsigned char state;
	switch(ep)
	{
	case 0:
		pep = GetEpState(0);
		pep->totallen = len;
		pep->curlen = 0;
		pep->data_addr = (unsigned int)buf;		
		pep->state = CPU_WRITE;
		
		break;
	case 0x81:
		pep = GetEpState(1);
		pep->totallen = len;
		pep->curlen = 0;
		pep->data_addr = (unsigned int)buf;
		SETREG16(A_UDC_INTRINE,EP1_INTR_BIT); //open ep1 in intr

		if(len < pep->fifosize)
		{
			pep->state = CPU_WRITE;
			
			OUTREG8(A_UDC_INDEX,1);
			state =  INREG16(A_UDC_INCSR);
			if(!(state & EP_FIFO_NOEMPTY))
			{
				len = len > pep->fifosize ? pep->fifosize :len;
				udc_write_fifo(pep,len);
				SETREG16(A_UDC_INCSR, USB_INCSR_INPKTRDY);		
				
			}
					
				
		}
		else
		{
			OUTREG8(A_UDC_INDEX,1);
			state =  INREG16(A_UDC_INCSR);
			pep->state = DMA_WRITE;
			if(!(state & EP_FIFO_NOEMPTY))
				dma_send_data(pep);	
		}
		break;
		
	case 0x1:
		pep = GetEpState(2);
		pep->totallen = len;
		pep->curlen = 0;
		pep->data_addr = (unsigned int)buf;
		if(len < pep->fifosize)
		{
			
			pep->state = CPU_READ;
			SETREG16(A_UDC_INTROUTE,EP1_INTR_BIT);	//Enable Ep Out
			
		}
		else
		{
			pep->state = DMA_READ;
			dma_receieve_data(pep);
			
		}

		break;
	case 0xff:             //mean send stall!
//		printf("Send Stall! %x \n",INREG16(A_UDC_INCSR));
		SETREG16( A_UDC_INCSR, 0x10);	//set stall
		while( ! (INREG16(A_UDC_INCSR) & 0x20 ) );                //wait stall sent!
		SETREG16( A_UDC_INCSR, 0x60);	             //clear datatag!
		CLRREG16( A_UDC_INCSR, 0x10);	             //clear sendstall
		CLRREG16( A_UDC_INCSR, 0x20);	             //clear sentstall
//		printf("Clear stall! %x \n",INREG16(A_UDC_INCSR));

		break;
	}
}
void init_endpoint_suppost(unsigned int handle,unsigned char *ep,USB_ENDPOINT_TYPE ep_type,unsigned short *ep_max_pkg)
{
	PEPSTATE pep;

	if(ep_type == ENDPOINT_TYPE_CONTROL)
	{
		*ep = 0;
		*ep_max_pkg = MAX_EP0_SIZE;
		
	}
	if(ep_type == ENDPOINT_TYPE_BULK)
	{
		if(*ep & 0x80)
			pep = GetEpState(1);
		else
			pep = GetEpState(2);
		
		*ep = pep->ep;
		*ep_max_pkg = pep->fifosize;
	}
//	printf("ep = %x ep_type = %x epmax = %x\r\n",*ep,ep_type,pep->fifosize);	
}

#ifdef USE_MIDWARE
static void GetRequest(MIDSRCDTA *dat)
{
	dat->Val = res.Val;
	printf("Up layer get :%d \n",res.Val);
}

static void Response(MIDSRCDTA *dat)
{
	res.Val = dat->Val;
	printf("Up layer said :%d \n",res.Val);
}
#endif
#define UdcID (('U' << 24) | ('D' << 16) | ('C' << 16) | (':' << 16))  
static void udcSetWakeUp()
{
	enable_irq(IRQ_GPIO_UDC_DETE);
}

static void udcWakeUp()
{
}

void init_udc(PUDC_BUS pBus)
{
	PM_AddWakeUp(UdcID,udcSetWakeUp,udcWakeUp);
	pBus->EnableDevice = enable_device;
	pBus->SetAddress = set_address;
	pBus->StartTransfer = start_transfer;
	pBus->InitEndpointSuppost = init_endpoint_suppost;
	pBus->DisableDevice = disable_device;
	printf("Init UDC %s %s\n",__DATE__,__TIME__);

#ifdef USE_MIDWARE
	udcsrc.GetRequest = GetRequest;
	udcsrc.Response = Response;
	udcsrc.Name = "UDC";
	printf("Register Midware SRC udc! \n");
	RegisterMidSrc((PMIDSRC)&udcsrc);
	udcid = udcsrc.ID;
//	res.Val = 0xffff;

#endif	
	cable_stat = CABLE_DISCONNECT;
	protocool_stat = CABLE_DISCONNECT;

	USB_Version=USB_HS;

#ifdef CHARG_STAT_PIN
	// Init CHARG_STAT_PIN and DC_DETE_PIN
	gpio_set_dir(CHARG_STAT_PIN, 0, 0);
#endif
#ifdef DC_DETE_PIN
	gpio_set_dir(DC_DETE_PIN, 0, 0);
#endif // DC_DETE_PIN

	udcEvent = os_SemaphoreCreate(0);
	printf("Create UDC Task!!\r\n");
	os_TaskCreate(udc_task_entry, (void *)pBus,
		     (void *)&udcTaskStack[UDC_TASK_STK_SIZE - 1],
		     UDC_TASK_PRIO);

	printf("request_irq %d\n", IRQ_UDC);
	request_irq(IRQ_UDC, udc_intr_handler, 0);
	disable_irq(IRQ_UDC);

	request_irq(IRQ_GPIO_UDC_DETE, gpio_handler, pBus);
	disable_irq(IRQ_GPIO_UDC_DETE);
	gpio_intr_init(GITT_R_EDGE, GPIO_UDC_DETE_PIN);
	__gpio_disable_pull(GPIO_UDC_DETE_PIN);

	udc_reset((unsigned int)pBus);
	
#if 1
#ifdef UDC_MODULE_KEY
	if (( __gpio_get_pin(GPIO_UDC_DETE_PIN) == 1 ) && (ReadKeyDirect() == UDC_MODULE_KEY))
#else
	if ( __gpio_get_pin(GPIO_UDC_DETE_PIN) == 1 )
#endif
	{
		disable_irq(IRQ_GPIO_UDC_DETE);
		udc_irq_type |= 0x10;
		os_SemaphorePost(udcEvent);
	}
	else
		enable_irq(IRQ_GPIO_UDC_DETE);

	#define PWM_FREQ		(1 * 1000)	// Condigure according to the BKL chip
	pwm_init(2, PWM_FREQ, 2);
	pwm_set_duty(2, 1);
#endif
}

int udc_detect_status(void)
{
	if (protocool_stat == CABLE_CONNECTED)
		return 0;
	else
		return 1;
}

int udc_detect_cable_status(void)
{
#ifndef DC_DETE_PIN
	printf("udc cable_stat = %d\n", (cable_stat == CABLE_CONNECTED));
    return (cable_stat == CABLE_CONNECTED);
#else
	printf("udc protocool_stat = %d\n", (protocool_stat == CABLE_CONNECTED));
	return (protocool_stat == CABLE_CONNECTED);
#endif // DC_DETE_PIN
}

/*For ensure usb detect status */
#ifdef USE_MIDWARE
BOOL  udc_get_real_detect_status(void)
{
	int ret;
	
	ret = check_gpio(GPIO_UDC_DETE_PIN);
	if(ret)
		return TRUE;
	else
		return FALSE;
}
#endif

