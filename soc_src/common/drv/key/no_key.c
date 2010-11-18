/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * http://www.ingenic.cn 
 *
 ********************** END LICENSE BLOCK **************************************
 *
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn> 
 *
 *  Create:   2008-06-26, by dsqiu
 *            
 *  Maintain: 2008-06-26, by jgao
 *            
 *
 *******************************************************************************
 */

#include <key.h>
#include <os_api.h>

#if (KEYTYPE==3)
#define KEY_NUM 5
#elif (KEYTYPE==2)
#define KEY_PIN (32*3 + 18)
#define KEY_NUM 6
#else
#define KEY_PIN (32*3 + 0)
#define KEY_NUM 6
#endif

#define KEY_MASK 0x03f;

#ifdef USE_MIDWARE
#include "midware.h"
#endif
PFN_KEYHANDLE UpKey = NULL;
PFN_KEYHANDLE DownKey = NULL;

#ifdef USE_MIDWARE
#include <os_api.h>
static MIDSRC keysrc;
static MIDSRCDTA res;
static unsigned int keyid;

static MIDSRC poffsrc;
static MIDSRCDTA res;
static unsigned int poffid;


static int DownKeyHandle();
static void UpKeyHandle();

static void info_power_off()
{
	keysrc.Src = SRC_POWER_OFF;
	keysrc.Event = EVENT_POWER_OFF;
	os_QueuePost(keysrc.CurEvent1 , (void *)&keyid);
	os_SemaphorePost(keysrc.CurEvent);
}

static void GetRequest(MIDSRCDTA *dat)
{
//	printf("Up layer get :%d \n",res.Val);
}

static void Response(MIDSRCDTA *dat)
{
	}

void check_poweroff()
{

}
#endif

static void UpKeyHandle(int key) 
{
;
}

static int DownKeyHandle(int key) 
{
;
}
void KeyInit()
{

}

int Key_GetPowerOn()
{
	return 0;
}


