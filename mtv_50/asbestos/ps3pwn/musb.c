/*  musb.h - TI OMAP USB OTG driver

Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#include <stdio.h>
#include <stdlib.h>

#include "mmio.h"
#include "musb.h"
#include "usb.h"
#include "musb_regs.h"

char *musb_map;

#define musb_read8(x) read8(musb_map,x)
#define musb_write8(x,y) write8(musb_map,x,y)
#define musb_read16(x) read16(musb_map,x)
#define musb_write16(x,y) write16(musb_map,x,y)
#define musb_read32(x) read32(musb_map,x)
#define musb_write32(x,y) write32(musb_map,x,y)
#define musb_set8(x,y) set8(musb_map,x,y)
#define musb_clear8(x,y) clear8(musb_map,x,y)
#define musb_set16(x,y) set16(musb_map,x,y)
#define musb_clear16(x,y) clear16(musb_map,x,y)
#define musb_set32(x,y) set32(musb_map,x,y)
#define musb_clear32(x,y) clear32(musb_map,x,y)

u8 musb_iread8(int ep, u32 offset)
{
	return musb_read8(ep*0x10 + 0x100 + offset);
}
void musb_iwrite8(int ep, u32 offset, u8 data)
{
	musb_write8(ep*0x10 + 0x100 + offset, data);
}
u16 musb_iread16(int ep, u32 offset)
{
	return musb_read16(ep*0x10 + 0x100 + offset);
}
void musb_iwrite16(int ep, u32 offset, u16 data)
{
	musb_write16(ep*0x10 + 0x100 + offset, data);
}
void musb_iset16(int ep, u32 offset, u16 data)
{
	musb_set16(ep*0x10 + 0x100 + offset, data);
}
void musb_iclear16(int ep, u32 offset, u16 data)
{
	musb_clear16(ep*0x10 + 0x100 + offset, data);
}

int get_vbus(void)
{
	return (musb_read8(MUSB_DEVCTL)&MUSB_DEVCTL_VBUS)>>MUSB_DEVCTL_VBUS_SHIFT;
}

void periph_reset(void)
{
	if (musb_read8(MUSB_POWER)&MUSB_POWER_HSMODE)
		printf("High-speed reset\n");
	else
		printf("Full-speed reset\n");
	cb_reset();
}

int need_to_ack_rx = 0;
u8 pending_addr = 0;
int addr_countdown = 0;

void write_fifo0(void *data, int len)
{
	u8 *p = data;
	//printf("write_fifo0(%d)\n", len);
	if (need_to_ack_rx) {
		musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_SVDRXPKTRDY);
		need_to_ack_rx = 0;
	}
	// clear irqs
	musb_read16(MUSB_INTRTX);
	int blks = 0;
	while(len) {
		int blen = len;
		if (blen > EP0_PKTSIZE)
			blen = EP0_PKTSIZE;
		int left = blen;
		while(left--)
			musb_write8(MUSB_FIFO(0), *p++);
		//printf("write %d bytes\n", blen);
		len -= blen;
		blks++;
		if (len) {
			musb_iset16(0, MUSB_CSR0, MUSB_CSR0_TXPKTRDY);
			while (!(musb_read16(MUSB_INTRTX) & 0x0001));
			//while (musb_read16(MUSB_CSR0) & MUSB_CSR0_TXPKTRDY);
		} else {
			musb_iset16(0, MUSB_CSR0, MUSB_CSR0_TXPKTRDY | MUSB_CSR0_P_DATAEND);
			//printf("Wrote %d blocks\n", blks);
		}
	}
}

int read_fifo0(void *data, int len)
{
	u8 *p = data;
	//printf("write_fifo0(%d)\n", len);
	if (need_to_ack_rx) {
		musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_SVDRXPKTRDY);
		need_to_ack_rx = 0;
	}
	// clear irqs
	musb_read16(MUSB_INTRRX);
	int read = 0;
	int i;
	while(len) {
		while (!(musb_iread16(0, MUSB_CSR0) & MUSB_CSR0_RXPKTRDY));
		int blen = musb_iread8(0,MUSB_COUNT0);
		if (blen == 0)
			break;
		if (blen > len) {
			printf("lolwut %d > %d\n", blen, blen);
			exit(1);
		}
		for(i=0; i<blen; i++) {
			*p++ = musb_read8(MUSB_FIFO(0));
		}
		len -= blen;
		read += blen;
		if (len) {
			musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_SVDRXPKTRDY);
		} else {
			musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_SVDRXPKTRDY | MUSB_CSR0_P_DATAEND);
		}
	}
	return read;
}

void ep0_stall(void)
{
	printf("STALL EP0\n");
	if (need_to_ack_rx)
		musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_SENDSTALL | MUSB_CSR0_P_SVDRXPKTRDY);
	else
		musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_SENDSTALL);
	need_to_ack_rx = 0;
}

void ep0_nodata(void)
{
	if (need_to_ack_rx)
		musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_DATAEND | MUSB_CSR0_P_SVDRXPKTRDY);
	else
		musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_DATAEND);
	while(musb_iread16(0, MUSB_CSR0) & MUSB_CSR0_P_DATAEND);
}

void write_fifo(int ep, void *data, int len)
{
	u8 *p = data;
	int blk;
	//printf("write_fifo(%d,%d)\n", ep, len);
	while(len) {
		blk = 0;
		while(len) {
			musb_write8(MUSB_FIFO(ep), *p++);
			blk++;
			len--;
			if (ep == 1 && blk >= EP1_PKTSIZE)
				break;
			if (ep == 2 && blk >= EP2_PKTSIZE)
				break;
		}
		fflush(stdout);
		musb_iset16(ep, MUSB_TXCSR, MUSB_TXCSR_TXPKTRDY);
		while (musb_iread16(ep, MUSB_TXCSR) & MUSB_TXCSR_TXPKTRDY);
	}
}

void read_fifo(int ep, void *data, int len)
{
	u8 *p = data;
	//printf("read_fifo(%d,%d)\n", ep, len);
	while(len--)
		*p++ = musb_read8(MUSB_FIFO(ep));
	musb_iclear16(ep, MUSB_RXCSR, MUSB_RXCSR_RXPKTRDY);
}

void usb_set_address(u8 addr, int delay)
{
	if (!delay) {
		musb_write8(MUSB_FADDR, addr);
	} else {
		pending_addr = addr;
		addr_countdown = delay - 1;
	}
}

void reset_ep_datatog(int ep)
{
	musb_iset16(ep, MUSB_TXCSR, MUSB_TXCSR_CLRDATATOG);
}

void ctl_xfer(int length)
{
	u8 buffer[64];
	int i;
	//printf("CSR0 = %04x\n",musb_iread16(0, MUSB_CSR0));
	if ((musb_iread16(0, MUSB_CSR0) & MUSB_CSR0_P_SETUPEND)) {
		printf("Got a SETUPEND condition\n");
		musb_iset16(0, MUSB_CSR0, MUSB_CSR0_P_SVDSETUPEND);
	}
	if ((musb_iread16(0, MUSB_CSR0) & MUSB_CSR0_P_SENTSTALL)) {
		printf("Sent a stall!\n");
		musb_iclear16(0, MUSB_CSR0, MUSB_CSR0_P_SENTSTALL);
	}
	if (!(musb_iread16(0, MUSB_CSR0) & MUSB_CSR0_RXPKTRDY)) {
		if (pending_addr) {
			if (addr_countdown) {
				addr_countdown--;
			} else {
				musb_write8(MUSB_FADDR, pending_addr);
				printf("Commit pending address %d\n", pending_addr);
				pending_addr=0;
			}
		}
		return;
	}
	if (length != 8) {
		if (length == 0) {
			printf("length 0?\n");
			return;
		}
		printf("lolwut %d\n", length);
		exit(1);
	}
	for(i=0; i<length; i++) {
		buffer[i] = musb_read8(MUSB_FIFO(0));
	}
	need_to_ack_rx = 1;
	u8 bmRequestType = buffer[0];
	u8 bRequest = buffer[1];
	u16 wValue = buffer[2] | (buffer[3]<<8);
	u16 wIndex = buffer[4] | (buffer[5]<<8);
	u16 wLength = buffer[6] | (buffer[7]<<8);
	if (bmRequestType == 0x80 && bRequest == GET_DESCRIPTOR) {
		cb_get_descriptor(wValue, wIndex, wLength);
	} else if (bmRequestType == 0x00 && bRequest == SET_ADDRESS) {
		cb_set_address(wValue);
		ep0_nodata();
	} else if (bmRequestType == 0x00 && bRequest == SET_CONFIGURATION) {
		printf("Set configuration %d\n", wValue);
		// just ack it
		ep0_nodata();
		cb_set_configuration(wValue);
	} else if (bmRequestType == 0x01 && bRequest == SET_INTERFACE) {
		printf("Set interface %d\n", wValue);
		// just ack it
		ep0_nodata();
	} else if (bmRequestType == 0x00 && bRequest == SET_FEATURE) {
		printf("Set feature\n");
		// just ack it
		ep0_nodata();
	} else if (bmRequestType == 0x80 && bRequest == GET_STATUS) {
		printf("Get status\n");
		// just return whatever
		u8 status[] = {0,0};
		write_fifo0(status, 2);
	} else if (cb_handle_ctl(bmRequestType, bRequest, wValue, wIndex, wLength)) {
		// nothing, already done
	} else {
		printf("Unknown control xfer: type %02x req %02x val %04x idx %04x len %04x\n", bmRequestType, bRequest, wValue, wIndex, wLength);
		ep0_stall();
	}
}

void tx_irq(u8 irqs)
{
	if (irqs & 1) {
		//printf("EP0 IRQ\n");
		u8 count0 = musb_iread8(0,MUSB_COUNT0);
		ctl_xfer(count0);
	}
}

u8 in_buffer[0x1000];

void usb_connect(void)
{
	printf("Now soft-connecting\n");
	musb_set8(MUSB_POWER, MUSB_POWER_SOFTCONN);
}

void usb_process(void)
{
	u8 irqs = musb_read8(MUSB_INTRUSB);
	if (irqs) {
		if (irqs != 0x08)
			printf("IRQs: %02x\n", irqs);
		if (irqs & 0x04)
			periph_reset();
		if (irqs & 0x08)
			cb_sof();
	}
	irqs = musb_read16(MUSB_INTRTX);
	if (irqs) {
		//printf("TX IRQs: %02x\n", irqs);
		tx_irq(irqs);
	}
	irqs = musb_read16(MUSB_INTRRX);
	if (irqs) {
		//printf("RX IRQs: %02x\n", irqs);
		int i;
		for (i=1; i<16; i++) {
			if (irqs & (1<<i)) {
				int len = musb_iread16(i, MUSB_RXCOUNT);
				read_fifo(i, in_buffer, len);
				cb_data_in(i, in_buffer, len);
			}
		}
	}
}

void musb_init(void)
{
	musb_map = mmio_map(MUSB_BASE, 0x1000);

	printf("Revision: %08x\n", musb_read32(0x400));

	printf("Resetting OTG controller\n");
	musb_write16(MUSB_INTRRXE, 0);
	musb_write16(MUSB_INTRTXE, 0);
	musb_write8(MUSB_INTRUSBE, 0);
	musb_set32(OTG_SYSCONFIG, 0x02);
	while(!(musb_read32(OTG_SYSSTATUS)&1));
	musb_write16(MUSB_INTRRXE, 0);
	musb_write16(MUSB_INTRTXE, 0);
	musb_write8(MUSB_INTRUSBE, 0);
	printf("Reset complete\n");
	printf("Devctl: %08x\n", musb_read8(MUSB_DEVCTL));

	musb_clear8(MUSB_POWER, MUSB_POWER_HSENAB);
	printf("High-speed mode disabled\n");

	u32 fifoadd = 64;

	printf("Configuring endpoints\n");
	musb_write8(MUSB_INDEX, 1);
	musb_write8(MUSB_TXFIFOSZ, EP1_LOG_PKTSIZE - 3);
	musb_write16(MUSB_TXFIFOADD, fifoadd);
	fifoadd += EP1_PKTSIZE;
	musb_write8(MUSB_RXFIFOSZ, EP1_LOG_PKTSIZE - 3);
	musb_write16(MUSB_TXFIFOADD, fifoadd);
	fifoadd += EP1_PKTSIZE;
	musb_iwrite16(1, MUSB_TXMAXP, EP1_PKTSIZE);
	musb_iwrite16(1, MUSB_RXMAXP, EP1_PKTSIZE);
	musb_iwrite16(1, MUSB_TXCSR, MUSB_TXCSR_CLRDATATOG|MUSB_TXCSR_FLUSHFIFO);
	musb_iwrite16(1, MUSB_RXCSR, MUSB_RXCSR_CLRDATATOG|MUSB_RXCSR_FLUSHFIFO);
	musb_iwrite16(1, MUSB_TXCSR, 0);
	musb_iwrite16(1, MUSB_RXCSR, 0);

	musb_write8(MUSB_INDEX, 2);
	musb_write8(MUSB_TXFIFOSZ, EP2_LOG_PKTSIZE - 3);
	musb_write16(MUSB_TXFIFOADD, fifoadd);
	fifoadd += EP2_PKTSIZE;
	musb_write8(MUSB_RXFIFOSZ, EP2_LOG_PKTSIZE - 3);
	musb_write16(MUSB_TXFIFOADD, fifoadd);
	fifoadd += EP2_PKTSIZE;
	musb_iwrite16(2, MUSB_TXMAXP, EP2_PKTSIZE);
	musb_iwrite16(2, MUSB_RXMAXP, EP2_PKTSIZE);
	musb_iwrite16(2, MUSB_TXCSR, MUSB_TXCSR_CLRDATATOG|MUSB_TXCSR_FLUSHFIFO);
	musb_iwrite16(2, MUSB_RXCSR, MUSB_RXCSR_CLRDATATOG|MUSB_RXCSR_FLUSHFIFO);
	musb_iwrite16(1, MUSB_TXCSR, 0);
	musb_iwrite16(1, MUSB_RXCSR, 0);

	musb_clear8(MUSB_POWER, MUSB_POWER_SOFTCONN);
}

void musb_reset(void)
{
	pending_addr = 0;
}