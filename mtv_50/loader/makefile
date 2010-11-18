# ----------------------------------------------------------------------------
#
# Copyright (C) 2009 Ingenic Semiconductor Co., Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# Just a invoke entry, and will include each .mak file if needed
#
# v1.0, 	2009-03-21,	Rocky,		Init version
#
# ----------------------------------------------------------------------------

TOP	:= ../..
PROGRAM := homebrew
include ../config16.mak
include ../configure.mak
include ../bspconfig.mak
include ./configure.mak

MODULE_GENLDS := 0
MOBILE_TVDIR := $(TOP)/mtv_50
LOADER_DIR = $(MOBILE_TVDIR)/loader
TARGETS := $(LOADER_DIR)/target

OSDIR    :=$(TOP)/src
SOCDIR = $(TOP)/soc_src/jz$(SOC_TYPE)
SOCCOMMONDIR = $(TOP)/soc_src/common
SOCCOMMONLIBS = $(TOP)/soc_src/LIBS
DRVDIR = $(SOCDIR)/drv
MINICLIB := $(TOP)/miniclib
INCLUDESDIR :=

DRVOBJ :=
OEMREALSE :=
LIBS :=
ASMS :=
OSLIBS := $(OSDIR)/minios.a
STDCLIBS = -lm -lgcc
MAINLDS := $(LOADER_DIR)/link.xn

CFLAGS += -DDRVMEMADDR=$(DRVMEMADDR) -DDRVMEMLEN=$(DRVMEMLEN)
CFLAGS += -DAPPMEMADDR=$(APPMEMADDR) -DAPPMEMLEN=$(APPMEMLEN)

CFLAGS +=-DFRAME_BUFF_ADDRESS=$(FRAME_BUFF)
CFLAGS  +=-DLOAD_DATA_ADDRESS=$(LOADCOMMONDATA)
CFLAGS += -DRELEASE=$(RELEASE) -DJZ4740_PMP16=$(JZ4740_PMP16)
CFLAGS  +=-DDM=1
#xltao add blow line
CFLAGS  +=-DXLTAO_MAKE_LOADER

include $(SOCDIR)/jz$(SOC_TYPE).mak

SOURCES	+= $(SOCDIR)/nand/jz_nand.c \
				$(SOCDIR)/nand/nandchipdma.c

CFLAGS += -I$(OSDIR) -I$(SOCDIR)/nand
VPATH += $(OSDIR) $(SOCDIR)/nand

SOURCES	+= $(wildcard $(MINICLIB)/*.c)
CFLAGS  += -I$(MINICLIB)
VPATH   += $(MINICLIB)

HEADS   := $(TOP)/soc_src/common/kernel_lib/head.S $(wildcard $(MINICLIB)/*.S)
VPATH   += $(TOP)/soc_src/common/kernel_lib


SOURCES	+= $(wildcard $(LOADER_DIR)/*.c)
CFLAGS	+= -I$(LOADER_DIR)
VPATH	+= $(LOADER_DIR)

include ../commondir.mak
include ../common16.mak

$(USER):
	cp $(TARGETS)/$(PROGRAM).bin ../../release
	cd $(TOP)/release; ./update_file.exe

build:all $(USER)

cleanall: clean

rebuild:clean build

