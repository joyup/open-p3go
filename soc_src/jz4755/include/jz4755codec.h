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
 *  Author:  <hyzhang@ingenic.cn>
 *
 *  Create:   2008-04-10, by hyzhang
 *
 *  Maintain: 2008-04-10, by hyzhang, suppport 4755
 *
 *
 *******************************************************************************
 */

#ifndef __JZ4755CODEC_H__
#define __JZ4755CODEC_H__

/*******************************************************************************
*			I2S CODEC Address Definition
*******************************************************************************/
//Register Name			Address		Function
#define A_CODEC_AICR		0			//0, Audio Interface Control, Software Write: 0F
#define A_CODEC_CR1			1			//1, Control Register 1
#define A_CODEC_CR2			2			//2, Control Register 2
#define A_CODEC_CCR1		3			//3, Control Clock Register 1, Software Write: 00
#define A_CODEC_CCR2		4			//4, Control Clock Register 2
#define A_CODEC_PMR1		5			//5, Power Mode Register 1
#define A_CODEC_PMR2		6			//6, Power Mode Register 2
#define A_CODEC_CRR			7			//7, Control Ramp Register
#define A_CODEC_ICR			8			//8, Interrupt Control Register
#define A_CODEC_IFR			9			//9, Interrupt Flag Register
#define A_CODEC_CGR1		10			//10, Control Gain Register 1
#define A_CODEC_CGR2		11			//11, Control Gain Register 2
#define A_CODEC_CGR3		12			//12, Control Gain Register 3
#define A_CODEC_CGR4		13			//13, Control Gain Register 4
#define A_CODEC_CGR5		14			//14, Control Gain Register 5
#define A_CODEC_CGR6		15			//15, Control Gain Register 6
#define A_CODEC_CGR7		16			//16, Control Gain Register 7
#define A_CODEC_CGR8		17			//17, Control Gain Register 8
#define A_CODEC_CGR9		18			//18, Control Gain Register 9
#define A_CODEC_CGR10		19			//19£¬Control Gain Register 10
#define A_CODEC_TR1			20			//20, Test Register 1
#define A_CODEC_TR2			21			//21, Test Register 2
#define A_CODEC_CR3			22			//22, Control Register 3
#define A_CODEC_AGC1		23			//23, Automatic Gain Control 1
#define A_CODEC_AGC2		24			//24, Automatic Gain Control 2
#define A_CODEC_AGC3		25			//25, Automatic Gain Control 3
#define A_CODEC_AGC4		26			//26, Automatic Gain Control 4
#define A_CODEC_AGC5		27			//27, Automatic Gain Control 5

/*******************************************************************************
*			Address: 0		AICR Common Define
*******************************************************************************/
#define CONFIG1_MASK		(0xF << 0)
#define CONFIG1				(0xF << 0)

/*******************************************************************************
*			Address: 1		CR1 Common Define
*******************************************************************************/
#define SB_MICBIAS			(1 << 7)
#define MONO				(1 << 6)
#define DAC_MUTE			(1 << 5)
#define HP_DIS				(1 << 4)
#define DACSEL				(1 << 3)
#define BYPASS				(1 << 2)

/*******************************************************************************
*			Address: 2		CR2 Common Define
*******************************************************************************/
#define DAC_DEEMP			(1 << 7)
#define DAC_ADWL_MASK		(0x3 << 5)
#define DAC_ADWL(x)			((x) << 5)					//x = 0, 1, 2, 3
#define ADC_ADWL_MASK		(0x3 << 3)
#define ADC_ADWL(x)			((x) << 3)					//x = 0, 1, 2, 3
#define ADC_HPF				(1 << 2)

/*******************************************************************************
*			Address: 3		CCR1 Common Define
*******************************************************************************/
#define CONFIG4_MASK		(0xF << 0)
#define CONFIG4				(0x0 << 0)

/*******************************************************************************
*			Address: 4		CCR2 Common Define
*******************************************************************************/
#define DFREQ_MASK			(0xF << 4)
#define DFREQ(x)			((x) << 4)					//x = 0, 1, 2...15
#define AFREQ_MASK			(0xF << 0)
#define AFREQ(x)			((x) << 0)					//x = 0, 1, 2... 15

/*******************************************************************************
*			Address: 5		PMR1 Common Define
*******************************************************************************/
#define SB_DAC				(1 << 7)
#define SB_OUT				(1 << 6)
#define SB_MIX				(1 << 5)
#define SB_ADC				(1 << 4)
#define SB_LIN				(1 << 3)
#define SB_IND				(1 << 0)

