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
# v1.0, 	2009-06-03,	Rocky,		Init version
#
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Chip Ship:
#	0. Debugging
#	1. Chip Shipping
# ----------------------------------------------------------------------------
RELEASE := 1

# ----------------------------------------------------------------------------
# Ingenic SOC Generation
#	* 4725
#	* 4740
#	* 4750
#	* 4755
# ----------------------------------------------------------------------------
SOC_GENERATION := 4755


# ----------------------------------------------------------------------------
# Ingenic Evaluate Board
# ----------------------------------------------------------------------------
ifeq ($(SOC_GENERATION),4725)
JZ4725_BSP := 1
JZ4725_CETUS := 1
endif

ifeq ($(SOC_GENERATION),4755)
JZ4755_BSP := 1
JZ4755_CETUS := 1
endif

ifeq ($(SOC_GENERATION),4750)
JZ4750_BSP := 1
JZ4750_APUS := 1
JZ4750_SG := 0
endif

ifeq ($(SOC_GENERATION),4740)
JZ4740_PAV := 1
JZ4740_HAVE_DCDV := 0
JZ4725_VOLANS := 0
JZ4740_LYRA := 0
JZ4740_TV430 := 0
endif

# ----------------------------------------------------------------------------
# The System Configuration, change the settings carefully, Thank you!
# ----------------------------------------------------------------------------
JZ4740_PMP16 := 1
NO_DRIVER := 0
