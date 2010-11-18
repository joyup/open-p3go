/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
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
 *  Author:  <dsqiu@ingenic.cn>  <zzhang@ingenic.cn> 
 *
 *  Create:   2008-10, by zhang zhi
 *            
 *  Maintain: 2008-11, by zhang zhi
 *            
 *
 *******************************************************************************
 */

#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>
#include <mipsregs.h>

//--------------------------------------------------------------------
#include <dm.h>

static int jz_divcovert(PPLL_OPT pllopt);
static int jz_pllcovert(PPLL_OPT pllopt);

//--------------------------------------------------------------------

//                                  cpu speed = cpu clock / cdiv
//                                  mem speed = cpu clock / mdiv
//                                  cpu clock     cdiv   mdiv   h1div,  pdiv,  h0div
#if 0
static PLL_OPT opt_pll[3] = {
																	{CFG_CPU_SPEED,  4,     4,			  2,    4,    4},
																	{CFG_CPU_SPEED,  2,     4,			  2,    4,    4},
																	{CFG_CPU_SPEED,  1,     3,        2,    3,    3}
															};
															
static int (*jz_pm_do_pllconvert)(PPLL_OPT pllopt) = jz_divcovert;
static PPLL_OPT initpll_opt = &opt_pll[2];

#else
#define OVER_CPU_SPEED  432000000
#define MP3_CPU_SPEED   48000000 
static PLL_OPT opt_pll[6] = {
																	{CFG_CPU_SPEED,  4,     4,			  2,    4,    4},
																	{CFG_CPU_SPEED,  2,     4,			  2,    4,    4},
																	{CFG_CPU_SPEED,  1,     3,        2,    3,    3},
																	{0,  1,     1,			  1,    1,    1},
																	{OVER_CPU_SPEED,    1,     3,        2,    3,    3},
																	{MP3_CPU_SPEED,     1,     3,        2,    3,    3}
															};

static int (*jz_pm_do_pllconvert)(PPLL_OPT pllopt) = jz_pllcovert;
static PPLL_OPT initpll_opt = &opt_pll[2];
#endif
															

int get_sdram_configure(unsigned int pllin,unsigned int *sdram_freq);
int find_cpudiv(int div);
static void pll_convert(PPLL_OPT pllopt,unsigned int *pll_cfcr,unsigned int *pll_plcr1);
static int save_reg[5];
static int save_gpio[6][4];
static int save_gpio2[6][4];
static int gpio_mask[6] = { 0xffffffff,
							0xffffffff,
							0xffffffff,
							0xffffffff,
							0xffffffff,
							0xffffffff,
						  };