/*******************************************************************************
*			Address: 6		PMR2 Common Define
*******************************************************************************/
#define LRGI_RLGI_MASK		(0x3 << 6)
#define LRGI_RLGI(x)		((x) << 6)					//x = 0, 1, 2, 3
#define LRGOD_RLGOD_MASK	(0x3 << 4)
#define LRGOD_RLGOD(x)		((x) << 4)					//x = 0, 1, 2, 3
#define GIM					(1 << 3)
#define SB_MC				(1 << 2)
#define SB					(1 << 1)
#define SB_SLEEP			(1 << 0)

/*******************************************************************************
*			Address: 7		CRR Common Define
*******************************************************************************/
#define RATIO_MASK			(0x3 << 5)
#define RATIO(x)			((x) << 5)					//x = 0, 1, 2, 3
#define KFAST_MASK			(0x7 << 2)
#define KFAST(x)			(x) << 2)					//x = 0, 1, 2...7
#define TRESH_MASK			(0x3 << 0)
#define TRESH(x)			((x) << 0)					//x = 0, 1, 2, 3

/*******************************************************************************
*			Address: 8		ICR Common Define
*******************************************************************************/
#define INT_FORM_MASK		((0x3) << 6)
#define INT_FORM(x)			((x) << 6)					//x = 0, 1, 2, 3
#define JACK_MASK				(1 << 5)
#define CCMC_MASK			(1 << 4)
#define RUD_MASK			(1 << 3)
#define RDD_MASK			(1 << 2)
#define GUD_MASK			(1 << 1)
#define GDD_MASK			(1 << 0)

/*******************************************************************************
*			Address: 9		IFR Common Define
*******************************************************************************/
#define JACK				(1 << 6)
#define JACK_EVENT				(1 << 6)
#define CCMC				(1 << 4)
#define RAMP_UP_DONE		(1 << 3)
#define RAMP_DOWN_DONE		(1 << 2)
#define GAIN_UP_DONE		(1 << 1)
#define GAIN_DOWN_DONE		(1 << 0)

/*******************************************************************************
*			Address: 10		CGR1 Common Define
*******************************************************************************/
#define GODR_MASK			(0xF << 4)
#define GODR(x)				((x) << 4)					//x = 0, 1, 2...15
#define GODL_MASK			(0xF << 0)
#define GODL(x)				((x) << 0)					//x = 0, 1, 2...15

/*******************************************************************************
*			Address: 11		CGR2 Common Define
*******************************************************************************/
#define LRGO1_RLGO1_MASK	((0x3) << 6)
#define LRGO1_RLGO1(x)		((x) << 6)					//x = 0, 1, 2, 3
#define GO1L_MASK			(0x1F << 0)
#define GO1L(x)				((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 12		CGR3 Common Define
*******************************************************************************/
#define GO1R_MASK			(0x1F << 0)
#define GO1R(x)				((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 13		CGR4 Common Define
*******************************************************************************/
#define LRGO2_RLGO2_MASK	(0x3 << 6)
#define LRGO2_RLGO2(x)		((x) << 6)					//x = 0, 1, 2, 3
#define GO2L_MASK			(0x1F << 0)
#define GO2L(x)				((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 14		CGR5 Common Define
*******************************************************************************/
#define GO2R_MASK			(0x1F << 0)
#define GO2R(x)				((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 15		CGR6 Common Define
*******************************************************************************/
#define RLGO3_MASK		(0x3 << 6)
#define RLGO3(x)			((x) << 6)					//x = 0, 1, 2, 3
#define GO3L_MASK		(0x1F << 0)
#define GO3L(x)			((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 16		CGR7 Common Define
*******************************************************************************/
#define GO3R_MASK		(0x1F << 0)
#define GO3R(x)			((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 17		CGR8 Common Define
*******************************************************************************/
#define LRGO_RLGO_MASK		(0x3 << 6)
#define LRGO_RLGO(x)		((x) << 6)
#define GOL_MASK			(0x1F << 0)
#define GOL(x)				((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 18		CGR9 Common Define
*******************************************************************************/
#define GOR_MASK			(0x1F << 0)
#define GOR(x)				((x) << 0)

/*******************************************************************************
*			Address: 19		CGR10 Common Define
*******************************************************************************/
#define GIL_MASK			(0xF << 4)
#define GIL(x)				((x) << 4)					//x = 0, 1, 2...15
#define GIR_MASK			(0xF << 0)
#define GIR(x)				((x) << 0)					//x = 0, 1, 2...15

