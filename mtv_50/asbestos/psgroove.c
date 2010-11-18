/*
    PSGroove Exploit

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <string.h>
#include <stdio.h>
#include "jz4755.h"
#include "usb.h"
#include "udc.h"
#include "usb_boot.h"
#include "descriptor.h"
#include "stage1.h"
#include "stage2.h"
#include "port1_config_descriptor.h"

//#undef printf
//#define printf(x,args...)
#define LED_RED_IO		(GPIO_GROUP_A + 17)
#define LED_GREEN_IO	(GPIO_GROUP_B + 23)
#define LED_TURN_ON		0
#define LED_TURN_OFF	1

#define PORT_EMPTY 0x0100   /* powered only */
#define PORT_FULL 0x0103    /* connected, enabled, powered, full-speed */
#define C_PORT_CONN  0x0001 /* connection */
#define C_PORT_RESET 0x0010 /* reset */
#define C_PORT_NONE  0x0000 /* no change */
u16 port_status[6] = { PORT_EMPTY, PORT_EMPTY, PORT_EMPTY, PORT_EMPTY, PORT_EMPTY, PORT_EMPTY };
u16 port_change[6] = { C_PORT_NONE, C_PORT_NONE, C_PORT_NONE, C_PORT_NONE, C_PORT_NONE, C_PORT_NONE };
enum { 
	init,
	wait_hub_ready,
	hub_ready,
	p1_wait_reset,
	p1_wait_enumerate,
	p1_ready,
	p2_wait_reset,
	p2_wait_enumerate,
	p2_ready,
	p3_wait_reset,
	p3_wait_enumerate,
	p3_ready,
	p2_wait_disconnect,
	p4_wait_connect,
	p4_wait_reset,
	p4_wait_enumerate,
	p4_ready,
	p5_wait_reset,
	p5_wait_enumerate,
	p5_challenged,
	p5_responded,
	p3_wait_disconnect,
	p3_disconnected,
	p5_wait_disconnect,
	p5_disconnected,
	p4_wait_disconnect,
	p4_disconnected,
	p1_wait_disconnect,
	p1_disconnected,
	p6_wait_reset,
	p6_wait_enumerate,
	done,
} state = init;

u8 hub_int_response = 0x00;
u8 hub_int_force_data0 = 0;
int last_port_conn_clear = 0;
int last_port_reset_clear = 0;

s8 port_addr[7] = { -1, -1, -1, -1, -1, -1, -1 };
s8 port_cur = -1;

volatile u8 expire = 0; /* counts down every 10 milliseconds */
volatile u16 IntrIn;

u8 packet_buff[64];

#define TXFIFOEP0 A_UDC_FIFO_EP0

//u32 Bulk_in_buf[BULK_IN_BUF_SIZE];
u8 Bulk_out_buf[BULK_OUT_BUF_SIZE];
u32 Bulk_in_size,Bulk_in_finish,Bulk_out_size;
//u8 udc_state;

u32 rx_buf[32];
//u32 tx_buf[32];
u8 *tx_buf;
u32 tx_size, rx_size, finished,fifo;
u8 ep0state;

u32 fifoaddr[] = 
{
	TXFIFOEP0, TXFIFOEP0+4 ,TXFIFOEP0+8
};

u32 fifosize[] = {
	MAX_EP0_SIZE, MAX_EP1_SIZE
};

extern unsigned int *pps3_boot;
unsigned int mmc_card_plugin;

inline void led_set(int io, int status)
{
	if(status)
		__gpio_set_pin(io);
	else
		__gpio_clear_pin(io);
}

inline void LED(int io)
{
	if(io == LED_RED_IO)
	{
		led_set(LED_RED_IO, LED_TURN_ON);
		led_set(LED_GREEN_IO, LED_TURN_OFF);
	}
	else
	{
		led_set(LED_RED_IO, LED_TURN_OFF);
		led_set(LED_GREEN_IO, LED_TURN_ON);
	}
}

inline void led_set_dual(void)
{
	led_set(LED_RED_IO, LED_TURN_ON);
	led_set(LED_GREEN_IO, LED_TURN_ON);
}

void switch_port(s8 port)
{
	if (port_cur == port)
		return;
	port_cur = port;
	if (port_addr[port] < 0)
		port_addr[port] = 0;
	jz_writeb(A_UDC_FADDR,port_addr[port] & 0x7f);
	unsigned char	faddr = 0;
	do {
		faddr = INREG8(A_UDC_FADDR);
	} while (faddr & 0x80);
}