#define __gpio_port_as_sleep(p, o)		\
do {						\
	OUTREG32(A_GPIO_PXFUNC(p),(o));		\
	OUTREG32(A_GPIO_PXSELC(p),(o));		\
	OUTREG32(A_GPIO_PXDIRC(p),(o));		\
	OUTREG32(A_GPIO_PXPEC(p),(o));		\
} while (0)
static gpioMask_configured=0;
static void gpioMask(unsigned int pin)
{	gpio_mask[pin/32]&=~(1<<pin%32);
}
static void init_gpioMask()
{
	if(!gpioMask_configured)
	{   // Interrupt pins below will be alive in sleep mode
#if 0   //xltao del
		gpioMask(KEY_VOL_DECR_PIN);
		gpioMask(KEY_VOL_INCR_PIN);
		gpioMask(KEY_AUD_PREV_PIN);
		gpioMask(KEY_AUD_NEXT_PIN);
		gpioMask(KEY_MENU_PIN);
#endif
		gpioMask(POWER_OFF_PIN);	
		gpioMask(GPIO_UDC_DETE_PIN);	
		gpioMask(AD_KEY_INT_PIN);
		gpioMask_configured=1;
	}
}
/*static void config_irq_wakeup(void)
{

#if (KEY_DOWN_VALUE == GPIO_HIGH_LEVEL)
	gpio_intr_init(GITT_R_EDGE, KEY_VOL_DECR_PIN);
	gpio_intr_init(GITT_R_EDGE, KEY_VOL_INCR_PIN);
	gpio_intr_init(GITT_R_EDGE, KEY_AUD_PREV_PIN);
	gpio_intr_init(GITT_R_EDGE, KEY_AUD_NEXT_PIN);
	gpio_intr_init(GITT_R_EDGE, KEY_MENU_PIN);
#else
	gpio_intr_init(GITT_F_EDGE, KEY_VOL_DECR_PIN);
	gpio_intr_init(GITT_F_EDGE, KEY_VOL_INCR_PIN);
	gpio_intr_init(GITT_F_EDGE, KEY_AUD_PREV_PIN);
	gpio_intr_init(GITT_F_EDGE, KEY_AUD_NEXT_PIN);
	gpio_intr_init(GITT_F_EDGE, KEY_MENU_PIN);
#endif

	// The pin can NOT be configured to R_EDGE
	gpio_intr_init(GITT_F_EDGE, POWER_OFF_PIN);
	
	gpio_intr_init(GITT_R_EDGE, GPIO_UDC_DETE_PIN);
	
	gpio_intr_init(GITT_F_EDGE, AD_KEY_INT_PIN);

	
}
*/
static void config_gpio_savepower(void)
{
	__gpio_port_as_sleep(2,gpio_mask[2]);
	__gpio_port_as_sleep(3,gpio_mask[3]);
	__gpio_port_as_sleep(4,gpio_mask[4]);
	__gpio_port_as_sleep(5,gpio_mask[5]);

	OUTREG32(A_GPIO_PXPES(4),0x024c0141);

/*	
	//AMPEN_N
	__gpio_as_output(4*32+5);
	__gpio_clear_pin(4*32+5);
*/
}
void config_gpio_savepower2(void)//for mp3
{

//	__gpio_port_as_sleep(4,0xffffffff);
//	__gpio_port_as_sleep(5,0xffffffff);

	//__gpio_port_as_sleep(3,0x003fffff);
	//mdelay(200);
	//jz_pm_control(4);
	__gpio_port_as_sleep(3,0xffffffff);
	__gpio_port_as_sleep(2,0x47c00000);
	
	//SETREG32(A_CPM_CLKGR,0xffff85d9);
	//SETREG32(A_CPM_CLKGR,0xfffff5d9);
				
	//__gpio_as_output(MMC1_POWER_PIN);//__gpio_as_output(4*32+3);
	//__gpio_clear_pin(MMC1_POWER_PIN);//__gpio_clear_pin(4*32+3);
	
	//MMC1_CD_PIN		
	OUTREG32(A_GPIO_PXPES(4),BIT4);//__gpio_as_input(4*32+4);
								   //OUTREG32(A_GPIO_PXPES(4),BIT4);
				
	__gpio_as_input(4*32+8);//cd_det
	
	OUTREG32(A_GPIO_PXPES(4),BIT6);//udc_det	
	OUTREG32(A_GPIO_PXPES(4),BIT9);//charge_det
	OUTREG32(A_GPIO_PXPEC(4),BIT9);//!should be pes
	OUTREG32(A_GPIO_PXPES(4),BIT23);//uart
				
				

/*	
	//AMPEN_N
	__gpio_as_output(4*32+5);
	__gpio_clear_pin(4*32+5);
*/
}
static void config_source_savepower(void)
{
	CLRREG32(A_EMC_NFCSR,0xff);
	CLRREG8(A_SADC_ADENA,7);
	CLRREG32(A_AIC_AICFR,1<<0);
	CLRREG32(A_LCD_LCDCTRL,1<<3);
	SETREG32(A_CPM_OPCR,1<<7);
	CLRREG32(A_CPM_OPCR,1<<6);
	                                                                                                                                                    
	CLRREG32(A_CPM_OPCR,1<<4); 
	SETREG32(A_CPM_OPCR,1<<2);

	CLRREG32(A_CPM_LCR,LCR_LPM_MASK);
	SETREG32(A_CPM_LCR,LCR_SLEEP_MODE);
}

