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

/*********************************************************************
*
*             Global functions
*
**********************************************************************

  Functions here are global, although their names indicate a local
  scope. They should not be called by user application.
*/

int MMC_LB_Init() {

	return -1;
}
int MMC_LB_FLASHCACHE() {

	return 0;
}
int MMC_LB_Deinit()
{
}
int MMC_LB_MultiRead(unsigned int Sector, void *pBuffer,unsigned int SectorCount) {
	return 0;
}
	
int MMC_LB_Read(unsigned int Sector, void *pBuffer) {
	return 0;
}

int MMC_LB_MultiWrite(unsigned int Sector, void *pBuffer,unsigned int SectorCount) {
  
	return 0;
}


int MMC_LB_Write(unsigned int Sector, void *pBuffer) {

	return 0;
}
