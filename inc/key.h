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

#ifndef __KEY_H__
#define __KEY_H__

typedef void (*PFN_KEYHANDLE)(unsigned int key);
extern PFN_KEYHANDLE UpKey;
extern PFN_KEYHANDLE DownKey;

int Key_GetPowerOn();
void KeyInit();

#define PK_A  					0x00000001
#define PK_B  					0x00000002
#define PK_L  					0x00000004
#define PK_R   					0x00000008
#define PK_UP                   0x00000010
#define PK_DOWN                 0x00000020
#define PK_LEFT   				0x00000040
#define	PK_RIGHT				0x00000080
#define PK_START				0x00000100
#define PK_SELECT				0x00000200
#define PK_BRIGHT				0x00000400

#define PK_ESC                  PK_B      // ESC
#define PK_MENU 				PK_SELECT
#define PK_PLAY                 PK_A      // PLAY
#define PK_OK 					PK_START
#define PK_LOCK					0x00040000	//0x00000400
#define PK_UNLOCK				0x00080000	//0x00000400
#define PK_POWER_				0x00020000

#define PK_NOKEY				0x10000000	//NO KEY

#define	PK_VOLSUB			 	PK_DOWN
#define PK_VOLADD			   	PK_UP		 // œÚ”“

#define PK_VOL                  PK_NOKEY
#define PK_AB                   PK_NOKEY
#define PK_EQ                   PK_NOKEY
#define PK_REC                  PK_NOKEY
#define PK_CAMERA               PK_NOKEY


#endif //__KEY_H__
