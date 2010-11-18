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
           -ffunction-sections  \
	   -fomit-frame-pointer -msoft-float -G 0 -nostdinc++
LIBS    := 

SOURCE :=
VPATH := 
OBJS := 
LDSFLAG := link.xn
GENLDS := $(TOP)/mtv_50/genlds 
LDSFILES := $(TOP)/mtv_50/target/mobile_tv.lds
MODULE_GENLDS := 0
MAINLDS := $(TOP)/mtv_50/target/mobile_tv.elf