/*******************************************************************************
*			Address: 20		TR1 Common Define
*******************************************************************************/
#define STBYO				(1 << 7)
#define STBYI				(1 << 6)
#define TSTDAC				(1 << 5)
#define TSTADC				(1 << 4)
#define TEST				(1 << 3)
#define STOPULL				(1 << 2)
#define NOSC				(1 << 1)
#define FAST_ON				(1 << 0)

/*******************************************************************************
*			Address: 21		TR2 Common Define
*******************************************************************************/
#define FAENDAC				(1 << 7)
#define FAENADC				(1 << 6)
#define NENCOMP				(1 << 5)
#define NODEM				(1 << 3)
#define HIPAS				(1 << 2)
#define NO_RST				(1 << 1)
#define UNSTBL				(1 << 0)

/*******************************************************************************
*			Address: 22		CR3 Common Define
*******************************************************************************/
#define SB_MIC1				(1 << 7)
#define SB_MIC2				(1 << 6)
#define SIDETONE1			(1 << 5)
#define SIDETONE2			(1 << 4)
#define MICDIFF				(1 << 3)
#define MICSTEREO			(1 << 2)
#define INSEL_MASK			(0x3 << 0)
#define INSEL(x)			((x) << 0)					//x = 0, 1, 2, 3

/*******************************************************************************
*			Address: 23		AGC1 Common Define
*******************************************************************************/
#define AGC_EN				(1 << 7)
#define TARGET_MASK			(0xf << 2)
#define TARGET(x)			((x) << 2)					//x = 0, 1, 2...15

/*******************************************************************************
*			Address: 24		AGC2 Common Define
*******************************************************************************/
#define NG_EN				(1 << 7)
#define NG_THR_MASK			(0x5 << 4)
#define NG_THR(x)			((x) << 4)
#define HOLD_MASK			(0xF << 0)
#define HOLD(x)				((x) << 0)

/*******************************************************************************
*			Address: 25		AGC3 Common Define
*******************************************************************************/
#define ATK_MASK			(0xF << 4)
#define ATK(x)				((x) << 4)					//x = 0, 1, 2...15
#define DCY_MASK			(0xF << 0)
#define DCY(x)				((x) << 0)					//x = 0, 1, 2...15

/*******************************************************************************
*			Address: 26		AGC4 Common Define
*******************************************************************************/
#define AGC_MAX_MASK		(0x1F << 0)
#define AGC_MAX(x)			((x) << 0)					//x = 0, 1, 2...31

/*******************************************************************************
*			Address: 27		AGC5 Common Define
*******************************************************************************/
#define AGC_MIN_MASK		(0x1F << 0)
#define AGC_MIN(x)			((x) << 0)					//x = 0, 1, 2...31

#ifndef __MIPS_ASSEMBLER


unsigned char codec_reg_read(unsigned char addr);
void codec_reg_write(unsigned char addr, unsigned char data);
void codec_reg_set(unsigned char addr, unsigned char data);
void codec_reg_clear(unsigned char addr, unsigned char data);


typedef volatile struct{
	unsigned char AICR;							//Address: 0
	unsigned char CR1;							//Address: 1
	unsigned char CR2;							//Address: 2
	unsigned char CCR1;							//Address: 3
	unsigned char CCR2;							//Address: 4
	unsigned char PMR1;							//Address: 5
	unsigned char PMR2;							//Address: 6
	unsigned char CRR;							//Address: 7
	unsigned char ICR;							//Address: 8
	unsigned char IFR;							//Address: 9
	unsigned char CGR1;							//Address: 10
	unsigned char CGR2;							//Address: 11
	unsigned char CGR3;							//Address: 12
	unsigned char CGR4;							//Address: 13
	unsigned char CGR5;							//Address: 14
	unsigned char CGR6;							//Address: 15
	unsigned char CGR7;							//Address: 16
	unsigned char CGR8;							//Address: 17
	unsigned char CGR9;							//Address: 18
	unsigned char CGR10;						//Address: 19
	unsigned char TR1;							//Address: 20
	unsigned char TR2;							//Address: 21
	unsigned char CR3;							//Address: 22
	unsigned char AGC1;							//Address: 23
	unsigned char AGC2;							//Address: 24
	unsigned char AGC3;							//Address: 25
	unsigned char AGC4;							//Address: 26
	unsigned char AGC5;							//Address: 27
}JZ4755_CODEC, *PJZ4755_CODEC;

#endif

#endif
