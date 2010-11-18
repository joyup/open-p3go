SOURCES	+= $(NANDDIR)/jz_no_nand.c $(NANDDIR)/lb_nand.c
DRVOBJ += $(NANDDIR)/ssfdc.o
OEMREALSE += $(NANDDIR)/ssfdc.c
CFLAGS += -I$(NANDDIR)
CFLAGS += -DNAND=$(NAND)
VPATH  += $(NANDDIR)