static void save_gpio_regs(void)
{	char i;
	for(i=0;i<6;i++)
	{
		save_gpio[i][0] = INREG32(A_GPIO_PXFUN(i));
		save_gpio[i][1] = INREG32(A_GPIO_PXSEL(i));
		save_gpio[i][2] = INREG32(A_GPIO_PXDIR(i));
		save_gpio[i][3] = INREG32(A_GPIO_PXPE(i));
	}
}
void save_gpio_regs2(void)
{	char i;
	for(i=0;i<6;i++)
	{
		save_gpio2[i][0] = INREG32(A_GPIO_PXFUN(i));
		save_gpio2[i][1] = INREG32(A_GPIO_PXSEL(i));
		save_gpio2[i][2] = INREG32(A_GPIO_PXDIR(i));
		save_gpio2[i][3] = INREG32(A_GPIO_PXPE(i));
	}
}
static void restore_gpio_regs(void)
{	char i;
	for(i=0;i<6;i++)
	{
		OUTREG32(A_GPIO_PXFUNS(i),  save_gpio[i][0]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXFUNC(i),(~save_gpio[i][0])&gpio_mask[i]);
		OUTREG32(A_GPIO_PXSELS(i),  save_gpio[i][1]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXSELC(i),(~save_gpio[i][1])&gpio_mask[i]);
		OUTREG32(A_GPIO_PXDIRS(i),  save_gpio[i][2]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXDIRC(i),(~save_gpio[i][2])&gpio_mask[i]);
		OUTREG32(A_GPIO_PXPES(i),   save_gpio[i][3]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXPEC(i), (~save_gpio[i][3])&gpio_mask[i]);
	}
}
void restore_gpio_regs2(void)
{	char i;
	for(i=0;i<6;i++)
	{
		OUTREG32(A_GPIO_PXFUNS(i),  save_gpio2[i][0]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXFUNC(i),(~save_gpio2[i][0])&gpio_mask[i]);
		OUTREG32(A_GPIO_PXSELS(i),  save_gpio2[i][1]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXSELC(i),(~save_gpio2[i][1])&gpio_mask[i]);
		OUTREG32(A_GPIO_PXDIRS(i),  save_gpio2[i][2]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXDIRC(i),(~save_gpio2[i][2])&gpio_mask[i]);
		OUTREG32(A_GPIO_PXPES(i),   save_gpio2[i][3]&gpio_mask[i]);
		OUTREG32(A_GPIO_PXPEC(i), (~save_gpio2[i][3])&gpio_mask[i]);
	}
	//jz_pm_control(1);
}
static void save_internal_regs(void)
{
	save_reg[0] = INREG32(A_EMC_NFCSR);
	save_reg[1] = INREG32(A_SADC_ADENA);
	save_reg[2] = INREG32(A_AIC_AICFR);
	save_reg[3] = INREG32(A_LCD_LCDCTRL);
	save_reg[4] = INREG32(A_CPM_OPCR); 
	codec_reg_write(A_CODEC_CGR8, 0x1f);
	codec_reg_write(A_CODEC_CGR9, 0x1f);
	codec_reg_write(A_CODEC_CGR1, 0xff);
	codec_reg_write(A_CODEC_CGR10, 0);
	mdelay(150);
	codec_reg_set(A_CODEC_PMR1, SB_OUT);
	mdelay(250);
	codec_reg_set(A_CODEC_PMR2, SB_SLEEP);
	mdelay(100);
	codec_reg_set(A_CODEC_PMR2, SB);
	//os_TimeDelay(30);
	mdelay(100);
	codec_reg_set(A_CODEC_PMR1, SB_MIX | SB_DAC | SB_ADC);
}

static void restore_internal_regs(void)
{
	codec_reg_write(A_CODEC_CGR8, 0x1f);
	codec_reg_write(A_CODEC_CGR9, 0x1f);
	codec_reg_write(A_CODEC_CGR1, 0xff);
	codec_reg_write(A_CODEC_CGR10, 0);
	codec_reg_clear(A_CODEC_PMR2, SB_MC);
//	codec_reg_set(A_CODEC_PMR2, SB_MC);
	mdelay(50);
	codec_reg_clear(A_CODEC_PMR2, SB);
	mdelay(300);
	codec_reg_clear(A_CODEC_PMR2, SB_SLEEP);
	mdelay(10);
	codec_reg_clear(A_CODEC_PMR1, SB_DAC);
	mdelay(2);
	codec_reg_clear(A_CODEC_PMR1, SB_MIX);
	mdelay(2);
	codec_reg_clear(A_CODEC_PMR1, SB_OUT);
	
	OUTREG32(A_EMC_NFCSR  ,save_reg[0] );
	OUTREG32(A_SADC_ADENA ,save_reg[1] );
	OUTREG32(A_AIC_AICFR  ,save_reg[2] );
	OUTREG32(A_LCD_LCDCTRL,save_reg[3] );
	OUTREG32(A_CPM_OPCR   ,save_reg[4] ); 
}


