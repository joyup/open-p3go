/********************** BEGIN LICENSE BLOCK **********************************
//
// INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
// Copyright(C) Ingenic Semiconductor Co. Ltd 2009. All rights reserved.
// 
// This file, and the files included with this file, is distributed and made 
// available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
// EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
// INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
// 
// http://www.ingenic.cn 
//
//********************* END LICENSE BLOCK ************************************
//
//  Author:  <xyzhang@ingenic.cn>
//
//  Create:   2008-10-09, by xyzhang
//  Maintain: 2009-03-20, by Rocky Xu, Clean up the code and change the
//			  return value of the MMC_DetectStatus()
//            
//
//***************************************************************************/

#include "jz4755.h"
#include "jzfs_api.h"
#include "threadprio.h"
#include "mmc_config.h"
#include "os_api.h"

#ifdef USE_MIDWARE
	#include "midware.h"
#endif

#define CARD_IN		1
#define CARD_OUT	0 

#define MMC_GPIO_TASK_PRIO			MMC_THREAD_PRIO //6
#define MMC_GPIO_TASK_STK_SIZE		1024
static OS_TASK_STACK MGTaskStack[MMC_GPIO_TASK_STK_SIZE];

unsigned char g_card_state = CARD_OUT;
static OS_EVENT *mmc_detect_event = NULL;

#ifdef USE_MIDWARE
	static unsigned int		mmcid;
	volatile static MIDSRC	mmcsrc;
#endif

//----------------------------------------------------------------------------

static void midw_card_in(void)
{
#ifdef USE_MIDWARE
	mmcsrc.Src = SRC_MMC;
	mmcsrc.Event = EVENT_MMC_IN;
	os_QueuePost(mmcsrc.CurEvent1, (void *)&mmcid);
	os_SemaphorePost(mmcsrc.CurEvent);
#endif
}

//----------------------------------------------------------------------------

static void midw_card_out(void)
{
#ifdef USE_MIDWARE
	mmcsrc.Src = SRC_MMC;
	mmcsrc.Event = EVENT_MMC_OUT;
	os_QueuePost(mmcsrc.CurEvent1, (void *)&mmcid);
	os_SemaphorePost(mmcsrc.CurEvent);
#endif
}

//----------------------------------------------------------------------------

#ifdef USE_MIDWARE
static void midw_get_request(MIDSRCDTA *dat)
{
}

//----------------------------------------------------------------------------

static void midw_response(MIDSRCDTA *dat)
{
}
#endif

//----------------------------------------------------------------------------

void detect_intr_handler(unsigned int arg)
{
	unsigned char err;

	disable_irq(EIRQ_GPIO_BASE + MMC_CD_PIN);
	g_card_state = CARD_OUT;
	os_SemaphoreSet(mmc_detect_event, 0, &err);
	os_SemaphorePost(mmc_detect_event);
}

//----------------------------------------------------------------------------

bool is_mmc_card_in(unsigned int pin)
{
	unsigned int group = pin / 32, bit = 1 << (pin % 32);

	if (INREG32(A_GPIO_PXPIN(group)) & bit)
		return (CARD_INSERT_VALUE == GPIO_HIGH_LEVEL);
	else
		return (CARD_INSERT_VALUE == GPIO_LOW_LEVEL);
}

//----------------------------------------------------------------------------

void detect_intr_init(unsigned int card_in)
{
#if (CARD_INSERT_VALUE == GPIO_HIGH_LEVEL)
	card_in = !card_in;
#endif

	if (card_in == CARD_IN)
		gpio_intr_init(GITT_H_LEVEL, MMC_CD_PIN);
	else
		gpio_intr_init(GITT_L_LEVEL, MMC_CD_PIN);
}

//----------------------------------------------------------------------------

static void detect_intr_task(void *arg)
{
	unsigned char err;

	while (1)
	{
		os_SemaphorePend(mmc_detect_event, 0, &err);

//		os_TimeDelay(50);
		os_TimeDelay(200);
		if (is_mmc_card_in(MMC_CD_PIN))
		{
			printf("MMC Card Insert!\n");
			g_card_state = CARD_IN;
			midw_card_in();
			MMC_Initialize(MMC_4BIT_MODE);
			detect_intr_init(CARD_IN);
		}
		else
		{
			printf("MMC Card Remove!\n");
			g_card_state = CARD_OUT;
			midw_card_out();
			detect_intr_init(CARD_OUT);
		}
		enable_irq(EIRQ_GPIO_BASE + MMC_CD_PIN);
	}
}

// ---------------------------------------------------------------------------
//
// @func    int | mmc_check_status | check card status 
//
//			NONE
//
// @rdesc   CARD IN return 1, otherwise return 0.
//
// ---------------------------------------------------------------------------
int mmc_check_status(void)
{
    //	printf("+mmc_check_status %d\n", g_card_state == CARD_IN);
	if (g_card_state == CARD_IN)
		return (1);
	else
		return (0);
}

// ---------------------------------------------------------------------------
//
// @func    int | mmc_write_protect | check card write protect status 
//
//			NONE
//
// @rdesc   Write protect return 1, otherwise return 0.
//
// ---------------------------------------------------------------------------
int mmc_write_protect(void)
{
	return 0;
}

// ---------------------------------------------------------------------------
//
// @func    void | mmc_detect_init | mmc card detect init
//
//			NONE
//
// @rdesc   NONE.
//
// ---------------------------------------------------------------------------
void mmc_detect_init(void)
{
	mmc_detect_event = os_SemaphoreCreate(1);
	os_TaskCreate(detect_intr_task, (void *)0,
		     (void *)&MGTaskStack[MMC_GPIO_TASK_STK_SIZE - 1],
		     MMC_GPIO_TASK_PRIO);

#if (CARD_INSERT_VALUE == GPIO_LOW_LEVEL)
	gpio_intr_init(GITT_L_LEVEL, MMC_CD_PIN);
#else
	gpio_intr_init(GITT_H_LEVEL, MMC_CD_PIN);
#endif
	request_irq(EIRQ_GPIO_BASE + MMC_CD_PIN, detect_intr_handler, 0);

#ifdef USE_MIDWARE
	mmcsrc.GetRequest = midw_get_request;
	mmcsrc.Response = midw_response;
	mmcsrc.Name = "MMC";

	printf("Register Midware SRC MMC! \n");
	RegisterMidSrc((PMIDSRC)&mmcsrc);
	mmcid = mmcsrc.ID;
	printf("mmc ID %d \n", mmcsrc.ID);
#endif
}

//----------------------------------------------------------------------------