void panic(void)
{
	serial_puts("panic error\n");

	led_set_dual();
	
	*pps3_boot = 0;
	//LED(LED_RED_IO);
	LoaderTask();
	while(1);
}

void timer_handler(unsigned int arg) 
{ 
	if (expire > 0)
		expire--;
	jz_writel(A_TCU_TFCR, TCU_INTR_FLAG(0));
}

void udcReadFifo(u8 *ptr, int size)
{
	u32 *d = (u32 *)ptr;
	int s;
	s = (size + 3) >> 2;
	while (s--)
		*d++ = REG32(fifo);
}

void udcWriteFifo(u8 *ptr, int size)
{
	u32 *d = (u32 *)ptr;
	u8 *c;
	int s, q;

	if (size > 0) {
		s = size >> 2;
		while (s--)
			REG32(fifo) = *d++;
		q = size & 3;
		if (q) {
			c = (u8 *)d;
			while (q--)
				REG8(fifo) = *c++;
		}
	} 
}

void HW_SendPKT(int ep, const u8 *buf, int size)
{
//	printf("EP%d send pkt :%d\n", ep, size);
	fifo = fifoaddr[ep];

	if (ep!=0)
	{
		Bulk_in_size = size;
		Bulk_in_finish = 0;
		jz_writeb(A_UDC_INDEX, ep);
		if (Bulk_in_size - Bulk_in_finish <= fifosize[ep]) 
		{
			udcWriteFifo((u8 *)((u32)buf+Bulk_in_finish),
				     Bulk_in_size - Bulk_in_finish);
			usb_setb(A_UDC_INCSR, USB_INCSR_INPKTRDY);
			Bulk_in_finish = Bulk_in_size;
		} else 
		{
			udcWriteFifo((u8 *)((u32)buf+Bulk_in_finish),
				     fifosize[ep]);
			usb_setb(A_UDC_INCSR, USB_INCSR_INPKTRDY);
			Bulk_in_finish += fifosize[ep];
		}
	}
	else  //EP0
	{
		tx_size = size;
		finished = 0;
		//memcpy((void *)tx_buf, buf, size);
		tx_buf = (u8 *)buf;
		ep0state = USB_EP0_TX;		
	}
}

void HW_GetPKT(int ep, const u8 *buf, int size)
{
//	printf("EP%d read pkt :%d\n", ep, size);
	memcpy((void *)buf, (u8 *)rx_buf, size);
	fifo = fifoaddr[ep];
	if (rx_size > size)
		rx_size -= size;
	else {
		size = rx_size;
		rx_size = 0;
	}
	memcpy((u8 *)rx_buf, (u8 *)((u32)rx_buf+size), rx_size);
}

void HUB_Task(void)
{
#if 0
	Endpoint_SelectEndpoint(1);

	if (Endpoint_IsReadWriteAllowed())
	{
		if (hub_int_response) {
			if (hub_int_force_data0) {
				Endpoint_ResetDataToggle();
				hub_int_force_data0 = 0;
			}
			Endpoint_Write_Byte(hub_int_response);
			Endpoint_ClearIN();
			hub_int_response = 0x00;
		}
	}
#else
	//if (IntrIn & 2) 
	{
		//printf("\nUDC EP1 IN operation!");
		if (hub_int_response) {
			jz_writeb(A_UDC_INDEX, 1);
			if (hub_int_force_data0) {
				usb_setb(A_UDC_INCSR, USB_INCSR_CDT);
				hub_int_force_data0 = 0;
			}
			fifo = fifoaddr[1];
			udcWriteFifo((u8 *)&hub_int_response, 1);
			usb_setw(A_UDC_INCSR, USB_INCSR_INPKTRDY);
			hub_int_response = 0x00;
		}
	}
#endif
}

const u8 jig_response[64] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xee, 0x78, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xee, 0x88,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x33, 0xe7, 0x20, 0xe8, 0x83, 0xff, 0xf0, 0xe8, 0x63, 0xff, 0xf8,
	0xe8, 0xa3, 0x00, 0x18, 0x38, 0x63, 0x10, 0x00, 0x7c, 0x04, 0x28, 0x00, 0x40, 0x82, 0xff, 0xf4,
	0x38, 0xc3, 0xf0, 0x20, 0x7c, 0xc9, 0x03, 0xa6, 0x4e, 0x80, 0x04, 0x20, 0x04, 0x00, 0x00, 0x00,
};