static int jz_pm_do_hibernate(void)
{
	printf("Put CPU into hibernate mode.\n");
	serial_waitfinish();
 
	while(!(INREG32(A_RTC_RTCCR) & RTCCR_READY));//wait for write_ready
	CLRREG32(A_RTC_RTCCR,RTCCR_AE);//rtc_clear_alarm_flag
		
	while(!(INREG32(A_RTC_RTCCR) & RTCCR_READY));//wait for write_ready
	OUTREG32(A_RTC_HWRSR,0);//rtc_clear_hib_stat_all
	
	while(!(INREG32(A_RTC_RTCCR) & RTCCR_READY));//wait for write_ready
	OUTREG32(A_RTC_HSPR,0x12345678);//rtc_set_scratch_pattern(0x12345678)
	
	while(!(INREG32(A_RTC_RTCCR) & RTCCR_READY));//wait for write_ready
	SETREG32(A_RTC_HWCR,HWCR_EALM);//rtc_enable_alarm_wakeup
	
	while(!(INREG32(A_RTC_RTCCR) & RTCCR_READY));//wait for write_ready
	SETREG32(A_RTC_HRCR,0x0fe0);//rtc_set_hrcr_val(0x0fe0)
	
	while(!(INREG32(A_RTC_RTCCR) & RTCCR_READY));//wait for write_ready
//	SETREG32(A_RTC_HWFCR,(0x00FF << 4));//rtc_set_hwfcr_val((0x00FF << 4))
	SETREG32(A_RTC_HWFCR,(0x0001 << 5));//rtc_set_hwfcr_val((0x00FF << 4))
	
	while(!(INREG32(A_RTC_RTCCR) & RTCCR_READY));//wait for write_ready
   	SETREG32(A_RTC_HCR,HCR_PD);//rtc_power_down()   
   	
   	
   	      
	while(1);
        
	return 0;
}

static int jz_pm_do_sleep(void)
{	
   	unsigned long imr = INREG32(A_INTC_ICMR);
	unsigned char pa_status; //xltao add

	printf("+jz_pm_do_sleep\n");
	init_gpioMask();
	cli();
	OUTREG32(A_INTC_ICMSR, 0xFFFFFFFF);
	
	
    save_internal_regs();
    save_gpio_regs();
                                                                                                                                                        
    config_gpio_savepower();                                                                                                                            
    config_source_savepower();
    
	//OUTREG32(A_INTC_ICMSR, 0xFFFFFFFF);
	//cli();
	//config_irq_wakeup();
	OUTREG32(A_INTC_ICMCR, EIRQ_PORT_A2F_BIT);
	__asm__(".set\tmips3\n\t"
		"sync\n\t"
		"wait\n\t"
		"nop\n\t"
		".set\tmips0");
		
	CLRREG32(A_CPM_LCR,LCR_LPM_MASK);
	SETREG32(A_CPM_LCR,LCR_IDLE_MODE);
	sti();
	OUTREG32(A_INTC_ICMSR,imr);
	OUTREG32(A_INTC_ICMCR,~imr);
	restore_gpio_regs();
	restore_internal_regs();

	return 0;
}

