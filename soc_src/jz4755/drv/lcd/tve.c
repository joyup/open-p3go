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
 *  Author:  <jbyu@ingenic.cn>
 *
 *	Create: 2008-11-01, by jbyu
 *
 *******************************************************************************
 */

#include <jz4755.h>
#include <lcd_app.h>
#include "lcdc.h"

//#define CFG_CPU_SPEED_FOR_TVE	(324000000
//#define CFG_CPU_SPEED_FOR_TVE	(378000000)
#define CFG_CPU_SPEED_FOR_TVE	(432000000)
#define TV_SPEED_CLCK			(27000000)
#if (CFG_CPU_SPEED_FOR_TVE != 378000000)
#define	CPPCR_CFG_FOR_TVE		( ((CFG_CPU_SPEED_FOR_TVE / (OSC_CLOCK / 2) - 2) << CPPCR_PLLM_BIT)	\
								| (0x20 << CPPCR_PLLST_BIT)											\
								| (CPPCR_PLLEN) )
#else
#define	CPPCR_CFG_FOR_TVE		(( 61 << CPPCR_PLLM_BIT)		\
								| (2 << CPPCR_PLLN_BIT)		\
								| (0x20 << CPPCR_PLLST_BIT)	\
								| (CPPCR_PLLEN) )
#endif

// TVECR bit25 is the YUV div control signal, only change this bit could use this function
static struct jz4755tve_info jz4755_tve_info_PAL =
{
#if (TV_VIDEO_OUT_MODE == TV_OUT_MODE_CVBS)
	.ctrl    = (4 << TV_TVECR_YCDLY_BIT) | TV_TVECR_SYNCT | TV_TVECR_PAL | TV_TVECR_SWRST | TV_TVECR_ECVBS | (0 << 4), // PAL, SVIDEO
#elif (TV_VIDEO_OUT_MODE == TV_OUT_MODE_3DIV)
	.ctrl    = (4 << TV_TVECR_YCDLY_BIT) | TV_TVECR_SYNCT | TV_TVECR_PAL | TV_TVECR_SWRST | (0 << 4) | (1 << 25),      // PAL, 3DIV
#else
	.ctrl    = (4 << TV_TVECR_YCDLY_BIT) | TV_TVECR_SYNCT | TV_TVECR_PAL | TV_TVECR_SWRST | (0 << 4), // PAL, SVIDEO
#endif
	.frcfg   = (23 << TV_FRCFG_L1ST_BIT) | (625 << TV_FRCFG_NLINE_BIT),
	.slcfg1  = (528 << TV_SLCFG1_WHITEL_BIT) | (272 << TV_SLCFG1_BLACKL_BIT),
	.slcfg2  = (296 << TV_SLCFG2_VBLANKL_BIT) | (240 << TV_SLCFG2_BLANKL_BIT),
	.slcfg3  = (72 << TV_SLCFG3_SYNCL_BIT),
	.ltcfg1  = (20 << TV_LTCFG1_FRONTP_BIT) | (63 << TV_LTCFG1_HSYNCW_BIT) | (78 << TV_LTCFG1_BACKP_BIT),
	.ltcfg2  = (1440 << TV_LTCFG2_ACTLIN_BIT) | (24 << TV_LTCFG2_PREBW_BIT) | (68 << TV_LTCFG2_BURSTW_BIT),
	.cfreq   = 0x2a098800,
	.cphase  = (0 << TV_CPHASE_INITPH_BIT) | (0 << TV_CPHASE_ACTPH_BIT) | (1 << TV_CPHASE_CCRSTP_BIT),
#if (TV_VIDEO_OUT_MODE == TV_OUT_MODE_CVBS)
	.ccfg    = (30 << TV_CBCRCFG_CBBA_BIT) | (33 << TV_CBCRCFG_CRBA_BIT) | (100  << TV_CBCRCFG_CBGAIN_BIT) | (100 << TV_CBCRCFG_CRGAIN_BIT), // CBGAIN CRGAIN
#else
	//.ccfg    = (32 << TV_CBCRCFG_CBBA_BIT) | (59 << TV_CBCRCFG_CRBA_BIT) | (137 << TV_CBCRCFG_CBGAIN_BIT) | (131 << TV_CBCRCFG_CRGAIN_BIT), // CBGAIN CRGAIN
	.ccfg    = (32 << TV_CBCRCFG_CBBA_BIT) | (59 << TV_CBCRCFG_CRBA_BIT) | (100 << TV_CBCRCFG_CBGAIN_BIT) | (100 << TV_CBCRCFG_CRGAIN_BIT), // CBGAIN CRGAIN
#endif
	.wsscr   = 0x00000070,	// default value
	.wsscfg1 = 0x0,
	.wsscfg2 = 0x0,
	.wsscfg3 = 0x0,
};