void JIG_Task(void)
{
	//注意这里是静态变量，貌似破解只运行一遍就退出了，
	//所以这里没有重新赋值
	static int bytes_out = 0, bytes_in = 0;	
#if 0
        Endpoint_SelectEndpoint(2);
        if (Endpoint_IsReadWriteAllowed())
        {
		Endpoint_Discard_Stream(8, NO_STREAM_CALLBACK);
                Endpoint_ClearOUT();
		bytes_out += 8;
		if (bytes_out >= 64) {
			state = p5_challenged;
			expire = 50; // was 90
		}
	}

        Endpoint_SelectEndpoint(1);
        if (Endpoint_IsReadWriteAllowed() && state == p5_challenged && expire == 0) 
	{
		if (bytes_in < 64) {
			Endpoint_Write_PStream_LE(&jig_response[bytes_in], 8, NO_STREAM_CALLBACK);
			Endpoint_ClearIN();
			bytes_in += 8;
			if (bytes_in >= 64) {
				state = p5_responded;
				expire = 15;
			}
		}
	}
#else
	volatile u16	IntrOut;

	IntrOut = jz_readw(A_UDC_INTROUT);
	if (IntrOut & 2) 
	{
		printf("\nUDC EP1 OUT operation!");
		u32 size;
		jz_writeb(A_UDC_INDEX, 1);
		size = jz_readw(A_UDC_OUTCOUNT);
		fifo = fifoaddr[1];
		udcReadFifo((u8 *)((u32)Bulk_out_buf), size);	//Discard_Stream
		usb_clearb(A_UDC_OUTCSR,USB_OUTCSR_OUTPKTRDY);
		bytes_out += size;
		if (bytes_out >= 64) {
			state = p5_challenged;
			expire = 50; // was 90
		}
	}

	jz_writeb(A_UDC_INDEX, 1);
	volatile u16 IntrCsr = jz_readw(A_UDC_INCSR);
	if (((IntrCsr & USB_INCSR_FFNOTEMPT)==0) && (state == p5_challenged) && (expire == 0))
	{
		printf("\nUDC EP1 IN operation!");
		fifo = fifoaddr[1];

		if (bytes_in < 64) {
			udcWriteFifo((u8 *)&jig_response[bytes_in], 8);
			usb_setw(A_UDC_INCSR, USB_INCSR_INPKTRDY);
			bytes_in += 8;
			if (bytes_in >= 64) {
				state = p5_responded;
				expire = 15;
			}
		}
	}
#endif
}

void connect_port(int port)
{
	last_port_reset_clear = 0;
	hub_int_response = (1 << port);
	port_status[port - 1] = PORT_FULL;
	port_change[port - 1] = C_PORT_CONN;
}

void disconnect_port(int port)
{
	last_port_conn_clear = 0;
	hub_int_response = (1 << port);
	port_status[port - 1] = PORT_EMPTY;
	port_change[port - 1] = C_PORT_CONN;
}