static int jz_divcovert(PPLL_OPT pllopt)
{
	unsigned int cfcr, pllout,sdram_freq;
	unsigned int pll_cfcr,pll_plcr1;
	unsigned int t;
	unsigned int dmcr;
	  
	  pll_convert(pllopt,&pll_cfcr,&pll_plcr1);
    dmcr = get_sdram_configure(pllopt->cpuclock / pllopt->mdiv,&sdram_freq);

    t = read_c0_status();
    write_c0_status(t & (~1));
    
    CLRREG32(A_CPM_CPCCR,CPCCR_CHANGE_EN);
		OUTREG32(A_CPM_CPCCR,pll_cfcr);
		
		OUTREG32(A_EMC_RTCOR , sdram_freq);
		OUTREG32(A_EMC_RTCNT , sdram_freq);
		SETREG32(A_CPM_CPCCR,CPCCR_CHANGE_EN);
    write_c0_status(t);

    pllout = GetCurrentPLLClock(0);
    //printf("pll out new: %dMHZ 0x%08x 0x%08x \n",pllout/1000000,pll_cfcr,INREG32(A_CPM_CPCCR)); 
    return 0;
}
static inline void set_bypass(unsigned int dmcr,unsigned int flushcount)
{
		   unsigned int prog_entry = ((unsigned int)set_bypass / 32 - 1) * 32 ;
       unsigned int prog_size = 512;
       unsigned int i;
 				unsigned int pll_cfcr_bypass;
   
       for( i = (prog_entry);i < prog_entry + prog_size; i += 32)
		__asm__ __volatile__(
			"cache 0x1c, 0x00(%0)\n\t"
            :
			: "r" (i));
		
		pll_cfcr_bypass = 0xfffff;
		CLRREG32(A_CPM_CPCCR,pll_cfcr_bypass);
		CLRREG32(A_CPM_CPPSR,CPPSR_CS);//cs=0

		
		
		OUTREG32(A_EMC_RTCOR , flushcount);
		OUTREG32(A_EMC_RTCNT , flushcount);
		SETREG32(A_CPM_CPPCR,CPPCR_PLLBP);//bp=1
		while (!(INREG32(A_CPM_CPPSR )& CPPSR_CS));//wait cs=1
		OUTREG32(A_EMC_DMCR,dmcr);
		
}
static inline void clr_bypass(unsigned int dmcr,unsigned int flushcount)
{
		   unsigned int prog_entry = ((unsigned int)clr_bypass / 32 - 1) * 32 ;
       unsigned int prog_size = 512;
       unsigned int i;
       for( i = (prog_entry);i < prog_entry + prog_size; i += 32)
					__asm__ __volatile__(
						"cache 0x1c, 0x00(%0)\n\t"
			            :
						: "r" (i));
		
		CLRREG32(A_CPM_CPPSR,CPPSR_CS);//cs=0
		OUTREG32(A_EMC_DMCR,dmcr);
		CLRREG32(A_CPM_CPPCR,CPPCR_PLLBP);//bp=0
		while (!(INREG32(A_CPM_CPPSR)& CPPSR_CS))printf("%x\n",INREG32(A_CPM_CPPSR));//wait cs=1		
		OUTREG32(A_EMC_RTCOR , flushcount);
		OUTREG32(A_EMC_RTCNT , flushcount);
		
}
#if 0
static int jz_pllcovert(PPLL_OPT pllopt)
{
}
#endif
#if 1
#define DETECT_LCD_EOF 0
int jz_pm_sleep(void);
static int jz_pllcovert(PPLL_OPT pllopt)
{
	unsigned int pllout,sdram_freq;
	unsigned int pll_cfcr,pll_plcr1;
	unsigned int t;
	unsigned int dmcr;
	static PLL_OPT opt_pll_bypass = {OSC_CLOCK,  1,     1,			  1,    1,    1};
	//printf("jz_pllcovert = %d %d\n",pllopt->cpuclock,initpll_opt->cpuclock);
	//printf("%x %x\n",INREG32(A_CPM_CPCCR),INREG32(A_CPM_CPPCR));
	if(pllopt->cpuclock == 0)
	{
		jz_pm_sleep();
		return 1;
	}
	if(initpll_opt->cpuclock == pllopt->cpuclock)
	{
		if(initpll_opt != pllopt)
			jz_divcovert(pllopt);
		initpll_opt = pllopt;	
		return 1;	
	}
	if(initpll_opt->cpuclock == MP3_CPU_SPEED)
	{
		restore_gpio_regs2();
		CLRREG32(A_EMC_DMCR, DMCR_POWER_DOWN);
	}
	if(pllopt->cpuclock == MP3_CPU_SPEED)
	{
		save_gpio_regs2();
		config_gpio_savepower2();	
		SETREG32(A_EMC_DMCR, DMCR_POWER_DOWN);	
	}	  
	initpll_opt = pllopt;	
	//printf("*************************************\n");
	 pll_convert(pllopt,&pll_cfcr,&pll_plcr1);
	 
	 if((pll_plcr1 & 0xffff0000) == (INREG32(A_CPM_CPPCR) & 0xffff0000))
	 	return 1;
	 pll_plcr1 |= CPPCR_PLLBP;	 
	 pll_cfcr &= ~CPCCR_CHANGE_EN; 
	 unsigned int dmcr1 = INREG32(A_EMC_DMCR) & (~(0xfffc));
	 
   dmcr = dmcr1 | get_sdram_configure(pllopt->cpuclock / pllopt->mdiv,&sdram_freq);
	
	 //printf("%x %x %x\n",pll_cfcr,pll_plcr1,sdram_freq);
	 
	 unsigned int sdram_freq_bypass;
	 unsigned int dmcr_bypass = dmcr1 | get_sdram_configure(OSC_CLOCK,&sdram_freq_bypass);
  
    //printf("24m?\n");
  	//printf("dmcr = 0x%08x 24:0x%08x old: 0x%08x\n",dmcr,dmcr_bypass,INREG32(A_EMC_DMCR));
		//lcd_stop();
		//lcd_close(LCD_NORMAL_CLOSE);
		dm_preconvert_all(&opt_pll_bypass);

//----------------------------------------------------------------------------  		
		//1. bypass the pll
		CLRREG32(A_CPM_CPCCR,CPCCR_CHANGE_EN);
		t = read_c0_status();
    write_c0_status(t & (~1));
#if DETECT_LCD_EOF		
		CLRREG8(A_LCD_LCDSTATE, LCD_STATE_EOF);
		while(!(INREG32(A_LCD_LCDSTATE) & LCD_STATE_EOF));
#endif
//----------------------------------------------------------------------------  		
    
    set_bypass(dmcr_bypass,sdram_freq_bypass);
		
		SETREG32(A_CPM_CPCCR,CPCCR_CHANGE_EN);
		write_c0_status(t);
		dm_convert_all();
		
//----------------------------------------------------------------------------
		
		//printf("%x %x %x %x\n",INREG32(A_CPM_CPCCR),INREG32(A_CPM_CPPCR),pll_cfcr,pll_plcr1);
	  //printf("convert? %d\n",lcddiv_bypass);
	  
	  //2. change the pll and div	
		CLRREG32(A_CPM_CPCCR,CPCCR_CHANGE_EN);//ce=0		
	
		CLRREG32(A_CPM_CPPSR,CPPSR_PS);//ps=0	
		OUTREG32(A_CPM_CPCCR,pll_cfcr);//update div
		OUTREG32(A_CPM_CPPCR,pll_plcr1);//update pll
		
		while (!(INREG32(A_CPM_CPPSR )& CPPSR_PS));//wait ps=1
		
		//printf("un24?\n");
//----------------------------------------------------------------------------		
		dm_preconvert_all(pllopt);
		//3. undo bypass 
		t = read_c0_status();
//---------------------------------------------------------------------------- 
#if DETECT_LCD_EOF		 		
		CLRREG8(A_LCD_LCDSTATE, LCD_STATE_EOF);
		while(!(INREG32(A_LCD_LCDSTATE) & LCD_STATE_EOF));
#endif
//---------------------------------------------------------------------------- 
    write_c0_status(t & (~1));
		clr_bypass(dmcr,sdram_freq);
		
		SETREG32(A_CPM_CPCCR,CPCCR_CHANGE_EN);
		write_c0_status(t);
		dm_convert_all();
		//lcd_open();
		//lcd_start();
//----------------------------------------------------------------------------		
    pllout = GetCurrentPLLClock(0);
    printf("dmcr = 0x%x,sdram_freq = 0x%x",dmcr,sdram_freq);
    printf("pll out new: %dMHZ 0x%08x \n",pllout/1000000,INREG32(A_CPM_CPCCR)); 
    //while(1);
    return 0;
}
#endif
static void pll_bypass(void)
{
	unsigned int freq;
     /* sdram convert */
    
	get_sdram_configure(OSC_CLOCK,&freq);
	OUTREG32(A_EMC_RTCOR , freq);
	OUTREG32(A_EMC_RTCNT , freq);
	SETREG32(A_CPM_CPPCR , CPPCR_PLLBP);
	OUTREG32(A_CPM_CPCCR , ((INREG32(A_CPM_CPCCR) & (~0xffff)) | CPCCR_CHANGE_EN));
	CLRREG32(A_CPM_CPCCR , CPCCR_CHANGE_EN);
       

}