static struct jz4755tve_info jz4755_tve_info_NTSC =
{
#if (TV_VIDEO_OUT_MODE == TV_OUT_MODE_CVBS)
	.ctrl    = (4 << TV_TVECR_YCDLY_BIT)| TV_TVECR_NTSC  | TV_TVECR_SWRST | TV_TVECR_ECVBS,	// NTSC, SVIDEO
#elif (TV_VIDEO_OUT_MODE == TV_OUT_MODE_3DIV)
	.ctrl    = (4 << TV_TVECR_YCDLY_BIT)| TV_TVECR_NTSC  | TV_TVECR_SWRST | (1 << 25),	// NTSC, 3DIV
#else
	.ctrl    = (4 << TV_TVECR_YCDLY_BIT)| TV_TVECR_NTSC  | TV_TVECR_SWRST,	// NTSC, SVIDEO
#endif
	.frcfg   = (21 << TV_FRCFG_L1ST_BIT) | (525 << TV_FRCFG_NLINE_BIT),
	.slcfg1  = (528 << TV_SLCFG1_WHITEL_BIT) | (272 << TV_SLCFG1_BLACKL_BIT),
	.slcfg2  = (296 << TV_SLCFG2_VBLANKL_BIT) | (240 << TV_SLCFG2_BLANKL_BIT),
	.slcfg3  = (72 << TV_SLCFG3_SYNCL_BIT),
	.ltcfg1  = (16 << TV_LTCFG1_FRONTP_BIT) | (63 << TV_LTCFG1_HSYNCW_BIT) | (59 << TV_LTCFG1_BACKP_BIT),
	.ltcfg2  = (1440 << TV_LTCFG2_ACTLIN_BIT) | (22 << TV_LTCFG2_PREBW_BIT) | (68 << TV_LTCFG2_BURSTW_BIT),
	.cfreq   = 0x21f03C1f,
	.cphase  = (0x17 << TV_CPHASE_INITPH_BIT) | (0 << TV_CPHASE_ACTPH_BIT) | (1 << TV_CPHASE_CCRSTP_BIT),
	//.ccfg    = (59 << TV_CBCRCFG_CBBA_BIT) | (0 << TV_CBCRCFG_CRBA_BIT) | (137 << TV_CBCRCFG_CBGAIN_BIT) | (131 << TV_CBCRCFG_CRGAIN_BIT),
	.ccfg    = (59 << TV_CBCRCFG_CBBA_BIT) | (0 << TV_CBCRCFG_CRBA_BIT) | (65 << TV_CBCRCFG_CBGAIN_BIT) | (65 << TV_CBCRCFG_CRGAIN_BIT),
	.wsscr   = 0x00000070,	// default value
	.wsscfg1 = 0x0,
	.wsscfg2 = 0x0,
	.wsscfg3 = 0x0,
};

struct jz4755tve_info *jz4755_tve_info = &jz4755_tve_info_NTSC; // default as PAL mode

