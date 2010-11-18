

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
#ifdef USE_MIDWARE
#include <midware.h>
#endif
int ts_init(void)
{
	return 0;
}

/********* battery start ************************************************/
int battery_init()
{
	
}

int read_battery(void)
{
#ifdef USE_MIDWARE
	return BAT_MAX_VOLT;
#else
	return 0;	
#endif
	
}

/************ battery end ************************************************/

#ifdef USE_MIDWARE //tcu 2

void BAT_StopTimer(void)
{

}

void tcu_timer_set(unsigned int d, unsigned int tps)
{

}

void JZ_Init_Battery_Timer()
{

}

void JZ_Start_Battery_Timer()
{

}

#endif