u16 CALLBACK_USB_GetDescriptor(u8 *buf, const void** const DescriptorAddress)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	const u8  DescriptorType   = (dreq->wValue >> 8);
	const u8  DescriptorNumber = (dreq->wValue & 0xFF);
	const u16  wLength = dreq->wLength;

	void* Address = NULL;
	u16 Size = 0;

	//if(mmc_card_plugin)
	//	return PL3_USB_GetDescriptor(buf,DescriptorAddress);

	switch (DescriptorType)
	{
	case DEVICE_DESCRIPTOR:
		switch (port_cur) {
		case 0:
			Address = (void *) HUB_Device_Descriptor;
			Size    = sizeof(HUB_Device_Descriptor);
			break;
		case 1:
			Address = (void *) port1_device_descriptor;
			Size    = sizeof(port1_device_descriptor);
			break;
		case 2:
			Address = (void *) port2_device_descriptor;
			Size    = sizeof(port2_device_descriptor);
			break;
		case 3:
			Address = (void *) port3_device_descriptor;
			Size    = sizeof(port3_device_descriptor);
			break;
		case 4:
			Address = (void *) port4_device_descriptor;
			Size    = sizeof(port4_device_descriptor);
			break;
		case 5:
			Address = (void *) port5_device_descriptor;
			Size    = sizeof(port5_device_descriptor);
			break;
		case 6:
			Address = (void *) port6_device_descriptor;
			Size    = sizeof(port6_device_descriptor);
			break;
		}
		break;
	case CONFIGURATION_DESCRIPTOR: 
		switch (port_cur) {
		case 0:
			Address = (void *) HUB_Config_Descriptor;
			Size    = sizeof(HUB_Config_Descriptor);
			break;
		case 1:
			// 4 configurations are the same.
			// For the initial 8-byte request, we give a different
			// length response than in the full request.
			if (DescriptorNumber < 4) {
				if (wLength == 8) {
					Address = (void *) port1_short_config_descriptor;
					Size    = sizeof(port1_short_config_descriptor);
				} else {
					Address = (void *) port1_config_descriptor;
					Size    = sizeof(port1_config_descriptor);
				}
				if (DescriptorNumber == 3 && wLength > 8) {
					state = p1_ready;
					expire = 10;
				}
			}
			break;
		case 2:
			// only 1 config
			Address = (void *) port2_config_descriptor;
			Size    = sizeof(port2_config_descriptor);
			state = p2_ready;
			expire = 15;
			break;
		case 3:
			// 2 configurations are the same
			Address = (void *) port3_config_descriptor;
			Size    = sizeof(port3_config_descriptor);
			if (DescriptorNumber == 1 && wLength > 8) {
				state = p3_ready;
				expire = 10;
			}
			break;
		case 4:
			// 3 configurations
			if (DescriptorNumber == 0) {
				Address = (void *) port4_config_descriptor_1;
				Size    = sizeof(port4_config_descriptor_1);
			} else if (DescriptorNumber == 1) {
				if (wLength == 8) {
					Address = (void *) port4_short_config_descriptor_2;
					Size    = sizeof(port4_short_config_descriptor_2);
				} else {
					Address = (void *) port4_config_descriptor_2;
					Size    = sizeof(port4_config_descriptor_2);
				}
			} else if (DescriptorNumber == 2) {
				Address = (void *) port4_config_descriptor_3;
				Size    = sizeof(port4_config_descriptor_3);
				if (wLength > 8) {
					state = p4_ready;
					expire = 20;  // longer seems to help this one?
				}
			}
			break;
		case 5:
			// 1 config
			Address = (void *) port5_config_descriptor;
			Size    = sizeof(port5_config_descriptor);
			break;
		case 6:
			// 1 config
			Address = (void *) port6_config_descriptor;
			Size    = sizeof(port6_config_descriptor);
			break;
		}
		break;
	case 0x29: // HUB descriptor (always to port 0 we'll assume)
		switch (port_cur) {
		case 0:
			Address = (void *) HUB_Hub_Descriptor;
			Size    = sizeof(HUB_Hub_Descriptor);
			break;
		}
		break;
	}
	
	*DescriptorAddress = Address;
	return Size;
}

void usbHandleStandDevReq(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	const void* DescriptorPointer;
	u16    DescriptorSize;
	int i;
	int bError = False, bNoData = False, by;

	switch (dreq->bRequest) {
	case GET_DESCRIPTOR:
		if (dreq->bmRequestType == 0x80)	/* Dev2Host */
		{
			DescriptorSize = CALLBACK_USB_GetDescriptor(buf, &DescriptorPointer);
			if(DescriptorSize)
				HW_SendPKT(0, (u8 *)DescriptorPointer, (DescriptorSize>dreq->wLength?dreq->wLength:DescriptorSize));
		}

		//printf("Descriptor is ");
		//for(i=0;i<DescriptorSize;i++)
		//{
		//	printf("%x ", ((u8 *)DescriptorPointer)[i]);
		//}
		//pntf("%\n");

		ep0state=USB_EP0_TX;
		
		break;
	case SET_ADDRESS:
		//printf("SET_ADDRESS!\n");
		jz_writeb(A_UDC_FADDR,dreq->wValue);
		//update port address
		port_addr[port_cur] = dreq->wValue & 0x7f;
		bNoData = True;
		break;
	case GET_STATUS:
		switch (dreq->bmRequestType) {
		case 80:	/* device */
			HW_SendPKT(0, "\x01\x00", 2);
			break;
		case 81:	/* interface */
		case 82:	/* ep */
			HW_SendPKT(0, "\x00\x00", 2);
			break;
		}
		ep0state=USB_EP0_TX;
		break;
	case SET_CONFIGURATION:
	case SET_INTERFACE:
	case CLEAR_FEATURE:
	case SET_FEATURE:
		bNoData = True;
		break;
	default:
		SETREG8(A_UDC_CSR0, USB_CSR0_SENTSTALL);
		panic();
		break;
	}
	
}