static void pll_convert(PPLL_OPT pllopt,unsigned int *pll_cfcr,unsigned int *pll_plcr1)
{
	unsigned int cfcr, plcr1;

	//printf("cpuspeed  memspeed: (%dM :%dM)\n",pllopt->cpuclock / pllopt->cdiv / 1000 / 1000,pllopt->cpuclock / pllopt->mdiv / 1000 / 1000);
	int cdiv = find_cpudiv(pllopt->cdiv);
	#if (RELEASE==0)
	if(cdiv == -1)
	{
		printf("error: init pll cdiv!");
		return;
	}
	#endif
	
	int mdiv = find_cpudiv(pllopt->mdiv);
	#if (RELEASE==0)
	if(mdiv == -1)
	{
		printf("error: init pll mdiv!");	
		return;	
	}
	#endif
	
	int h0div = find_cpudiv(pllopt->h0div);
	#if (RELEASE==0)
	if(h0div == -1)
	{
		printf("error: init pll =h0div!");	
		return;	
	}
	#endif
	
	int h1div = find_cpudiv(pllopt->h1div);
	#if (RELEASE==0)
	if(h1div == -1)
	{
		printf("error: init pll h1div!");	
		return;	
	}
	#endif
	
	int pdiv = find_cpudiv(pllopt->pdiv);
	#if (RELEASE==0)
	if(pdiv == -1)
	{
		printf("error: init pll pdiv!");	
		return;	
	}
	#endif
	cfcr = INREG32(A_CPM_CPCCR);
	cfcr &= ~(0xfffff);
	
	cfcr |= (h1div << CPCCR_H1DIV_BIT) |
					(mdiv << CPCCR_MDIV_BIT)  | 
					(pdiv << CPCCR_PDIV_BIT)  | 
					(h0div << CPCCR_H0DIV_BIT) |
					(cdiv << CPCCR_CDIV_BIT) ;

	/* init PLL */
	*pll_cfcr = cfcr;
	
	
	unsigned int nf = pllopt->cpuclock * 2 / OSC_CLOCK;
		plcr1 = ((nf - 2) << CPPCR_PLLM_BIT) | /* FD */
		(0 << CPPCR_PLLN_BIT) |	/* RD=0, NR=2 */
		(0 << CPPCR_PLLOD_BIT) |    /* OD=0, NO=1 */
		(0x11 << CPPCR_PLLST_BIT)|  /* PLL stable time */
		CPPCR_PLLEN;                /* enable PLL */
	*pll_plcr1=plcr1;
	
}

