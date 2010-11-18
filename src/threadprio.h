/********************** BEGIN LICENSE BLOCK ************************************
 *
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2009. All rights reserved.
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

#ifndef __THREAD_PRIO_h__
#define __THREAD_PRIO_h__

// Group0
#define AUDIO_THREAD_PRIO	1
#define KEY_THREAD_PRIO		3
#define MIDI_THREAD_PRIO			4
#define MASS_STORAGE_THREAD_PRIO	5
#define MMC_THREAD_PRIO			6
#define UDC_THREAD_PRIO			7
#define SADC_THREAD_PRIO		9

// Group1
#define TSSI_THREAD_PRIO		8

#define MIDI_BKGROUND_THREAD_PRIO 180

/*For Jelly driver*/

#define TASK_APP_PRIO_BASE        40
#define TASK_JELLYMGR_PRIO        (TASK_APP_PRIO_BASE + 0)
#define TASK_TSPOLL_PRIO          (TASK_APP_PRIO_BASE + 1)
#define TASK_SQPOLL_PRIO          (TASK_APP_PRIO_BASE + 2)
#define TASK_EVENTMGR_PRIO        (TASK_APP_PRIO_BASE + 3)


#endif
