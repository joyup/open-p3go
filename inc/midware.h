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
 *  Author: <yliu@ingenic.cn>  <jgao@ingenic.cn> <xyzhang@ingenic.cn>
 *          <xliu@ingenic.cn>
 *  Create:   2008-03-04, by yuan liu
 *  Maintain: 2008-05-04, by jim gao
 *  Maintain: 2008-07-04, by xyzhang
 *  Maintain: 2009-04-24, by xliu
 *  Maintain: 
 *            
 *
 *******************************************************************************
 */

#ifndef _MIDWARE_H_
#define _MIDWARE_H_

#define MAX_MID_SRC             10
#define MAX_MID_OBJ             1

#define EVENT_KEY_UP            0x00000001
#define EVENT_USB_IN            0x00000002
#define EVENT_USB_OUT           0x00000004
#define EVENT_POWER_IN          0x00000008
#define EVENT_POWER_OUT         0x00000010
#define EVENT_MMC_IN            0x00000020
#define EVENT_MMC_OUT           0x00000040
#define EVENT_POWER_OFF         0x00000080
#define EVENT_CHARG_ON          0x00000100
#define EVENT_CHARG_OFF         0x00000200
#define EVENT_LCD_ON            0x00000400
#define EVENT_LCD_OFF           0x00000800
#define EVENT_UNINSTALL         0x00001000
#define EVENT_AUTOOFF           0x00002000
#define EVENT_BAT_CRITICAL_LOW  0x00004000
#define EVENT_BAT_FULL          0x00008000
#define EVENT_SET_ALM           0x00010000
#define EVENT_SET_TIMER         0x00020000
#define EVENT_HP_IN             0x00040000
#define EVENT_HP_OUT            0x00080000
#define EVENT_BAT_LOW           0x00100000

#define SRC_KEY                 1
#define SRC_UDC                 2
#define SRC_MMC                 3
#define SRC_POWER_OFF           4
#define SRC_CHARG_DETE          5
#define SRC_POWER_LOW           6
#define SRC_SET_ALM             7
#define TCU_SET_TIMER           8
#define SRC_KEY_DOWN            9

#define MIDWARE_SELF            (MAX_MID_SRC + 1)

#define BAT_CHARGING            1
#define BAT_NOT_CHARGING        2
#define BAT_FULL                3
#define BAT_VERY_LOW            4
#define BAT_LOW_1               5

//#define USE_AD_CHARGE         1
#define BAT_FULL_VALUE          4150

#if 1 //xltao dele
#define	BAT_MAX_VOLT            4300		//	Unit, mV
#else
#define	BAT_MAX_VOLT            4035		//	Unit, mV
#endif

#if 0 //xltao del
#define	BAT_MIN_VOLT            3550		//	Unit, mV
#else
#define	BAT_MIN_VOLT            3150		//	Unit, mV
#endif

#define FULL_BAT_TIME   	    (2 * 60 * 60)
#define BAT_CAPACITY		    ((BAT_MAX_VOLT - BAT_MIN_VOLT) * 2)	// 1200
#define	BAT_DISCHARG_TIME	    (FULL_BAT_TIME / BAT_CAPACITY)	// 6


typedef struct
{
	unsigned int Obj;
	unsigned int Val;
}MIDSRCDTA;

typedef struct
{
	unsigned int ID;
	unsigned char *Name;
	unsigned int Src;
	unsigned int Event;
	void (*GetRequest) (MIDSRCDTA *);
	void (*Response) (MIDSRCDTA *);
	OS_EVENT *CurEvent, *CurEvent1, *CurEvent2;
} MIDSRC,* PMIDSRC;

typedef unsigned int (* PFN_MID_CALLBACK)(unsigned int dat);
typedef struct
{
	unsigned int ID;
	unsigned char *Name;
	PFN_MID_CALLBACK  Notify;
	void (*SetLcdBLLevel)(unsigned int);          // 设置背光亮度
	void (*SetLcdBLTime)(unsigned int);           // 设置背光时间
	void (*SetAutoOffTime)(unsigned int );        // 设置自动关机时间
	void (*AdjustBatteryCheck)(void);             // 校准电池采样定时器
	int  (*GetBatteryPercent)(void);              // 获得电池电压百分比
	int  (*SleepEnable)(int);                     // 是否允许休眠
	void (*UsbLink)(void);                        // 强制要求USB进行连接
	void (*RegisterUIThread)(unsigned int id);    // 注册动画线程
	void (*RemoveUIThread)(unsigned int id);      // 注销动画线程
	void (*SetAudioEffectFlag)(unsigned int flag);// 设置音乐是否开音效标志
} MIDOBJ,* PMIDOBJ;

int RegisterMidSrc(PMIDSRC);

MIDOBJ* MidWareInit(PFN_MID_CALLBACK func);

#endif
