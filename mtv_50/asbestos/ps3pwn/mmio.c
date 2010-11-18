/*  mmio.c - common functions to do MMIO from userspace

Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>

This code is licensed to you under the terms of the GNU GPL, version 2;
see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mmio.h"

static int fd = -1;

char *mmio_map(u32 addr, u32 size)
{
	void *map;
	if (fd == -1) {
		fd = open("/dev/mem", O_RDWR|O_SYNC);
		if (fd == -1) {
			printf("Failed to open /dev/mem\n");
			exit(1);
		}
	}

	map = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, addr);
	if (map == MAP_FAILED) {
		printf("mmap %08x failed\n", addr);
		exit(1);
	}
	printf("mmapped %08x -> %p [0x%x]\n", addr, map, size);
	return map;
}
