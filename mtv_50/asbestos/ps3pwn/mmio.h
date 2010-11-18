/*  mmio.h - common functions to do MMIO from userspace

Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;

static inline void write8(char *map, u32 addr, u8 data)
{
	*(vu8*)(map + addr) = data;
}
static inline u8 read8(char *map, u32 addr)
{
	return *(vu8*)(map + addr);
}
static inline void write16(char *map, u32 addr, u16 data)
{
	*(vu16*)(map + addr) = data;
}
static inline u16 read16(char *map, u32 addr)
{
	return *(vu16*)(map + addr);
}
static inline void write32(char *map, u32 addr, u32 data)
{
	*(vu32*)(map + addr) = data;
}
static inline u32 read32(char *map, u32 addr)
{
	return *(vu32*)(map + addr);
}
static inline void set32(char *map, u32 addr, u32 data)
{
	write32(map, addr, read32(map, addr) | data);
}
static inline void clear32(char *map, u32 addr, u32 data)
{
	write32(map, addr, read32(map, addr) & (~data));
}
static inline void set16(char *map, u32 addr, u32 data)
{
	write16(map, addr, read16(map, addr) | data);
}
static inline void clear16(char *map, u32 addr, u32 data)
{
	write16(map, addr, read16(map, addr) & (~data));
}
static inline void set8(char *map, u32 addr, u32 data)
{
	write8(map, addr, read8(map, addr) | data);
}
static inline void clear8(char *map, u32 addr, u32 data)
{
	write8(map, addr, read8(map, addr) & (~data));
}

char *mmio_map(u32 addr, u32 size);

#endif