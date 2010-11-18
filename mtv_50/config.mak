CC	:= mipsel-linux-gcc
CXX	:= mipsel-linux-g++

AR	:= mipsel-linux-ar  rcsv
LD	:= mipsel-linux-ld
OBJCOPY	:= mipsel-linux-objcopy
NM	:= mipsel-linux-nm
OBJDUMP	:= mipsel-linux-objdump

CFLAGS	:= -mips32 -O4 -mno-abicalls -fno-pic -fno-builtin -Werror\
	   -fno-exceptions -ffunction-sections \
	   -fomit-frame-pointer -msoft-float -fshort-wchar -G 0
CXXFLAGS := -mips32 -O4 -mno-abicalls -fno-pic -fno-builtin -Werror\
           -ffunction-sections -finit-priority \
	   -fomit-frame-pointer -msoft-float -G 0 -nostdinc++
#	   -fno-exceptions -ffunction-sections -finit-priority \


LIBS    := -lstdc++ -lc -lm -lgcc
MOBILE_TVDIR := $(TOP)/mtv_50
OSDIR   := $(TOP)/mips
JZOSDIR := $(TOP)/src
FSDIR   := $(TOP)/jzfs
SOCDIR  := $(TOP)/jz4740
COMMONDIR  := $(MOBILE_TVDIR)/common
LIBDIR	   := $(MOBILE_TVDIR)/LIBS
MIDWAREDIR := $(TOP)/midware
LCDDIR := $(SOCDIR)/drv/lcd
SLCDDIR := $(SOCDIR)/drv/slcd

CFLAGS	+= -I$(FSDIR)
CFLAGS	+= -I$(COMMONDIR)
CFLAGS	+= -I$(JZOSDIR)
CFLAGS	+= -I$(OSDIR)
CFLAGS	+= -I$(MIDWAREDIR)
CFLAGS  += -I$(SOCDIR)/include
CFLAGS  += -I$(SOCDIR)/drv/key
CFLAGS   += -I$(LCDDIR)
CFLAGS   += -I$(SLCDDIR)

