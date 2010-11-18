/*
**********************************************************************
*
*                            uC/MMC
*
*             (c) Copyright 2005 - 2007, Ingenic Semiconductor, Inc
*                      All rights reserved.
*
***********************************************************************

----------------------------------------------------------------------
File        : mmc_jz.c
Purpose     : Jz47xx low level functions to MMC/SD.

----------------------------------------------------------------------
Version-Date-----Author-Explanation
----------------------------------------------------------------------
1.00.00 20060831 WeiJianli     First release

----------------------------------------------------------------------
Known problems or limitations with current version
----------------------------------------------------------------------
(none)
----------------------------------------------------------------------
*/

/***************************************************************************
 * Define for DMA mode
 *
 */
#include "dm.h"
#include "mmc_protocol.h"
#include "mmc_api.h"
#include "mmc_core.h"
#include "os_api.h"

int jz_mmc_exec_cmd(struct mmc_request *request)
{
}

/* Set the MMC clock frequency */
void jz_mmc_set_clock(int sd, unsigned int rate)
{
}


/*******************************************************************************************************************
** Name:	  void mmc_hardware_init()
** Function:      initialize the hardware condiction that access sd card
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
static int init_jz_mmc_hardware_init = 0;
int jz_mmc_hardware_init(void)
{
	return 0;
}

#if (DM==1)
int mmc_poweron(void)
{
	return 0;
}
int mmc_poweroff(void)
{
	return 0;
}
int mmc_convert(void)
{
	return 0;
}

void mng_init_mmc(void)
{
	struct dm_jz4740_t mmc_dm;
	mmc_dm.name = "MMC driver";
//	mmc_dm.init = jz_mmc_hardware_init;
//	mmc_dm.poweron = mmc_poweron;
//	mmc_dm.poweroff = mmc_poweroff;
//	mmc_dm.convert = mmc_convert;
//	dm_register(1, &mmc_dm);
}
#endif
