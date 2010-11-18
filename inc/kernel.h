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
*/

#ifndef   __KERNEL_H__
#define   __KERNEL_H__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define KERNEL_RESTART_SCHEDULE    1
#define KERNEL_STOP_SCHEDULE 			 2
#define KERNEL_PM_CONTROL          3
#define KERNEL_PM_SETAUDIOCONTROL  4
#define KERNEL_PM_CLRAUDIOCONTROL  5
#define KERNEL_BACKLIGHT_CTRL      6
#define KERNEL_LCD_CTRL            7


int kernel_ioctl(void *buff, unsigned int cmd);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__KERNEL_H__
