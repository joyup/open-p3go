################################################################
#			0. operate nand by dma
#			1. operate nand by cpu
NANDMODE = 0
################################################################

ifeq ($(NANDMODE),0)
SOURCES	+= $(NANDDIR)/nandchipdma.c \
				$(NANDDIR)/jz_nand.c \
				 $(NANDDIR)/lb_nand.c
endif

ifeq ($(NANDMODE),1)
SOURCES	+= $(NANDDIR)/nandchipcpu.c \
				$(NANDDIR)/jz_nand.c \
				 $(NANDDIR)/lb_nand.c
endif
					 
DRVOBJ += $(NANDDIR)/ssfdc.o
OEMREALSE += $(NANDDIR)/ssfdc.c
CFLAGS += -I$(NANDDIR)
CFLAGS += -DNAND=$(NAND)
VPATH  += $(NANDDIR)
