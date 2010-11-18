SOURCES	+= no_lcdc.c nobacklight.c
CFLAGS	+= -DLCDTYPE=$(LCDTYPE)
CFLAGS	+= -I$(LCDDIR)
VPATH   += $(LCDDIR)

