# ----------------------------------------------------------------------------
#
# Copyright (C) 2009 Ingenic Semiconductor Co., Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# Just a invoke entry, and will include each .mak file if needed
#
# v1.0, 	2009-04-03,	rocky,		Init version
#
# comment: oxff - null driver, 0 - no driver	2009-09.27 danny added
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
#LCDTYPE :                                  
#           0. No LCD Driver
#           1. Samsung tft 480 * 272        
#           2. FOXCONN 8-bit serial
#           3. slcd
#           4. AUO_A030FL01_V1
#           5. AUO_TD043MGEB1 800 * 480
#           6. 320x240
#           7. FTG500C01Z,xltao add   
# --------------------------------------------------------------------------
LCDTYPE = 0
SCREEN_WIDTH  = 320
SCREEN_HEIGHT = 240
LCD16BPP    = 0
LCD32BPP    = 1

# ----------------------------------------------------------------------------
#SLCDTYPE : ???, xltao 
#           0. No SLCD Driver
#           1. SPFD5420A 
#           4. Ili9325
#           5. Ili9325_sc
#           28. LG4535
#           30. Ili9331
#			31. 3225
# ----------------------------------------------------------------------------
SLCDTYPE    = 0

# ----------------------------------------------------------------------------
# KEYTYPE:
#           0. No Key Driver
#           1. GPIO Key
#           2. SADC Key(TBD)
# ----------------------------------------------------------------------------
KEYTYPE = 0xff

# ----------------------------------------------------------------------------
# LOGO:
#          0: jz logo
#          1: glwx logo 
# ---------------------------------------------------------------------------- 
GLWX_LOGO = 1

# ----------------------------------------------------------------------------
# FMTYPE:
#           0. No FM driver
#      1. philips
#      2. RD5807
# ----------------------------------------------------------------------------
FMTYPE = 0xff


# ----------------------------------------------------------------------------
# MMCTYPE:
#			1. standed SD card
#			2. tiny SD card
# ----------------------------------------------------------------------------
MMCTYPE = 2


# ----------------------------------------------------------------------------
# UDC:
#			NONE
# ----------------------------------------------------------------------------
UCFS   = 1
UDC    = 1
NAND   = 1
VMNAND = 0

# ----------------------------------------------------------------------------
#CODECTYPE :
#           0. No Codec Driver
#           1. jz4755 Internal Codec
#           2. External Codec(TBD)
# ----------------------------------------------------------------------------
CODECTYPE = 0


# ----------------------------------------------------------------------------
#TOUCHTYPE :
#           0. No Touch driver
#           1. JZ4750 Internal SADC(TBD)
#           2. AK4182
# ----------------------------------------------------------------------------
TOUCHTYPE = 1


# ----------------------------------------------------------------------------
#RTCTYPE :
#           0. No RTC Driver
#           1. jz4755 Internal RTC
# ----------------------------------------------------------------------------
RTCTYPE    = 1


# ----------------------------------------------------------------------------
# Application Configuarion:
#           NONE
# ----------------------------------------------------------------------------
DM = 1
USE_MIDWARE = 1


# ----------------------------------------------------------------------------
# Memory Configuarion:
#           NONE
# ----------------------------------------------------------------------------
LOADCOMMONDATA := 0x807FFE00
LOADCOMMONLEN  := 0x200

FRAME_BUFF := 0x807A1000
FRAME_BUFF_LEN := 0x5EC00

FRAME_BUFF_DECS := 0x807FFC00
FRAME_BUFF_DECS_LEN := 0x200

DRVMEMADDR := 0x80122000
DRVMEMLEN  := 0x00180000

APPMEMADDR := 0x802A2000
APPMEMLEN  := 0x00167C00

################################################################

SECTIONLDS16_DIR := $(TOP)/mtv_50/sectionlds16
SECTIONLDS32_DIR := $(TOP)/mtv_50/sectionlds

SECT_LDS_ADDRESS := ./sectionlds16/address.lds
SECT_LDS_1 := $(SECTIONLDS16_DIR)/sect_1_m1.lds
SECT_LDS_2 := $(SECTIONLDS16_DIR)/sect_1_m2.lds
SECT_LDS_3 := $(SECTIONLDS16_DIR)/sect_1_m3.lds
SECT_LDS_5 := $(SECTIONLDS16_DIR)/sect_1_m5.lds
SECT_LDS_AUDIO := $(SECTIONLDS16_DIR)/sect_1_audio.lds
SECT_LDS_PHOTO := $(SECTIONLDS16_DIR)/sect_1_photo.lds
SECT_LDS_FLASH := $(SECTIONLDS16_DIR)/sect_1_swf.lds
SECT_LDS_GAME := $(SECTIONLDS16_DIR)/sect_1_game.lds
SECT_LDS_GAMELIB := $(SECTIONLDS16_DIR)/sect_1_gamelib.lds
SECT_LDS_VD_PLUG := link_vd16.xn
SECT_LDS_AD_PLUG := link_ad16.xn
################################################################


################################################################
# PLATFORM
################################################################
CFLAGS += -DPLATFORM_CETUS
CFLAGS += -DGLWX_LOGO=$(GLWX_LOGO) #hqz add
ifneq ($(LCD16BPP),0)
CFLAGS += -DLCD_TYPE_BPP_16
endif

ifneq ($(LCD32BPP),0)
CFLAGS += -DLCD_TYPE_BPP_32
endif
CFLAGS += -DGBA_DEBUG=1
#The End!
