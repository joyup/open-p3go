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
 *  Author:  <jgao@ingenic.cn> 
 *
 *  Create:   2008-06-26, by jim gao
 *            
 *  Maintain: 
 *            
 *
 *******************************************************************************
 */


#ifndef _FUNCTION_H
#define  _FUNCTION_H

#define lcdstart           lcd_start
#define lcdstop            lcd_stop
#define TpanelInit         tpanel_init
#define Battery_Init       battery_init
#define Read_Battery       read_battery
#define ReadTsData         read_ts_data
#define SADC_DeInit        sadc_deinit
#define SADC_Init          sadc_init
#define TS_init            ts_init
//#define alignAlloc         align_alloc
//#define deAlloc            de_alloc
//#define Drv_calloc         drv_calloc
//#define Drv_realloc        drv_realloc
//#define printMemory        print_memory
//#define heapInit           heap_init
#define CONSOL_GetChar     consol_get_char
#define CONSOL_SendCh      consol_send_ch
#define GetCodeInit        get_code_init
#define GetLcdInit         get_lcd_init
#define GetLoadData        get_load_data
#define InitLoadCommon     init_load_common
#define myCRC8             my_crc8
#define SetLcdInit         set_lcd_init
#define SetLoadData        set_load_data
#define SetLoadInit        set_load_init
#define InitUdc            init_udc
#define InitUdcNand        init_udc_nand
#define InitUdcRam         init_udc_ram
#define InitUdcMMC         init_udc_mmc
#define DisableDevice      disable_device
#define EnableDevice       enable_device
#define EP0_Handler        ep0_handler 
#define EPIN_Handler       ep_in_handler
#define EPOUT_Handler      ep_out_handler
#define GPIO_Handle        gpio_handler
#define GPIO_IRQ_init      gpio_irq_init
#define InitEndpointSuppost  init_endpoint_suppost
#define InitUDC            init_udc
#define udc4740Proc        udc4740_proc
#define UDC_DetectStatus   udc_detect_status
#define UDC_DetectCableStatus udc_detect_cable_status
#define udcDisableDeviceDirectly udc_disable_device_directly_global
#define udcIntrbhandle     udc_intrb_handler
#define Init_Mass_Storage  init_mass_storage
#define RegisterDev        register_dev
#define FMM_Deinit         fm_deinit
#define FMM_Init           fm_init
#define FMM_Read           fm_read
#define FMM_Write          fm_write
#define Get_Frequency      fm_get_frequency
#define Set_Frequency      fm_set_frequency
#define Get_Max_Vol        fm_get_max_vol
#define Get_Min_Vol        fm_get_min_vol
#define Get_Pll            fm_get_pll
#define Get_Status         fm_get_status
#define Set_Status         fm_set_status
#define Get_Volume         fm_get_volume
#define Set_Volume         fm_set_volume
#define Set_Mute           fm_set_mute
#define HLSI_optimal       fm_hlsi_optimal
#define I2CSim_CommonRead  i2c_sim_common_read
#define I2CSim_CommonWrite i2c_sim_common_write
#define Get_atom_time      rtc_get_atom_time
#define Get_week_from_date rtc_get_week_from_date
#define Set_RTC_ABS        rtc_set_rtc_abs
#define Set_RTC_datetime   rtc_set_rtc_datetime
#define Get_curUrtime      rtc_get_cur_urtime
#define Is_double_year     rtc_is_double_year
#define Jz_rtc_init        jz_rtc_init


#endif /* _ASM_TYPES_H */
