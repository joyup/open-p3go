/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4750  mobile_tv  Project  V1.0.0
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
 *  Author:  <jbyu@ingenic.cn>
 *
 *  Create: 2008-09-26, by jbyu
 *
 *******************************************************************************
 */

unsigned int lcd_get_width(void)
{
	return 0;
}

unsigned int lcd_get_height(void)
{
	return 0;
}

unsigned int lcd_get_bpp(void)
{
	return 0;
}
unsigned char* lcd_get_frame(void)
{
	return 0;
}

unsigned char* lcd_get_cframe(void)
{
	return 0;
}

unsigned char* lcd_get_change_frame(void)
{
	return 0;
}

unsigned char* lcd_get_change_cframe(void)
{
	return 0;
}

unsigned char* lcd_get_change_phyframe(void)
{
	return 0;
}

unsigned char* lcd_get_current_frame(void)
{
	return 0;
}

unsigned char* lcd_get_current_cframe(void)
{
	return 0;
}

unsigned char* lcd_get_current_phyframe(void)
{
	return 0;
}

void lcd_flush_frame(int x,int y,int w,int h)
{

}

void lcd_clean_frame_other()
{
}

void lcd_clean_frame_all()
{
}
void lcd_flush_frame_all()
{
	
}

void lcd_change_frame(void)
{
	
}





void FrameBufferMalloc(void)
{

}


void lcd_comm_mode_init(void)
{

}

void lcd_osd_mode_init(void)
{

}


int jzlcd_init(void)
{
	return 0;

}

static void lcd_reinit()
{
}
#if (DM==1)
int  lcd_poweron(void)
{
}
int  lcd_poweroff(void)
{
}
int lcd_preconvert(void)
{
//	__cpm_stop_lcd();

	return (1);
}
int  lcd_convert(void)
{
}

void  mng_init_lcd(void)
{
}

void lcd_stop()
{
    ;
}
#endif

void lcd_start(void)
{

}


void lcd_enter_frame(void)
{

}

void lcd_reset_frame(void)
{

}

void lcd_to_tv(void)
{

}
int lcd_open(void)
{
return (0);
}
int lcd_close(unsigned int type)
{
return (0);
}
void tv_to_lcd(void)
{
}