#if 0
void print_tve(void)
{
	printf("A_TV_TVECR=%x\r\n", INREG32(A_TV_TVECR));
	printf("A_TV_FRCFG=%x\r\n", INREG32(A_TV_FRCFG));
	//printf("####### LCD registers #######\r\n");
	printf("A_TV_SLCFG1=%x\r\n", INREG32(A_TV_SLCFG1));
	printf("A_TV_SLCFG2=%x\r\n", INREG32(A_TV_SLCFG2	));
	printf("A_TV_SLCFG3=%x\r\n", INREG32(A_TV_SLCFG3));
	printf("A_TV_LTCFG1=%x\r\n", INREG32(A_TV_LTCFG1));
	printf("A_TV_LTCFG2=%x\r\n", INREG32(A_TV_LTCFG2));
	printf("A_TV_CFREQ=%x\r\n", INREG32(A_TV_CFREQ));
	printf("A_TV_CPHASE=%x\r\n", INREG32(A_TV_CPHASE));
	printf("A_TV_CCFG=%x\r\n", INREG32(A_TV_CCFG));
	//printf("####### DMA0 registers #######\r\n");
	printf("A_TV_WSSCR=%x\r\n", INREG32(A_TV_WSSCR));
	printf("A_TV_WSSCFG1=%x\r\n", INREG32(A_TV_WSSCFG1));
	printf("A_TV_WSSCFG2=%x\r\n", INREG32(A_TV_WSSCFG2));
	printf("A_TV_WSSCFG3=%x\r\n", INREG32(A_TV_WSSCFG3));
}
#endif

int jz4755tve_open(void)
{
	// enable tve controller, enable DACn
	CLRREG32(A_TV_TVECR, TV_TVECR_SWRST);
	SETREG32(A_TV_TVECR, TV_TVECR_DAPD);
#if (TV_VIDEO_OUT_MODE == TV_OUT_MODE_3DIV)
	CLRREG32(A_TV_TVECR, TV_TVECR_DAPD1 | TV_TVECR_DAPD2 | TV_TVECR_DAPD3);
#else
	CLRREG32(A_TV_TVECR, TV_TVECR_DAPD1 | TV_TVECR_DAPD2);
#endif
}

// turn off TVE, turn off DACn...
int jz4755tve_close(void)
{
	CLRREG32(A_TV_TVECR, TV_TVECR_DAPD);
#if (TV_VIDEO_OUT_MODE == TV_OUT_MODE_3DIV)
	SETREG32(A_TV_TVECR, TV_TVECR_DAPD1 | TV_TVECR_DAPD2| TV_TVECR_DAPD3);
#else
	SETREG32(A_TV_TVECR, TV_TVECR_DAPD1 | TV_TVECR_DAPD2);
#endif
	SETREG32(A_TV_TVECR, TV_TVECR_SWRST);
}

void jz4755tve_set_tve_mode( struct jz4755tve_info *tve )	//ÅäÖÃTVE¼Ä´æÆ÷
{
	OUTREG32(A_TV_TVECR,   tve->ctrl);
	OUTREG32(A_TV_FRCFG,   tve->frcfg);
	OUTREG32(A_TV_SLCFG1,  tve->slcfg1);
	OUTREG32(A_TV_SLCFG2,  tve->slcfg2);
	OUTREG32(A_TV_SLCFG3,  tve->slcfg3);
	OUTREG32(A_TV_LTCFG1,  tve->ltcfg1);
	OUTREG32(A_TV_LTCFG2,  tve->ltcfg2);
	OUTREG32(A_TV_CFREQ,   tve->cfreq);
	OUTREG32(A_TV_CPHASE,  tve->cphase);
	OUTREG32(A_TV_CCFG,    tve->ccfg);
	OUTREG32(A_TV_WSSCR,   tve->wsscr);
	OUTREG32(A_TV_WSSCFG1, tve->wsscfg1);
	OUTREG32(A_TV_WSSCFG2, tve->wsscfg2);
	OUTREG32(A_TV_WSSCFG3, tve->wsscfg3);
}

