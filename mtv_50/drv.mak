include $(MOBILE_TVDIR)/bspconfig.mak
OSDIR    :=$(TOP)/src
FSDIR    := $(TOP)/jzfs

SOCDIR = $(TOP)/soc_src/jz4740

ifeq ($(JZ4750_BSP),1)
SOCDIR = $(TOP)/soc_src/jz4750
endif

ifeq ($(JZ4755_BSP),1)
SOCDIR = $(TOP)/soc_src/jz4755
endif

ifeq ($(JZ4725_BSP),1)
SOCDIR = $(TOP)/soc_src/jz4725
endif

SOCCOMMONDIR = $(TOP)/soc_src/common
SOCCOMMONLIBS = $(TOP)/soc_src/LIBS
DRVDIR = $(SOCDIR)/drv
MINICLIB := $(TOP)/miniclib
MIDWAREDIR := $(TOP)/midware
INCLUDESDIR :=

DRVOBJ := 
OEMREALSE := 
LIBS := 
ASMS :=

OSLIBS := $(FSDIR)/jzfs.a $(OSDIR)/minios.a 
STDCLIBS = -lm -lgcc

CFLAGS += -DDRVMEMADDR=$(DRVMEMADDR) -DDRVMEMLEN=$(DRVMEMLEN)
CFLAGS += -DAPPMEMADDR=$(APPMEMADDR) -DAPPMEMLEN=$(APPMEMLEN)
CFLAGS += -DFRAME_BUFF_ADDRESS=$(FRAME_BUFF) -DFRAME_BUFF_LEN=$(FRAME_BUFF_LEN) 
CFLAGS += -DLOAD_DATA_ADDRESS=$(LOADCOMMONDATA) -DLOADCOMMONLEN=$(LOADCOMMONLEN)
CFLAGS += -DFRAME_BUFF_DECS=$(FRAME_BUFF_DECS) -DFRAME_BUFF_DECS_LEN=$(FRAME_BUFF_DECS_LEN)

ifeq ($(JZ4750_BSP),1)
include $(SOCDIR)/jz4750.mak
else
ifeq ($(JZ4755_BSP),1)
include $(SOCDIR)/jz4755.mak
else
ifeq ($(JZ4725_BSP),1)
include $(SOCDIR)/jz4725.mak
else
include $(SOCDIR)/jz4740.mak
endif
endif
endif

CFLAGS += -I$(OSDIR) -I$(FSDIR)
CFLAGS += -DDM=$(DM)
ifeq ($(USE_MIDWARE),1)
include $(MIDWAREDIR)/midware.mk
CFLAGS	+= -DUSE_MIDWARE=$(USE_MIDWARE)
endif

SOURCES	+= $(wildcard $(SYSBINMNGDIR)/*.c)
CFLAGS  += -I$(SYSBINMNGDIR)
VPATH   += $(SYSBINMNGDIR)

SOURCES	+= $(wildcard $(MINICLIB)/*.c)
CFLAGS  += -I$(MINICLIB)
VPATH   += $(MINICLIB)

HEADS   := $(TOP)/soc_src/common/kernel_lib/head.S $(wildcard $(MINICLIB)/*.S)
VPATH   += $(TOP)/soc_src/common/kernel_lib