void usbHandleClassReq(u8 *buf)
{
	int ret_state;
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;

	if (port_cur == 0 &&
	    dreq->bmRequestType == 0xA0 &&
	    dreq->bRequest == 0x00 &&  // GET HUB STATUS
	    dreq->wValue == 0x00 &&
	    dreq->wIndex == 0x00 &&
	    dreq->wLength == 0x04) {
	    memcpy(packet_buff, "\x00\x00", 2); // wHubStatus
	    memcpy(packet_buff+2, "\x00\x00", 2); // wHubChange
		HW_SendPKT(0, packet_buff, 4);
		ep0state=USB_EP0_TX;
		return;
	}

	if (port_cur == 0 &&
	    dreq->bmRequestType == 0xA3 &&  
	    dreq->bRequest == 0x00 &&   //  GET PORT STATUS
	    dreq->wValue == 0x00 &&
	    dreq->wLength == 0x04) {
		u8 p = dreq->wIndex;
		if (p < 1 || p > 6) return;

		memcpy(packet_buff, (u8 *)&port_status[p - 1], 2); // wHubStatus
		memcpy(packet_buff+2, (u8 *)&port_change[p - 1], 2); // wHubChange
		HW_SendPKT(0, packet_buff, 4);
		ep0state=USB_EP0_TX;
		return;
	}

	if (port_cur == 0 &&
	    dreq->bmRequestType == 0x23 &&
	    dreq->bRequest == 0x03 && // SET_FEATURE
	    dreq->wLength == 0x00) {
		u8 p = dreq->wIndex;
		if (p < 1 || p > 6) return;
		
		switch(dreq->wValue) {
		case 0x0008: // PORT_POWER
			if (p == 6 && state == init) {
				/* after the 6th port is powered, wait a bit and continue */
				state = hub_ready;
				expire = 15;
			}
			break;
		case 0x0004: // PORT_RESET
			hub_int_response = (1 << p);
			port_change[p - 1] |= C_PORT_RESET;
			break;
		//2010.10.17 danny added
		default:
			panic();
			break;
		}
		return;
	}

	if (port_cur == 0 &&
	    dreq->bmRequestType == 0x23 &&
	    dreq->bRequest == 0x01 && // CLEAR_FEATURE
	    dreq->wLength == 0x00) {
		u8 p = dreq->wIndex;
		if (p < 1 || p > 6) return;
		
		switch(dreq->wValue) {
		case 0x0010: // C_PORT_CONNECTION
			port_change[p - 1] &= ~C_PORT_CONN;
			last_port_conn_clear = p;
			break;
		case 0x0014: // C_PORT_RESET
			port_change[p - 1] &= ~C_PORT_RESET;
			last_port_reset_clear = p;
			break;
		//2010.10.17 danny added
		default:
			panic();
			break;
		}
		return;
	}
	
	if (dreq->bmRequestType == 0xa0 && dreq->bRequest == 0x06 )
	{
		const void* DescriptorPointer;
		u16    DescriptorSize;
		DescriptorSize = CALLBACK_USB_GetDescriptor(buf, &DescriptorPointer);
		if(DescriptorSize)
			HW_SendPKT(0, (u8 *)DescriptorPointer, (DescriptorSize>dreq->wLength?dreq->wLength:DescriptorSize));
		else
			panic();	//if size == 0, it's plug in pc and direct to minios
			//HW_SendPKT(0, (u8 *)DescriptorPointer, DescriptorSize);
		ep0state=USB_EP0_TX;
		return;
	}
	//SETREG8(A_UDC_CSR0, USB_CSR0_SENTSTALL);

}

void usbHandleVendorReq(u8 *buf)
{
	USB_DeviceRequest *dreq = (USB_DeviceRequest *)buf;
	int i;
	int bError = False, bNoData = False;
	int len;
	int stage2_size = sizeof(stage2_bin);

	if (dreq->bmRequestType == TYPE_DEV2HOST && dreq->bRequest == REQ_GET_STAGE2_SIZE) {
		printf("get_stage2_size()\n");
		u8 reply[4] = {stage2_size>>24, stage2_size>>16, stage2_size>>8, stage2_size};
		memcpy(packet_buff, reply, 4);
		HW_SendPKT(0, packet_buff, 4);
		ep0state=USB_EP0_TX;
		return;
	}
	if (dreq->bmRequestType == TYPE_DEV2HOST && dreq->bRequest == REQ_READ_STAGE2_BLOCK) {
		int offset = dreq->wIndex<<12;
		int available = stage2_size - offset;
		int length = dreq->wLength;
		printf("read_stage2_block(offset=0x%x,len=0x%x)\n", offset, length);
		if (available < 0)
			available = 0;
		if (length > available) {
			printf("warning: length exceeded, want 0x%x, avail 0x%x\n", length, available);
			length = available;
		}
		HW_SendPKT(0, &stage2_bin[offset], length);
		return;
	}
	if (dreq->bmRequestType == TYPE_HOST2DEV && dreq->bRequest == REQ_PRINT) {
		char buf[0x400];		
		int do_hdr = 1;
		int read = 0;
		u8 *p = buf;

		if (dreq->wLength > 0x400) {
			printf("WTF\n");
			return;
		}
		// clear irqs
		jz_writeb(A_UDC_INDEX, 0);
		jz_readw(A_UDC_INTROUT);
		len = dreq->wLength;
		while(len) {
			while (!(jz_readb(A_UDC_CSR0) & USB_CSR0_OUTPKTRDY));
			int blen = jz_readb(A_UDC_COUNT0);
			if (blen == 0)
				break;
			if (blen > len) {
				printf("lolwut %d > %d\n", blen, len);
				while(1);
			}
			fifo = fifoaddr[0];
			for(i=0; i<blen; i++) {
				*p++ = REG8(fifo);
			}
			len -= blen;
			read += blen;
			if (len) {
				SETREG8(A_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY);
			} else {
				SETREG8(A_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND);
			}
		}

		p = buf;
		while(len--) {
			if (do_hdr) {
				do_hdr = 0;
				printf("[LV2] ");
			}
			printf("%c", *p);
			if (*p == '\n')
				do_hdr = 1;
			p++;
		}
		return;
	}

}