/*static void set_pll(unsigned int div,unsigned int pll)
{

       u32 prog_entry = ((u32)set_pll / 32 - 1) * 32 ;
       u32 prog_size = 1024;
       u32 i;

       for( i = (prog_entry);i < prog_entry + prog_size; i += 32)
		__asm__ __volatile__(
			"cache 0x1c, 0x00(%0)\n\t"
            :
			: "r" (i));
	REG_CPM_CPCCR = div;
//	REG_CPM_CPCCR |= CPM_CPCCR_CE;
	REG_CPM_CPPCR = pll;
	while (!(REG_CPM_CPPCR & CPM_CPPCR_PLLS));

}*/

/* convert pll while program is running */
int jz_pm_pllconvert(PPLL_OPT pllin)
{
	return jz_pm_do_pllconvert(pllin);
}

//--------------------------------------------------------------------

/* Put CPU to IDLE mode */
void jz_pm_idle(void)
{
	__asm__(
        ".set\tmips3\n\t"
        "sync\n\t"
        "wait\n\t"
        "nop\n\t"
        "nop\n\t"
		".set\tmips0"
        );
		//cpu_wait();
}
//--------------------------------------------------------
static int curLevel = 0;

void pm_init()
{
#if (DM==1)
	printf("Driver Manager \r\n");
	jz_dm_init();
#endif
}
/*
    	level:
    	0,1,2,3,-1;
    	-1 is GetLevel;
*/
#define OP_PM_LOCK()
#define OP_PM_UNLOCK()	
int jz_pm_control(int level)
{
	int ret = 0;
	OP_PM_LOCK();
	if(level != -1)
	{	
		if(curLevel != level)
		{
			
			if(level<0 || level > sizeof(opt_pll)/sizeof(PLL_OPT) - 1)
			{	
				OP_PM_UNLOCK();
				return -1;
			}
			{
					 ret = 1;
					 jz_pm_pllconvert(&opt_pll[level]);
			}
			if(ret)
				curLevel = level;
			OP_PM_UNLOCK();
			return ret;
	  }
	}else
	{
		OP_PM_UNLOCK();
		return curLevel;
	}
	OP_PM_UNLOCK();
	return 0;
}
//--------------------------------------------------------------------