void jz4755fb_change_clock(void )
{
	unsigned int val = 0;
	unsigned int reg;

	/* Timing setting */
	SETREG32(A_CPM_CLKGR, CLKGR_STOP_LCD);
	SETREG32(A_CPM_CLKGR, CLKGR_STOP_TVE);

	OUTREG32(A_CPM_CPPCR, CPPCR_CFG_FOR_TVE);

	CLRREG32(A_CPM_LPCDR, LPCDR_LSCS);
	SETREG32(A_CPM_LPCDR, LPCDR_LTCS);

//	val = GetCurrentPLLClock(1) / GetCommonClock(CPM_HDIV);
//	reg = (INREG32(A_CPM_CPCCR) & ~CPCCR_LDIV_MASK);
//	OUTREG32(A_CPM_CPCCR, reg | ((val - 1) << (CPCCR_LDIV_BIT)));

	/********* In TVE mode PCLK = 27MHz ***********/
	val = CFG_CPU_SPEED_FOR_TVE / TV_SPEED_CLCK;				// PLLCLK = 324

	OUTREG32(A_CPM_LPCDR, val - 1);
	SETREG32(A_CPM_LPCDR, LPCDR_LTCS);		//	REG_CPM_LPCDR  |= CPM_LPCDR_LTCS;
	SETREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN); //	REG_CPM_CPCCR |= CPM_CPCCR_CE ; /* update divide */

	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_LCD);
	CLRREG32(A_CPM_CLKGR, CLKGR_STOP_TVE);	//Ò»¶¨Òª¿ªÆô°¡

	udelay(1000);
}

int jz4755tve_init( int tve_mode )
{
	switch ( tve_mode )
	{
		case PANEL_MODE_TVE_PAL:
			jz4755_tve_info = &jz4755_tve_info_PAL;
			break;

		case PANEL_MODE_TVE_NTSC:
			jz4755_tve_info = &jz4755_tve_info_NTSC;
			break;
	}

	jz4755tve_set_tve_mode( jz4755_tve_info );		//ÅäÖÃTVE¼Ä´æÆ÷
}

int jz4755tve_deinit(void)
{
	SETREG32(A_CPM_CLKGR, CLKGR_STOP_TVE);
	OUTREG32(A_CPM_CPPCR, CPPCR_CFG);
}

int jz4755tve_ioctl( void *buff, unsigned int cmd )
{
	switch (cmd)
	{
		case IOCTL_TV_CHANGE_CLOCK:
			jz4755fb_change_clock();
			break;

		case IOCTL_TV_SET_PAL_PARAM:
			memcpy(&jz4755_tve_info_PAL, buff, sizeof(struct jz4755tve_info));
			break;

		case IOCTL_TV_SET_NTSC_PARAM:
			memcpy(&jz4755_tve_info_NTSC, buff, sizeof(struct jz4755tve_info));
			break;

		case IOCTL_TV_GET_PAL_PARAM:
			memcpy( buff, &jz4755_tve_info_PAL, sizeof(struct jz4755tve_info));
			break;

		case IOCTL_TV_GET_NTSC_PARAM:
			memcpy( buff, &jz4755_tve_info_NTSC, sizeof(struct jz4755tve_info));
			break;

		case IOCTL_TV_SVIDEO_TO_CVBS:
			jz4755_tve_info_PAL.ctrl |= TV_TVECR_ECVBS;
			jz4755_tve_info_NTSC.ctrl |= TV_TVECR_ECVBS;
			SETREG32(A_TV_TVECR,TV_TVECR_ECVBS);
			break;

		case IOCTL_TV_CVBS_TO_SVIDEO:
			jz4755_tve_info_PAL.ctrl &= ~TV_TVECR_ECVBS;
			jz4755_tve_info_NTSC.ctrl &= ~TV_TVECR_ECVBS;
			CLRREG32(A_TV_TVECR,TV_TVECR_ECVBS);
			break;

		default:
			break;
	}
}





