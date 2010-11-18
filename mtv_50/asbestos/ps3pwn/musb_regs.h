/*  ps3pwn.c - implementation of the PSJailbreak exploit for OMAP/MUSB devices

Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.tx

Portions taken from Linux (drivers/usb/musb/musb_regs.h):
 * MUSB OTG driver register defines
 * 
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
*/

#ifndef MUSB_REGS_H
#define MUSB_REGS_H

#define MUSB_BASE 0x480ab000
//#define MUSB_BASE 0x480ac000

/* POWER */
#define MUSB_POWER_ISOUPDATE	0x80
#define MUSB_POWER_SOFTCONN		0x40
#define MUSB_POWER_HSENAB		0x20
#define MUSB_POWER_HSMODE		0x10
#define MUSB_POWER_RESET		0x08
#define MUSB_POWER_RESUME		0x04
#define MUSB_POWER_SUSPENDM		0x02
#define MUSB_POWER_ENSUSPEND	0x01


#define MUSB_FADDR			0x00	/* 8-bit */
#define MUSB_POWER			0x01	/* 8-bit */
#define MUSB_INTRTX			0x02	/* 16-bit */
#define MUSB_INTRRX			0x04
#define MUSB_INTRTXE		0x06
#define MUSB_INTRRXE		0x08
#define MUSB_INTRUSB		0x0A	/* 8 bit */
#define MUSB_INTRUSBE		0x0B	/* 8 bit */
#define MUSB_FRAME			0x0C
#define MUSB_INDEX			0x0E	/* 8 bit */
#define MUSB_TESTMODE		0x0F	/* 8 bit */

#define MUSB_INTRTX			0x02	/* 16-bit */
#define MUSB_INTRRX			0x04
#define MUSB_INTRTXE		0x06
#define MUSB_INTRRXE		0x08
#define MUSB_INTRUSB		0x0A	/* 8 bit */
#define MUSB_INTRUSBE		0x0B	/* 8 bit */
#define MUSB_FRAME			0x0C
#define MUSB_INDEX			0x0E	/* 8 bit */
#define MUSB_TESTMODE		0x0F	/* 8 bit */

#define OTG_REVISION	0x400
#define OTG_SYSCONFIG	0x404
#define OTG_SYSSTATUS	0x408
#define OTG_INTERFSEL	0x40c
#define OTG_SIMENABLE	0x410
#define OTG_FORCESTDBY	0x414

/* DEVCTL */
#define MUSB_DEVCTL_BDEVICE	0x80
#define MUSB_DEVCTL_FSDEV	0x40
#define MUSB_DEVCTL_LSDEV	0x20
#define MUSB_DEVCTL_VBUS	0x18
#define MUSB_DEVCTL_VBUS_SHIFT	3
#define MUSB_DEVCTL_HM		0x04
#define MUSB_DEVCTL_HR		0x02
#define MUSB_DEVCTL_SESSION	0x01

#define MUSB_DEVCTL			0x60	/* 8 bit */

#define MUSB_TXMAXP			0x00
#define MUSB_TXCSR			0x02
#define MUSB_CSR0			MUSB_TXCSR	/* Re-used for EP0 */
#define MUSB_RXMAXP			0x04
#define MUSB_RXCSR			0x06
#define MUSB_RXCOUNT		0x08
#define MUSB_COUNT0			MUSB_RXCOUNT	/* Re-used for EP0 */
#define MUSB_TXTYPE			0x0A
#define MUSB_TYPE0			MUSB_TXTYPE	/* Re-used for EP0 */
#define MUSB_TXINTERVAL		0x0B
#define MUSB_NAKLIMIT0		MUSB_TXINTERVAL	/* Re-used for EP0 */
#define MUSB_RXTYPE			0x0C
#define MUSB_RXINTERVAL		0x0D
#define MUSB_FIFOSIZE		0x0F
#define MUSB_CONFIGDATA		MUSB_FIFOSIZE	/* Re-used for EP0 */

#define MUSB_FIFO(n) (0x20 + 4*(n))

#define MUSB_CSR0_FLUSHFIFO	0x0100
#define MUSB_CSR0_TXPKTRDY	0x0002
#define MUSB_CSR0_RXPKTRDY	0x0001

/* CSR0 in Peripheral mode */
#define MUSB_CSR0_P_SVDSETUPEND	0x0080
#define MUSB_CSR0_P_SVDRXPKTRDY	0x0040
#define MUSB_CSR0_P_SENDSTALL	0x0020
#define MUSB_CSR0_P_SETUPEND	0x0010
#define MUSB_CSR0_P_DATAEND		0x0008
#define MUSB_CSR0_P_SENTSTALL	0x0004

/* TXCSR in Peripheral and Host mode */
#define MUSB_TXCSR_AUTOSET			0x8000
#define MUSB_TXCSR_DMAENAB			0x1000
#define MUSB_TXCSR_FRCDATATOG		0x0800
#define MUSB_TXCSR_DMAMODE			0x0400
#define MUSB_TXCSR_CLRDATATOG		0x0040
#define MUSB_TXCSR_FLUSHFIFO		0x0008
#define MUSB_TXCSR_FIFONOTEMPTY		0x0002
#define MUSB_TXCSR_TXPKTRDY			0x0001

/* TXCSR in Peripheral mode */
#define MUSB_TXCSR_P_ISO			0x4000
#define MUSB_TXCSR_P_INCOMPTX		0x0080
#define MUSB_TXCSR_P_SENTSTALL		0x0020
#define MUSB_TXCSR_P_SENDSTALL		0x0010
#define MUSB_TXCSR_P_UNDERRUN		0x0004

/* RXCSR in Peripheral and Host mode */
#define MUSB_RXCSR_AUTOCLEAR		0x8000
#define MUSB_RXCSR_DMAENAB			0x2000
#define MUSB_RXCSR_DISNYET			0x1000
#define MUSB_RXCSR_PID_ERR			0x1000
#define MUSB_RXCSR_DMAMODE			0x0800
#define MUSB_RXCSR_INCOMPRX			0x0100
#define MUSB_RXCSR_CLRDATATOG		0x0080
#define MUSB_RXCSR_FLUSHFIFO		0x0010
#define MUSB_RXCSR_DATAERROR		0x0008
#define MUSB_RXCSR_FIFOFULL			0x0002
#define MUSB_RXCSR_RXPKTRDY			0x0001

/* RXCSR in Peripheral mode */
#define MUSB_RXCSR_P_ISO			0x4000
#define MUSB_RXCSR_P_SENTSTALL		0x0040
#define MUSB_RXCSR_P_SENDSTALL		0x0020
#define MUSB_RXCSR_P_OVERRUN		0x0004

#define MUSB_TXFIFOSZ		0x62	/* 8-bit (see masks) */
#define MUSB_RXFIFOSZ		0x63	/* 8-bit (see masks) */
#define MUSB_TXFIFOADD		0x64	/* 16-bit offset shifted right 3 */
#define MUSB_RXFIFOADD		0x66	/* 16-bit offset shifted right 3 */

#endif