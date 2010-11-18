################################################################
# 0 8bit nand
# 1 16bit nand
NANDBITMODE = 0
################################################################

KERNELDIR = $(SOCDIR)/kernel
DRVDIR = $(SOCDIR)/drv
COMMONDRV = $(SOCDIR)/../common/drv
INCLUDESDIR += -I$(SOCDIR)/include

ifeq ($(MMCTYPE),2)
	CFLAGS += -DTF_SD_CARD=1
else
	CFLAGS += -DTF_SD_CARD=0
endif

SOURCES += $(wildcard $(KERNELDIR)/*.c)
CFLAGS += -I$(KERNELDIR) -I$(SOCCOMMONDIR)/inc -I$(SOCDIR)/include
VPATH += $(KERNELDIR)

LIBS += $(SOCCOMMONLIBS)/kernel_lib.a
VPATH += $(SOCCOMMONLIBS)

ifneq ($(VMNAND),0)
	VMNANDDIR = $(SOCDIR)/vmnand
	include  $(VMNANDDIR)/vmnand.mk
endif

ifneq ($(NAND),0)
	ifneq ($(NAND),0xff)
		NANDDIR = $(SOCDIR)/nand_new
		include  $(NANDDIR)/nand.mk
	else
		NANDDIR = $(COMMONDRV)/../nand
		include  $(NANDDIR)/nand.mk
	endif
endif

ifneq ($(KEYTYPE),0)
	ifneq ($(KEYTYPE),0xff)
		KEYDIR = $(DRVDIR)/key
		include $(KEYDIR)/key.mk
	else
		KEYDIR = $(COMMONDRV)/key
		include $(KEYDIR)/key.mk
	endif
endif

ifneq ($(CODECTYPE),0)
	ifneq ($(CODECTYPE),0xff)
		CODECDIR = $(DRVDIR)/codec
		include $(CODECDIR)/codec.mk
	else
		CODECDIR = $(COMMONDRV)/codec
		include $(CODECDIR)/codec.mk
	endif
endif
ifneq ($(RTCTYPE),0)
	ifneq ($(RTCTYPE),0xff)
		RTCDIR = $(DRVDIR)/rtc
		include  $(RTCDIR)/rtc.mk
	else
		RTCDIR = $(COMMONDRV)/rtc
		include  $(RTCDIR)/rtc.mk
	endif
endif

ifneq ($(LCDTYPE),0)
	ifeq ($(LCDTYPE), 3)
		ifneq ($(SLCDTYPE), 0)
			SLCDDIR = $(DRVDIR)/slcd
			include $(SLCDDIR)/slcd.mk
		else
			LCDDIR = $(COMMONDRV)/lcd
			include $(LCDDIR)/lcd.mk
		endif
	else
		ifneq ($(LCDTYPE),0xff)
			LCDDIR = $(DRVDIR)/lcd
			include $(LCDDIR)/lcd.mk
		else
			LCDDIR = $(COMMONDRV)/lcd
			include $(LCDDIR)/lcd.mk
		endif
	endif
endif

ifneq ($(TOUCHTYPE),0)
	ifneq ($(TOUCHTYPE),0xff)
		TOUCHDIR = $(DRVDIR)/touch
		include $(TOUCHDIR)/touch.mk
	else
		TOUCHDIR = $(COMMONDRV)/touch
		include $(TOUCHDIR)/touch.mk
	endif
endif
ifneq ($(MMCTYPE),0)
	ifneq ($(MMCTYPE),0xff)
		MMCDIR = $(DRVDIR)/mmc
		include $(MMCDIR)/mmc.mk
	else
		MMCDIR = $(COMMONDRV)/mmc
		include $(MMCDIR)/mmc.mk
	endif
endif

ifeq ($(I2C),1)
I2CDIR = $(DRVDIR)/i2c
include $(I2CDIR)/i2c.mk
endif

ifeq ($(DVB_DRV),1)
I2CDIR = $(DRVDIR)/i2c
include $(I2CDIR)/i2c.mk
	ifeq ($(DVBTYPE),1)
	DVBDIR = $(DRVDIR)/dvb
	else
	DVBDIR = $(DRVDIR)/dvb_dibcom
	endif
include $(DVBDIR)/dvb.mk
endif

ifeq ($(CAMERA),1)
CAMERADIR = $(DRVDIR)/camera
include $(CAMERADIR)/camera.mk
endif

ifneq ($(UDC),0)
	ifneq ($(UDC),0xff)
		UDCDIR = $(DRVDIR)/udc
		include $(UDCDIR)/udc.mk
	else
		UDCDIR = $(COMMONDRV)/udc
		include $(UDCDIR)/udc.mk
	endif
endif
ifneq ($(FMTYPE),0)
	ifneq ($(FMTYPE),0xff)
		FMDIR = $(DRVDIR)/fm
		include $(FMDIR)/fm.mk
	else
		FMDIR = $(COMMONDRV)/fm
		include $(FMDIR)/fm.mk
	endif
endif

ifeq ($(NANDBITMODE),0)
	CFLAGS += -DNAND_opera_16BIT=0
else
	CFLAGS += -DNAND_opera_16BIT=1
endif