typedef struct __tag_wake_type__
{
	void (*WakeUp)();
	void (*PreWakeUp)();
	unsigned int id; 
}WakeUpType;
#define MAX_WakeUp_SOURCE 5
static WakeUpType WakeUpsource[MAX_WakeUp_SOURCE] = {0};
static unsigned char wcount = 0;
 
void PM_AddWakeUp(int id,void* preWakeUp,void* WakeUp)
{
	if(wcount < MAX_WakeUp_SOURCE)
	{
		WakeUpsource[wcount].id = id;
		WakeUpsource[wcount].PreWakeUp = preWakeUp;
		WakeUpsource[wcount].WakeUp = WakeUp;
		wcount++;
	}
}
static void PM_SetWakeUp()
{
	int i;
	for(i = 0;i < wcount;i++)
	{
		if(WakeUpsource[i].PreWakeUp)
		{
			printf("%s->%x\n",__FUNCTION__,WakeUpsource[i].id);
			WakeUpsource[i].PreWakeUp();
		}
	}
}
static void PM_WakeUp()
{
	int i;
	for(i = 0;i < wcount;i++)
	{
		if(WakeUpsource[i].WakeUp)
		{	
			printf("%s->%x\n",__FUNCTION__,WakeUpsource[i].id);
			WakeUpsource[i].WakeUp();
		}
	}
}
//--------------------------------------------------------------------
/* Put CPU to SLEEP mode */
int jz_pm_sleep(void)
{
	 PM_SetWakeUp();
	 jz_pm_do_sleep();
	 PM_WakeUp();
	 return 1;
}
//---------------------------------------------------------------------
//--------------------------------------------------------------------
typedef struct __tag_power_type__
{
	void (*pPowerDown)();
	void (*pPowerUp)();
	unsigned int id; 
}PowerType;
#define MAX_Power_SOURCE 10
static PowerType PowerSource[MAX_Power_SOURCE] = {0};
static unsigned char w_powercount = 0;


void PM_AddPowerCtrl(int id,void* ppowerdown,void* ppowerup)
{
	if(w_powercount < MAX_Power_SOURCE)
	{
		PowerSource[w_powercount].id = id;
		PowerSource[w_powercount].pPowerDown = ppowerdown;
		PowerSource[w_powercount].pPowerUp = ppowerup;
		w_powercount++;
	}
}
static void PM_PowerDown()
{
	int i;
	for(i = 0;i < w_powercount;i++)
	{
		if(PowerSource[i].pPowerDown)
		{
			printf("%s->%x\n",__FUNCTION__,PowerSource[i].id);
			PowerSource[i].pPowerDown();
		}
	}
}
static void PM_PowerUp()
{
	int i;
	for(i = 0;i < w_powercount;i++)
	{
		if(PowerSource[i].pPowerUp)
		{	
			printf("%s->%x\n",__FUNCTION__,PowerSource[i].id);
			PowerSource[i].pPowerUp();
		}
	}
}

/* Put CPU to HIBERNATE mode */
int jz_pm_hibernate(void)
{
	PM_PowerDown();
	
	codec_reg_set(A_CODEC_CR1, DAC_MUTE);
	codec_reg_write(A_CODEC_CGR8, 0xf);
	codec_reg_write(A_CODEC_CGR9, 0xf);
	codec_reg_write(A_CODEC_CGR1, 0xff);
	codec_reg_write(A_CODEC_CGR10, 0);
	mdelay(50);
	codec_reg_set(A_CODEC_PMR1, SB_OUT);
	mdelay(100);
	codec_reg_set(A_CODEC_PMR2, SB | SB_SLEEP);
	//os_TimeDelay(30);
	mdelay(10);
	codec_reg_set(A_CODEC_PMR1, SB_MIX | SB_DAC | SB_ADC);	
	//SETREG32(A_CPM_CLKGR, CLKGR_STOP_AIC_PCLK);
	return jz_pm_do_hibernate();
}
