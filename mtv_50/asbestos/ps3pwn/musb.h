/*  musb.h - TI OMAP USB OTG driver

Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifndef MUSB_H
#define MUSB_H

#define EP0_PKTSIZE 8

#define EP1_LOG_PKTSIZE 9
#define EP1_PKTSIZE (1<<EP1_LOG_PKTSIZE)

#define EP2_LOG_PKTSIZE 9
#define EP2_PKTSIZE (1<<EP2_LOG_PKTSIZE)

int get_vbus(void);
void usb_set_address(u8 addr, int delay);
void reset_ep_datatog(int ep);

void write_fifo0(void *data, int len);
int read_fifo0(void *data, int len);
void ep0_stall(void);
void ep0_nodata(void);
void write_fifo(int ep, void *data, int len);
void read_fifo(int ep, void *data, int len);

void usb_connect(void);
void usb_process(void);
void musb_init(void);
void musb_reset(void);

void cb_reset(void);
void cb_data_in(int endpoint, u8 *data, int size);
void cb_set_address(u8 addr);
void cb_get_descriptor(u16 wValue, u16 wIndex, u16 wLength);
int cb_handle_ctl(u8 bmRequestType, u8 bRequest, u16 wValue, u16 wIndex, u16 wLength);
void cb_set_configuration(int idx);
void cb_sof(void);

#endif
