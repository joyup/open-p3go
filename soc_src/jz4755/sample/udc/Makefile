
TOP     := ../../../..
SOCCOMMONDIR = $(TOP)/soc_src/common
SOCCOMMONLIBS = $(TOP)/soc_src/LIBS
LIBDIR = $(TOP)/soc_src/LIBS
SOCDIR = $(TOP)/soc_src/jz4755
DRVDIR = $(SOCDIR)/drv
OSDIR = $(TOP)/src
PROGRAM = minios
MINICLIB := $(TOP)/miniclib


include $(SOCDIR)/tools.mak
include ./config.mak

#include $(TOP)/mtv_50/configure.mak
#include $(TOP)/mtv_50/bspconfig.mak

INCDIR = $(TOP)/soc_src/common/inc
CFLAGS += -I$(SOCDIR) -I$(OSDIR) -I$(INCDIR) -DRELEASE=$(RELEASE)\
					-I$(SOCDIR)/include -I$(TOP)/inc -DDM=1 -DPLATFORM_CETUS

SOURCES := main.c


VPATH = $(INCDIR) $(OSDIR) $(SOCDIR)
include $(SOCDIR)/jz4755.mak
SOURCES	+= $(wildcard $(MINICLIB)/*.c)
CFLAGS  += -I$(MINICLIB)
VPATH   += $(MINICLIB)


HEADS := head.S $(wildcard $(MINICLIB)/*.S)
VPATH += $(SOCCOMMONDIR)/kernel_lib
HEADO	:= $(addsuffix .o ,$(basename $(notdir $(HEADS))))
OBJS := $(addsuffix .o ,$(basename $(notdir $(SOURCES))))

OSLIBS := $(OSDIR)/minios.a 

APP	:= $(PROGRAM).elf
.SUFFIXES: .c .o .cpp
all:	$(APP)
	$(OBJCOPY) -O binary $(APP) $(PROGRAM).bin
	cp $(PROGRAM).bin /tftpboot/$(USER)
$(APP):$(HEADO) $(OBJS) $(OSLIBS) $(LIBS) $(EXTRAOBJ) $(DRVOBJ) Makefile link.xn
	$(LD) -r -o $@.obj  $(HEADO) $(OBJS) $(DRVOBJ) $(EXTRAOBJ) $(LIBS) $(OSLIBS)
	$(CC) --static -nostdlib -T link.xn -o $@ $@.obj 
$(DRVOBJ):%o:%c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OSLIBS):
	cd $(TOP)/os_src;make
$(LIBS):
	cd $(SOCCOMMONDIR)/kernel_lib;make
	cd $(SOCCOMMONDIR)/mass_lib;make
	cd $(SOCCOMMONDIR)/mmc_lib;make
		
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
.cpp.o:
	$(CXX) $(CXXFLAGS) -fno-rtti -o $@ -c $<
.S.o:
	$(CC) $(CFLAGS) -D_ASSEMBLER_ -D__ASSEMBLY__ -o $@ -c $<	
dump: $(TARGETS) $(APP)
	$(OBJCOPY) -O binary $(APP) $(PROGRAM).bin
	$(OBJDUMP) -d $(APP) > $(PROGRAM).dump
	$(NM) $(APP) | sort > $(PROGRAM).sym 
	$(OBJDUMP) -h $(APP) > $(PROGRAM).map
clean:
	rm *.o *.elf *.bin *.obj -f *.dump *.sym *.map
ls:
	echo $(SOURCES)
	
