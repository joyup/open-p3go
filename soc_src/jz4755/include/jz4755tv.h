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
 *  Create:   2008-09-23, by jbyu
 *
 *******************************************************************************
 */

#ifndef __JZ4755TV_H__
#define __JZ4755TV_H__

//--------------------------------------------------------------------------
// TV Controller(TV) Address Definition
//--------------------------------------------------------------------------
#define TV_PHYS_ADDR			( 0x13050000 )
#define TV_BASE_U_VIRTUAL		( 0xB3050000 )

//--------------------------------------------------------------------------
// TV Registers Offset Definition
//--------------------------------------------------------------------------
#define TV_TVECR_OFFSET			(0x140)		//RW, 32
#define TV_FRCFG_OFFSET			(0x144)		//RW, 32
#define TV_SLCFG1_OFFSET		(0x150)		//RW, 32
#define TV_SLCFG2_OFFSET		(0x154)		//RW, 32
#define TV_SLCFG3_OFFSET		(0x158)		//RW, 32
#define TV_LTCFG1_OFFSET		(0x160)		//RW, 32
#define TV_LTCFG2_OFFSET		(0x164)		//RW, 32
#define TV_CFREQ_OFFSET			(0x170)		//RW, 32
#define TV_CPHASE_OFFSET		(0x174)		//RW, 32
#define TV_CCFG_OFFSET			(0x178)		//RW, 32
#define TV_WSSCR_OFFSET			(0x180)		//RW, 32
#define TV_WSSCFG1_OFFSET		(0x184)		//RW, 32
#define TV_WSSCFG2_OFFSET		(0x188)		//RW, 32
#define TV_WSSCFG3_OFFSET		(0x18C)		//RW, 32

//--------------------------------------------------------------------------
// TV Registers Address Definition
//--------------------------------------------------------------------------
#define A_TV_TVECR				(TV_BASE_U_VIRTUAL + TV_TVECR_OFFSET)
#define A_TV_FRCFG				(TV_BASE_U_VIRTUAL + TV_FRCFG_OFFSET)
#define A_TV_SLCFG1				(TV_BASE_U_VIRTUAL + TV_SLCFG1_OFFSET)
#define A_TV_SLCFG2				(TV_BASE_U_VIRTUAL + TV_SLCFG2_OFFSET)
#define A_TV_SLCFG3				(TV_BASE_U_VIRTUAL + TV_SLCFG3_OFFSET)
#define A_TV_LTCFG1				(TV_BASE_U_VIRTUAL + TV_LTCFG1_OFFSET)
#define A_TV_LTCFG2				(TV_BASE_U_VIRTUAL + TV_LTCFG2_OFFSET)
#define A_TV_CFREQ				(TV_BASE_U_VIRTUAL + TV_CFREQ_OFFSET)
#define A_TV_CPHASE				(TV_BASE_U_VIRTUAL + TV_CPHASE_OFFSET)
#define A_TV_CCFG				(TV_BASE_U_VIRTUAL + TV_CCFG_OFFSET)
#define A_TV_WSSCR				(TV_BASE_U_VIRTUAL + TV_WSSCR_OFFSET)
#define A_TV_WSSCFG1			(TV_BASE_U_VIRTUAL + TV_WSSCFG1_OFFSET)
#define A_TV_WSSCFG2			(TV_BASE_U_VIRTUAL + TV_WSSCFG2_OFFSET)
#define A_TV_WSSCFG3			(TV_BASE_U_VIRTUAL + TV_WSSCFG3_OFFSET)

//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------

