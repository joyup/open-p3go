#  ps3pwn Makefile
#
# Copyright (C) 2010  Hector Martin "marcan" <hector@marcansoft.com>
#
# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

all: ps3pwn

ps3pwn: ps3pwn.o musb.o mmio.o

ps3pwn.o: descriptors.h

%.o: %.c
	gcc -Wall -c -o $@ $<

clean:
	rm -f *.o ps3pwn