void usbHandleDevReq(u8 *buf)
{
	if (port_cur == 6 && buf[1] == 0xAA) {
		/* holy crap, it worked! */
		state = done;
		SETREG8(A_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND);
		return;
	}		
	
//	printf("dev req:%d\n", (buf[0] & (3 << 5)) >> 5);
	switch ((buf[0] & (3 << 5)) >> 5) {
	case 0: /* Standard request */
		usbHandleStandDevReq(buf);
		break;
	case 1: /* Class request */
		usbHandleClassReq(buf);
		break;
	case 2: /* Vendor request */
		usbHandleVendorReq(buf);
		break;
	} 
}

void udc_reset(void)
{
	u8 byte;
	//data init
	ep0state = USB_EP0_IDLE;
	Bulk_in_size = 0;
	Bulk_in_finish = 0;
	Bulk_out_size = 0;
	//udc_state = IDLE;
	tx_size = 0;
	rx_size = 0;
	finished = 0;
	/* Enable the USB PHY */
//	REG_CPM_SCR |= CPM_SCR_USBPHY_ENABLE;
	/* Disable interrupts */
	byte=jz_readb(A_UDC_POWER);
	jz_writew(A_UDC_INTRINE, 0);
	jz_writew(A_UDC_INTROUTE, 0);
	jz_writeb(A_UDC_INTRUSBE, 0);
	jz_writeb(A_UDC_FADDR,0);
	jz_writeb(A_UDC_POWER,0x40);   //Full speed
	jz_writeb(A_UDC_INDEX,0);
	jz_writeb(A_UDC_CSR0,0xc0);
	jz_writeb(A_UDC_INDEX,1);
	jz_writew(A_UDC_INMAXP,512);
	jz_writew(A_UDC_INCSR,0x2048);
	jz_writeb(A_UDC_INDEX,1);
	jz_writew(A_UDC_OUTMAXP,512);
	jz_writew(A_UDC_OUTCSR,0x0090);
	jz_writew(A_UDC_INTRINE,0x7);   //enable intr
	jz_writew(A_UDC_INTROUTE,0x2);
	jz_writeb(A_UDC_INTRUSBE,0x4);

	byte=jz_readb(A_UDC_POWER);
//	printf("\nREG_POWER: %02x",byte);
	if ((byte&0x10)==0) 
	{
		jz_writeb(A_UDC_INDEX,1);
		jz_writew(A_UDC_INMAXP,8);
		jz_writew(A_UDC_INCSR,0x2048);
		jz_writeb(A_UDC_INDEX,1);
		jz_writew(A_UDC_OUTMAXP,8);
		jz_writew(A_UDC_OUTCSR,0x0090);
		fifosize[1]=64;
		//EP0_init(1,64,1,64);
	}
	else
	{
		jz_writeb(A_UDC_INDEX,1);
		jz_writew(A_UDC_INMAXP,512);
		jz_writew(A_UDC_INCSR,0x2048);
		jz_writeb(A_UDC_INDEX,1);
		jz_writew(A_UDC_OUTMAXP,512);
		jz_writew(A_UDC_OUTCSR,0x0090);
		fifosize[1]=512;
		//EP0_init(1,512,1,512);
	}
}

