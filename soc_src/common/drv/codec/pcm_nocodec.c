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


int pcm_ioctl(unsigned int cmd, unsigned long arg)
{
	return 0;
}


int pcm_read(char *buffer, int count)
{
	return 0;
}
void pcm_set_write_pagesize(unsigned int pagesize)
{
}
int pcm_write(char *buffer, int count)
{
	return 0;
}
int pcm_init()
{
	return 0;
}


void  jz_codec_poweron(int d)
{
}
void  jz_codec_poweroff(int d)
{
}

int Buffer_Init(void *buf, unsigned int BufferLen, unsigned int BufferCount)
{
	return 1;
}

void pcm_fill_buff(void *tar,void* src,int size)
{
}

#if (DM==1)
int  codec_poweron(void)
{
}
int  codec_poweroff(void)
{
}
int  codec_preconvert(void)
{
	return 1;
}
int  codec_convert(void)
{
}
void  mng_init_codec(void)
{
	
}
#endif


