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
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn> 
 *
 *  Create:   2008-06-26, by dsqiu
 *            
 *  Maintain: 2008-06-26, by jgao
 *            
 *
 *******************************************************************************
 */
 

#if (DM==1)
#include <jz4755.h>
#include <bsp.h>
#include <os_api.h>
struct pll_opt
{
	unsigned int cpuclock;
	int div;
};
static struct pll_opt opt_pll[4];
static int currentlevel;
void  StatHookInit (void)
{
	unsigned int pllout;
	opt_pll[0].cpuclock= CFG_CPU_SPEED/4;
	opt_pll[0].div=3;
	opt_pll[1].cpuclock=(CFG_CPU_SPEED*2)/4;
	opt_pll[1].div=2;
	opt_pll[2].cpuclock=(CFG_CPU_SPEED*3)/4;
	opt_pll[2].div=3;
	opt_pll[3].cpuclock=CFG_CPU_SPEED;
	opt_pll[3].div=3;

	//pllout = (__cpm_get_pllm() + 2)* OSC_CLOCK / (__cpm_get_plln() + 2);
	pllout = (((INREG32(A_CPM_CPPCR)&CPPCR_PLLM_MASK)>>CPPCR_PLLM_BIT) + 2)* OSC_CLOCK / (((INREG32(A_CPM_CPPCR)&CPPCR_PLLN_MASK)>>CPPCR_PLLN_BIT) + 2);
	if(pllout<opt_pll[0].cpuclock)
	{
		currentlevel=0;
	}
	if(pllout>opt_pll[0].cpuclock && pllout<opt_pll[1].cpuclock)
	{
		currentlevel=1;
	}
	if(pllout>opt_pll[1].cpuclock && pllout<opt_pll[2].cpuclock)
	{
		currentlevel=2;
	}
	if(pllout>opt_pll[2].cpuclock)
	{
		currentlevel=3;
	}

}
int count_sec=0;

int OSTaskStatHook_jz4755 (void)
{
#if 0
	count_sec=count_sec+1;
	if(count_sec>10)
	{
	printf("hook22222 %d\n",OSCPUUsage);
		if(OSCPUUsage > 76)
		{
			if(currentlevel<3)
			{
				currentlevel=currentlevel+1;
				jz_pm_pllconvert(opt_pll[currentlevel].cpuclock,opt_pll[currentlevel].div);
			}
		}
		if(OSCPUUsage < 70)
		{
			if(currentlevel>0)
			{
				currentlevel=currentlevel-1;
				jz_pm_pllconvert(opt_pll[currentlevel].cpuclock,opt_pll[currentlevel].div);
			}
		}
		count_sec=0;
	}
#endif

}

int OSTaskIdleHook_jz4755()
{
	jz_pm_idle();
}
OSBTaskBSPHook jz4755Bsp = {
	0,
	0,
	0,
	0,
	OSTaskIdleHook_jz4755,
	OSTaskStatHook_jz4755,
	0,
	0,
	0,
	0
};
#endif