void USB_EP0Task ()
{
	u8 byCSR0;
	if(!(IntrIn & USB_INTR_EP0))
	{
		return;
	}

	/* Read CSR0 */
	jz_writeb(A_UDC_INDEX, 0);
	byCSR0 = jz_readb(A_UDC_CSR0);

	//printf("EP0 CSR = %x\n",byCSR0);
	//if(byCSR0 == 0)
	//	return;

	/* Check for SentStall 
	if sendtall is set ,clear the sendstall bit*/
	if (byCSR0 & USB_CSR0_SENTSTALL) 
	{
		jz_writeb(A_UDC_CSR0, (byCSR0 & ~USB_CSR0_SENDSTALL));
		ep0state = USB_EP0_IDLE;
		printf("\nSentstall!");
		return;
	}

	/* Check for SetupEnd */
	if (byCSR0 & USB_CSR0_SETUPEND) 
	{
		jz_writeb(A_UDC_CSR0, (byCSR0 | USB_CSR0_SVDSETUPEND));
		ep0state = USB_EP0_IDLE;
		printf("\nSetupend!");
		return;
	}
	/* Call relevant routines for endpoint 0 state */
	if (ep0state == USB_EP0_IDLE) 
	{
		if (byCSR0 & USB_CSR0_OUTPKTRDY)   //There are datas in fifo
		{
			USB_DeviceRequest *dreq;
			fifo=fifoaddr[0];
			udcReadFifo((u8 *)rx_buf, sizeof(USB_DeviceRequest));
			dreq = (USB_DeviceRequest *)rx_buf;
#if 1
			printf("\nbmRequestType:%02x\nbRequest:%02x\n"
				"wValue:%04x\nwIndex:%04x\n"
				"wLength:%04x\n",
				dreq->bmRequestType,
				dreq->bRequest,
				dreq->wValue,
				dreq->wIndex,
				dreq->wLength);
#endif
			if ( dreq->bRequest == SET_ADDRESS || dreq->bRequest == SET_CONFIGURATION || dreq->bRequest == CLEAR_FEATURE )
				SETREG8(A_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY | USB_CSR0_DATAEND);//clear OUTRD bit and DATAEND
			else
				SETREG8(A_UDC_CSR0, USB_CSR0_SVDOUTPKTRDY);
			//SETREG8(A_UDC_CSR0, 0x48);
			usbHandleDevReq((u8 *)rx_buf);
		} else 
		{
			//printf("0:R DATA\n");
		}
		rx_size = 0;
	}
	
	if (ep0state == USB_EP0_TX) 
	{
		fifo=fifoaddr[0];
		if (tx_size - finished <= 8) 
		{
			udcWriteFifo((u8 *)((u32)tx_buf+finished),
				     tx_size - finished);
			finished = tx_size;
			usb_setb(A_UDC_CSR0, USB_CSR0_INPKTRDY);
			usb_setb(A_UDC_CSR0, USB_CSR0_DATAEND); //Set dataend!
			ep0state=USB_EP0_IDLE;
		} else 
		{
			udcWriteFifo((u8 *)((u32)tx_buf+finished), 8);
			usb_setb(A_UDC_CSR0, USB_CSR0_INPKTRDY);
			finished += 8;
		}
	}
	return;
}


void enable_device(unsigned int handle)
{
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_UDC);
	SETREG32(A_CPM_OPCR, SCR_NO_SUSPEND_UDC);
	OUTREG8(A_UDC_POWER, 0x40);     //enable sofeconnect
	serial_puts("Enable USB Phy!\r\n");	
}

void disable_device(unsigned int handle)
{
	unsigned char err;
  	
	jz_writeb(A_UDC_POWER, 0x00);      //disable sofeconnet!
	serial_puts("Disable USB Phy!\r\n");
}


