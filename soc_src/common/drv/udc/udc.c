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


#include <os_api.h>
#include "usb.h"
#include "udc.h"
#include "udcbus.h"
#ifdef USE_MIDWARE
#include "midware.h"
#endif

#ifdef USE_MIDWARE
volatile static MIDSRC udcsrc;
volatile static MIDSRCDTA res;
volatile static unsigned int udcid;
volatile static unsigned int cable_stat;
volatile static unsigned int protocool_stat;
static unsigned char err;
#define CABLE_CONNECTED          0
#define CABLE_DISCONNECT         1
#endif

/*   interface   */

void udc_disable_device_directly_global(void)
{
}
void enable_device(unsigned int handle)
{

	
}
void disable_device(unsigned int handle)
{

}

void start_transfer(unsigned int handle,unsigned char ep,unsigned char *buf,unsigned int len)
{
}
static void set_address(unsigned int handle,unsigned short value)
{
}

void init_endpoint_suppost(unsigned int handle,unsigned char *ep,USB_ENDPOINT_TYPE ep_type,unsigned short *ep_max_pkg)
{

}

#ifdef USE_MIDWARE
static void GetRequest(MIDSRCDTA *dat)
{
	dat->Val = res.Val;
//	printf("Up layer get :%d \n",res.Val);
}

static void Response(MIDSRCDTA *dat)
{

	res.Val = dat->Val;
	printf("Up layer said :%d \n",res.Val);

}
#endif

void init_udc(PUDC_BUS pBus)
{
	pBus->EnableDevice = enable_device;
	pBus->SetAddress = set_address;
	pBus->StartTransfer = start_transfer;
	pBus->InitEndpointSuppost = init_endpoint_suppost;
	pBus->DisableDevice = disable_device;
	printf("Init UDC %s %s\n",__DATE__,__TIME__);

#ifdef USE_MIDWARE
	udcsrc.GetRequest = GetRequest;
	udcsrc.Response = Response;
	udcsrc.Name = "UDC";
	printf("Register Midware SRC udc! \n");
	RegisterMidSrc((PMIDSRC)&udcsrc);
	udcid = udcsrc.ID;
//	res.Val = 0xffff;
	cable_stat = CABLE_DISCONNECT;
	protocool_stat = CABLE_DISCONNECT;
#endif	
}

int udc_detect_status(void)
{
}

int udc_detect_cable_status(void)
{
}

unsigned int MidCheckBatState(void)
{
	return (BAT_NOT_CHARGING);
}

unsigned int BAT_GetDCDetect(void)
{
	return (0);
}
