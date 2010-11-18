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
TOP     := ../../..
LIBDIR = $(TOP)/soc_src/LIBS
SRCDIR = $(TOP)/soc_src/common/mmc_lib
OSDIR = $(TOP)/src
INCDIR = $(TOP)/soc_src/common/inc 

LIBNAME := mmc_lib.a

SOURCES = mmc.c
 
CFLAGS += -I$(SRCDIR) -I$(OSDIR) -I$(INCDIR) -I$(TOP)/inc -DRELEASE=1
VPATH = $(SRCDIR) $(OSDIR)
OBJS	:= $(addsuffix .o , $(basename $(notdir $(SOURCES))))

.SUFFIXES: .c .o .cpp

all: $(OBJS) $(LIBDIR)
	$(AR) $(LIBDIR)/$(LIBNAME) $(OBJS)
$(LIBDIR):
	-@test -d $(LIBDIR) || mkdir $(LIBDIR)	
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -fno-rtti -fvtable-gc -o $@ -c $<
.S.o:
	$(CC) $(CFLAGS) -D_ASSEMBLER_ -D__ASSEMBLY__ -o $@ -c $<
clean:
	rm -f  *~ *.o $(LIBDIR)/$(LIBNAME) depend

dep:    depend
depend:
	$(CC) -MM $(CFLAGS) $(SOURCES) 1>.depend

filelist:
	$(OBJS) $(SOURCES) $(SRCDIR)