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
 *  Author:  <xyzhang@ingenic.cn>  
 *
 *  Create:   2008-06-26, by xyzhang
 *            
 *  Maintain:
 *            
 *
 *******************************************************************************
 */

int i2cucs_read(unsigned char device, unsigned char *buf,
	      int count)
{
	
	return -1;
}
int i2cucs_write(unsigned char device, unsigned char *buf,
	       int count)
{

	return -1;
}

char i2c_sim_common_read(unsigned char ucSlaveID, unsigned char *pucBuffer, unsigned char ucBytes)
{
	return (0);
}
char i2c_sim_common_write(unsigned char ucSlaveID, unsigned char *pucBuffer, unsigned char ucBytes)
{
	return (0);
}
//------------------------------------------------------------------------------
void sleep(unsigned int time)
{

}

void fm_set_stby(void)
{
	
	return;
}

int  fm_deinit(void)
{
	
	return (0);
}
//------------------------------------------------------------------------------

int fm_init( void )
{
	
	return ( 0 );
}
//-----------------------------------------------------------------------------
int fm_hlsi_optimal(unsigned int freq)
{
   
}
//------------------------------------------------------------------------------
void fm_set_volume(int vol_level)
{
	
}
int fm_get_volume(void)
{
	
}
int fm_get_max_vol(void)
{
	return (31);	
}
int fm_get_min_vol(void)
{
	return (0);
}
//Calculate frequency from PLL
void fm_set_frequency(int freq)
{
	}
int fm_get_frequency(void)
{
   
	return 0;
}
char   fm_is_band_limit( void )
{

		return ( 0 );		
}
char   fm_is_ready( void )
{
	
		return ( 0 );		
}
//------------------------------------------------------------------------------

unsigned int fm_open (unsigned int dwData, unsigned int dwAccess, unsigned int dwShareMode) {return (4);}

//------------------------------------------------------------------------------

int fm_read(void)
{
	return (0);
}

//------------------------------------------------------------------------------

int fm_write(void) 
{
	return (0);
}

//------------------------------------------------------------------------------
void mul_search (void )
{

} 

int auto_search ( int direc)
{
		return (0);
}

void auto_search_save( int dir )
{
	
} 
int	fm_set_status( int mute, int stereo, int level, int band)
{
	
	return 0;
		
}
	  
int fm_get_status( void )
{

	return 0;
}

int	fm_set_mute(int mute)
{

	  return 0;
}

int fm_test_station( int freq,int threshold )
{
	return 0;
}