// TV Encoder Control Register (TVECR)
#define TV_TVECR_ECVBS          	(1 << 24)    // cvbs_enable
#define TV_TVECR_DAPD3	        	(1 << 23)    // DAC 3 power down, not exist in jz4755
#define TV_TVECR_DAPD2	        	(1 << 22)    // DAC 2 power down
#define TV_TVECR_DAPD1	        	(1 << 21)    // DAC 1 power down
#define TV_TVECR_DAPD           	(1 << 20)    // power down all DACs
#define TV_TVECR_YCDLY_BIT      	16
#define TV_TVECR_YCDLY_MASK     	(0x7 << TV_TVECR_YCDLY_BIT)
#define TV_TVECR_CGAIN_BIT      	14
#define TV_TVECR_CGAIN_MASK     	(0x3 << TV_TVECR_CGAIN_BIT)
#define TV_TVECR_CGAIN_FULL			(0 << TV_TVECR_CGAIN_BIT) // gain = 1
#define TV_TVECR_CGAIN_QUTR			(1 << TV_TVECR_CGAIN_BIT) // gain = 1/4
#define TV_TVECR_CGAIN_HALF			(2 << TV_TVECR_CGAIN_BIT) // gain = 1/2
#define TV_TVECR_CGAIN_THREE_QURT	(3 << TV_TVECR_CGAIN_BIT) // gain = 3/4
#define TV_TVECR_CBW_BIT        	12
#define TV_TVECR_CBW_MASK       	(0x3 << TV_TVECR_CBW_BIT)
#define TV_TVECR_CBW_NARROW			(0 << TV_TVECR_CBW_BIT) // Narrow band
#define TV_TVECR_CBW_WIDE			(1 << TV_TVECR_CBW_BIT) // Wide band
#define TV_TVECR_CBW_EXTRA			(2 << TV_TVECR_CBW_BIT) // Extra wide band
#define TV_TVECR_CBW_ULTRA			(3 << TV_TVECR_CBW_BIT) // Ultra wide band
#define TV_TVECR_SYNCT          	(1 << 9)
#define TV_TVECR_PAL            	(1 << 8)
#define TV_TVECR_NTSC           	(0 << 8)
#define TV_TVECR_FINV           	(1 << 7) // invert_top:1-invert top and bottom fields.
#define TV_TVECR_ZBLACK         	(1 << 6) // bypass_yclamp:1-Black of luminance (Y) input is 0.
#define TV_TVECR_CR1ST          	(1 << 5) // uv_order:0-Cb before Cr,1-Cr before Cb
#define TV_TVECR_CLBAR          	(1 << 4) // Color bar mode:0-Output input video to TV,1-Output color bar to TV
#define TV_TVECR_SWRST          	(1 << 0) // Software reset:1-TVE is reset


// Signal level configure register 1
#define TV_SLCFG1_BLACKL_BIT   		0
#define TV_SLCFG1_BLACKL_MASK  		(0x3ff << TV_SLCFG1_BLACKL_BIT)
#define TV_SLCFG1_WHITEL_BIT   		16
#define TV_SLCFG1_WHITEL_MASK  		(0x3ff << TV_SLCFG1_WHITEL_BIT)

// Signal level configure register 2
#define TV_SLCFG2_BLANKL_BIT    	0
#define TV_SLCFG2_BLANKL_MASK   	(0x3ff << TV_SLCFG2_BLANKL_BIT)
#define TV_SLCFG2_VBLANKL_BIT   	16
#define TV_SLCFG2_VBLANKL_MASK  	(0x3ff << TV_SLCFG2_VBLANKL_BIT)

// Signal level configure register 3
#define TV_SLCFG3_SYNCL_BIT   		0
#define TV_SLCFG3_SYNCL_MASK  		(0xff << TV_SLCFG3_SYNCL_BIT)

// Line timing configure register 1
#define TV_LTCFG1_BACKP_BIT   		0
#define TV_LTCFG1_BACKP_MASK  		(0x7f << TV_LTCFG1_BACKP_BIT)
#define TV_LTCFG1_HSYNCW_BIT  		8
#define TV_LTCFG1_HSYNCW_MASK 		(0x7f << TV_LTCFG1_HSYNCW_BIT)
#define TV_LTCFG1_FRONTP_BIT  		16
#define TV_LTCFG1_FRONTP_MASK 		(0x1f << TV_LTCFG1_FRONTP_BIT)

// Line timing configure register 2
#define TV_LTCFG2_BURSTW_BIT    	0
#define TV_LTCFG2_BURSTW_MASK   	(0x3f << TV_LTCFG2_BURSTW_BIT)
#define TV_LTCFG2_PREBW_BIT     	8
#define TV_LTCFG2_PREBW_MASK    	(0x1f << TV_LTCFG2_PREBW_BIT)
#define TV_LTCFG2_ACTLIN_BIT    	16
#define TV_LTCFG2_ACTLIN_MASK		(0x7ff << TV_LTCFG2_ACTLIN_BIT)