int psgroove(void)
{
	volatile u8 IntrUSB;

	state = init;
	switch_port(0);

	for (;;)
	{
		IntrUSB = jz_readb(A_UDC_INTRUSB);
		if (IntrUSB & USB_INTR_RESET) 
		{
			serial_puts("\nUDC reset intrupt!");  
			udc_reset();
			continue;
		}

		IntrIn	= jz_readw(A_UDC_INTRIN);
		
		//printf("IntrIn=0x%x\n", IntrIn);
		if (port_cur == 0)
			HUB_Task();

		if (port_cur == 5)
			JIG_Task();

		USB_EP0Task();
		
		// connect 1
		if (state == hub_ready && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			connect_port(1);
			state = p1_wait_reset;
		}
		
		if (state == p1_wait_reset && last_port_reset_clear == 1)
		{
			T();
			LED(LED_RED_IO);
			switch_port(1);
			state = p1_wait_enumerate;
		}

		// connect 2
		if (state == p1_ready && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			switch_port(0);
			connect_port(2);
			state = p2_wait_reset;
		}

		if (state == p2_wait_reset && last_port_reset_clear == 2)
		{
			T();
			LED(LED_RED_IO);
			switch_port(2);
			state = p2_wait_enumerate;
		}

		// connect 3
		if (state == p2_ready && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			switch_port(0);
			connect_port(3);
			state = p3_wait_reset;
		}

		if (state == p3_wait_reset && last_port_reset_clear == 3)
		{
			T();
			LED(LED_RED_IO);
			switch_port(3);
			state = p3_wait_enumerate;
		}

		// disconnect 2
		if (state == p3_ready && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			switch_port(0);
			disconnect_port(2);
			state = p2_wait_disconnect;
		}

		if (state == p2_wait_disconnect && last_port_conn_clear == 2)
		{
			T();
			LED(LED_RED_IO);
			state = p4_wait_connect;
			expire = 15;
		}

		// connect 4
		if (state == p4_wait_connect && expire == 0) 
		{
			T();
			LED(LED_GREEN_IO);
			connect_port(4);
			state = p4_wait_reset;
		}

		if (state == p4_wait_reset && last_port_reset_clear == 4)
		{
			T();
			LED(LED_RED_IO);
			switch_port(4);
			state = p4_wait_enumerate;
		}

		// disconnect 3
		if (state == p4_ready && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			switch_port(0);
			/* Need wrong data toggle again */
			hub_int_force_data0 = 1;
			disconnect_port(3);
			state = p3_wait_disconnect;
		}

		if (state == p3_wait_disconnect && last_port_conn_clear == 3)
		{
			T();
			LED(LED_RED_IO);
			state = p3_disconnected;
			expire = 45;
		}

		// disconnect 4
		if (state == p3_disconnected && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			switch_port(0);
			disconnect_port(4);
			state = p4_wait_disconnect;
		}

		if (state == p4_wait_disconnect && last_port_conn_clear == 4)
		{
			T();
			LED(LED_RED_IO);
			state = p4_disconnected;
			expire = 20;
		}

		// disconnect 1
		if (state == p4_disconnected && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			switch_port(0);
			disconnect_port(1);
			state = p1_wait_disconnect;
		}

		if (state == p1_wait_disconnect && last_port_conn_clear == 1)
		{
			T();
			LED(LED_RED_IO);
			if(mmc_card_plugin)
				state = done;
			else
			{
				state = p1_disconnected;
				expire = 20;
			}
		}

		// connect 6
		if (state == p1_disconnected && expire == 0)
		{
			T();
			LED(LED_GREEN_IO);
			switch_port(0);
			connect_port(6);
			state = p6_wait_reset;
		}

		if (state == p6_wait_reset && last_port_reset_clear == 6)
		{
			T();
			LED(LED_RED_IO);
			switch_port(6);
			state = p6_wait_enumerate;
		}

		// done
		if (state == done)
		{
			T();
			LED(LED_GREEN_IO);
			*pps3_boot = 1;
			LoaderTask();
		}
	}
}

//unsigned int g_stack[2049];
void usb_main(void *data)
{
	u8 byte;
	u8 *test_buf;

	__dcache_writeback_all();
	__icache_invalidate_all();

	__gpio_as_output(LED_RED_IO);
	__gpio_as_output(LED_GREEN_IO);
	LED(LED_RED_IO);

	//Check MMC card plug in
	__gpio_as_input(MMC1_CD_PIN);
	__gpio_enable_pull(MMC1_CD_PIN);
	T();
	if(__gpio_get_pin(MMC1_CD_PIN))
	{
		mmc_card_plugin = 0;
	}
	else
	{
		mmc_card_plugin = 1;
	}

	ep0state = USB_EP0_IDLE;
	Bulk_in_size = 0;
	Bulk_in_finish = 0;
	Bulk_out_size = 0;
	//udc_state = IDLE;
	tx_size = 0;
	rx_size = 0;
	finished = 0;

	disable_device(1);
	enable_device(1);

	byte=jz_readb(A_UDC_POWER);
	if ((byte&0x10)==0) 
	{
		printf("fs\n");
		fifosize[1]=8;
		//EP0_init(1,64,1,64);
	}
	else
	{
		printf("hs\n");
		fifosize[1]=512;
		//EP0_init(1,512,1,512);
	}
	fifosize[1]=8;

	serial_puts("\n Init UDC");

	JZ_StartTimerEx(0, 100, timer_handler);
	psgroove();
	
}


