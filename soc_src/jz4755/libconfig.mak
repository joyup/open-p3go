include ./tools.mak
TOP     := ../../..
LIBDIR = $(TOP)/soc_src/LIBS
SOCDIR = $(TOP)/soc_src/jz4740
OSDIR = $(TOP)/src
LIBNAME := 
INCDIR = $(TOP)/soc_src/common/inc 
CFLAGS += -I$(SOCDIR) -I$(OSDIR) -I$(INCDIR) -DRELEASE=$(RELEASE)
SOURCES := 
VPATH = $(INCDIR) $(OSDIR) $(SOCDIR)
