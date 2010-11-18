# 
# 
# Author: Seeger Chin
# e-mail: seeger.chin@gmail.com
# 
# Copyright (C) 2006 Ingenic Semiconductor Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
# 
# 

# select which module to compile, set value to 1 for the module to compile, remain blank for the module not to compile
include ../configure.mak
TOP     := ../..
include ../config.mak
LIBNAME := libuc.a

SOURCES = string.c strtol.c strtoul.c strtod.c strtok_r.c strtok.c strerror.c swab.c dtoa.c atof.c findfp.c  fvwrite.c fwalk.c impure.c makebuf.c mbtowc_r.c  mprec.c  sprintf.c snprintf.c vfprintf.c vsprintf.c vsnprintf.c  wsetup.c fflush.c printf.c puts.c fputs.c wbuf.c putc.c putchar.c vfscanf.c sscanf.c ungetc.c refill.c getc.c rget.c lcltime.c lcltime_r.c mktime.c gmtime.c ctime.c asctime.c asctime_r.c div.c misc.c cvalue.c

OBJS	:= $(addsuffix .o , $(basename $(notdir $(SOURCES))))
include ../commondir.mak
include ../common.mak