// Chrominance sub-carrier phase configure register
#define TV_CPHASE_CCRSTP_BIT    	0
#define TV_CPHASE_CCRSTP_MASK   	(0x3 << TV_CPHASE_CCRSTP_BIT)
#define TV_CPHASE_CCRSTP_8			(0 << TV_CPHASE_CCRSTP_BIT) 	// Every 8 field
#define TV_CPHASE_CCRSTP_4			(1 << TV_CPHASE_CCRSTP_BIT) 	// Every 4 field
#define TV_CPHASE_CCRSTP_2			(2 << TV_CPHASE_CCRSTP_BIT) 	// Every 2 lines
#define TV_CPHASE_CCRSTP_0			(3 << TV_CPHASE_CCRSTP_BIT) 	// Never
#define TV_CPHASE_ACTPH_BIT     	16
#define TV_CPHASE_ACTPH_MASK    	(0xff << TV_CPHASE_ACTPH_BIT)
#define TV_CPHASE_INITPH_BIT    	24
#define TV_CPHASE_INITPH_MASK   	(0xff << TV_CPHASE_INITPH_BIT)

// Chrominance filter configure register
#define TV_CBCRCFG_CRGAIN_BIT       0
#define TV_CBCRCFG_CRGAIN_MASK      (0xff << TV_CBCRCFG_CRGAIN_BIT)
#define TV_CBCRCFG_CBGAIN_BIT       8
#define TV_CBCRCFG_CBGAIN_MASK      (0xff << TV_CBCRCFG_CBGAIN_BIT)
#define TV_CBCRCFG_CRBA_BIT         16
#define TV_CBCRCFG_CRBA_MASK        (0xff << TV_CBCRCFG_CRBA_BIT)
#define TV_CBCRCFG_CBBA_BIT         24
#define TV_CBCRCFG_CBBA_MASK        (0xff << TV_CBCRCFG_CBBA_BIT)

// Frame configure register
#define TV_FRCFG_NLINE_BIT          0
#define TV_FRCFG_NLINE_MASK         (0x3ff << TV_FRCFG_NLINE_BIT)
#define TV_FRCFG_L1ST_BIT           16
#define TV_FRCFG_L1ST_MASK          (0xff << TV_FRCFG_L1ST_BIT)

// Wide screen signal control register
#define TV_WSSCR_EWSS0_BIT			0
#define TV_WSSCR_EWSS1_BIT			1
#define TV_WSSCR_WSSTP_BIT			2
#define TV_WSSCR_WSSCKBP_BIT		3
#define TV_WSSCR_WSSEDGE_BIT		4
#define TV_WSSCR_WSSEDGE_MASK		(0x7 << TV_WSSCR_WSSEDGE_BIT)
#define TV_WSSCR_ENCH_BIT			8
#define TV_WSSCR_NCHW_BIT			9
#define TV_WSSCR_NCHFREQ_BIT		12
#define TV_WSSCR_NCHFREQ_MASK		(0x7 << TV_WSSCR_NCHFREQ_BIT)

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	TVRSV0[80];		// 0x00 -0x13C
	unsigned int	TVECR;			// 0x140
	unsigned int	FRCFG;			// 0x144
	unsigned int	TVRSV1[2];		// 0x148, 0x14C
	unsigned int	SLCFG1;			// 0x150
	unsigned int	SLCFG2;			// 0x154
	unsigned int	SLCFG3;			// 0x158
	unsigned int	TVRSV2[1];		// 0x15C
	unsigned int	LTCFG1;			// 0x160
	unsigned int	LTCFG2;			// 0x164
	unsigned int	TVRSV3[2];		// 0x168, 0x16C
	unsigned int	CFREQ;			// 0x170
	unsigned int	CPHASE;			// 0x174
	unsigned int	CCFG;			// 0x178
	unsigned int	TVRSV4[1];		// 0x17C
	unsigned int	WSSCR;			// 0x180
	unsigned int	WSSCFG1;		// 0x184
	unsigned int	WSSCFG2;		// 0x188
	unsigned int	WSSCFG3;		// 0x18C

} JZ4755_TV, *PJZ4755_TV;

struct jz4755tve_info
{
	unsigned int 	ctrl;
	unsigned int 	frcfg;
	unsigned int 	slcfg1;
	unsigned int 	slcfg2;
	unsigned int 	slcfg3;
	unsigned int 	ltcfg1;
	unsigned int 	ltcfg2;
	unsigned int 	cfreq;
	unsigned int 	cphase;
	unsigned int 	ccfg;
	unsigned int 	wsscr;
	unsigned int 	wsscfg1;
	unsigned int 	wsscfg2;
	unsigned int 	wsscfg3;
};

#endif //__MIPS_ASSEMBLER

#endif // __JZ4755TV_H__
