//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755TCU_H__
#define __JZ4755TCU_H__

//--------------------------------------------------------------------------
// Timer & Counter Unit(TCU) Address Definition
//--------------------------------------------------------------------------
#define TCU_PHYS_ADDR			( 0x10002000 )
#define TCU_BASE_U_VIRTUAL		( 0xB0002000 )

#define	TCU_TIMER0_U_VIRTUAL	( 0xB0002040 )
#define	TCU_CHANNEL_OFFSET		( 0x10 )

//--------------------------------------------------------------------------
// TCU Registers Offset Definition
//--------------------------------------------------------------------------
#define TCU_TSR_OFFSET			( 0x1C )	//  R,  8, 0x00
#define TCU_TSSR_OFFSET			( 0x2C )	//  W,  8, 0x??
#define TCU_TSCR_OFFSET			( 0x3C )	//  W,  8, 0x??
#define TCU_TER_OFFSET			( 0x10 )	//  R,  8, 0x00
#define TCU_TESR_OFFSET			( 0x14 )	//  W,  8, 0x??
#define TCU_TECR_OFFSET			( 0x18 )	//  W,  8, 0x??
#define TCU_TFR_OFFSET			( 0x20 )	//  R, 32, 0x00000000
#define TCU_TFSR_OFFSET			( 0x24 )	//  W, 32, 0x????????
#define TCU_TFCR_OFFSET			( 0x28 )	//  W, 32, 0x????????
#define TCU_TMR_OFFSET			( 0x30 )	//  R, 32, 0x00000000
#define TCU_TMSR_OFFSET			( 0x34 )	//  W, 32, 0x????????
#define TCU_TMCR_OFFSET			( 0x38 )	//  W, 32, 0x????????
#define	TCU_TDFRX_OFFSET		( 0x00 )	// RW, 16, 0x????
#define	TCU_TDHRX_OFFSET		( 0x04 )	// RW, 16, 0x????
#define	TCU_TCNTX_OFFSET		( 0x08 )	// RW, 16, 0x????
#define	TCU_TCSRX_OFFSET		( 0x0C )	// RW, 16, 0x0000

#define TCU_TSTR_OFFSET			( 0xF0 )	//  R,  8, 0x00
#define TCU_TSTSR_OFFSET		( 0xF4 )	//  W,  8, 0x??
#define TCU_TSTCR_OFFSET		( 0xF8 )	//  W,  8, 0x??
//--------------------------------------------------------------------------
// TCU Registers Address Definition
//--------------------------------------------------------------------------
#define A_TCU_TSTR				( TCU_BASE_U_VIRTUAL + TCU_TSTR_OFFSET )
#define A_TCU_TSTSR				( TCU_BASE_U_VIRTUAL + TCU_TSTSR_OFFSET )
#define A_TCU_TSTCR				( TCU_BASE_U_VIRTUAL + TCU_TSTCR_OFFSET )
#define A_TCU_TSR				( TCU_BASE_U_VIRTUAL + TCU_TSR_OFFSET )
#define A_TCU_TSSR				( TCU_BASE_U_VIRTUAL + TCU_TSSR_OFFSET )
#define A_TCU_TSCR				( TCU_BASE_U_VIRTUAL + TCU_TSCR_OFFSET )
#define A_TCU_TER				( TCU_BASE_U_VIRTUAL + TCU_TER_OFFSET )
#define A_TCU_TESR				( TCU_BASE_U_VIRTUAL + TCU_TESR_OFFSET )
#define A_TCU_TECR				( TCU_BASE_U_VIRTUAL + TCU_TECR_OFFSET )
#define A_TCU_TFR				( TCU_BASE_U_VIRTUAL + TCU_TFR_OFFSET )
#define A_TCU_TFSR				( TCU_BASE_U_VIRTUAL + TCU_TFSR_OFFSET )
#define A_TCU_TFCR				( TCU_BASE_U_VIRTUAL + TCU_TFCR_OFFSET )
#define A_TCU_TMR				( TCU_BASE_U_VIRTUAL + TCU_TMR_OFFSET )
#define A_TCU_TMSR				( TCU_BASE_U_VIRTUAL + TCU_TMSR_OFFSET )
#define A_TCU_TMCR				( TCU_BASE_U_VIRTUAL + TCU_TMCR_OFFSET )
#define A_TCU_TDFR(x)			( TCU_TIMER0_U_VIRTUAL + TCU_CHANNEL_OFFSET * (x) + TCU_TDFRX_OFFSET )
#define A_TCU_TDHR(x)			( TCU_TIMER0_U_VIRTUAL + TCU_CHANNEL_OFFSET * (x) + TCU_TDHRX_OFFSET )
#define A_TCU_TCNT(x)			( TCU_TIMER0_U_VIRTUAL + TCU_CHANNEL_OFFSET * (x) + TCU_TCNTX_OFFSET )
#define A_TCU_TCSR(x)			( TCU_TIMER0_U_VIRTUAL + TCU_CHANNEL_OFFSET * (x) + TCU_TCSRX_OFFSET )

//--------------------------------------------------------------------------
// TER, TSR Common Define
//--------------------------------------------------------------------------
#define	TCU_OSTEN		( 1 << 15 )
#define	TCU_OSTST		( 1 << 15 )
#define	TCU_OSTCL		( 1 << 15 )
#define	TCU_TIMER5		( 1 << 5 )
#define	TCU_TIMER4		( 1 << 4 )
#define	TCU_TIMER3		( 1 << 3 )
#define	TCU_TIMER2		( 1 << 2 )
#define	TCU_TIMER1		( 1 << 1 )
#define	TCU_TIMER0		( 1 << 0 )
#define	TCU_TIMER(x)	( 1 << (x) )		// The range is from 0 to 5

//--------------------------------------------------------------------------
// TFLAG TMASK Common Define
//--------------------------------------------------------------------------
#define	TCU_OSTFLAG		( 1 << 15 )
#define	TCU_OSTFST		( 1 << 15 )
#define	TCU_OSTFCL		( 1 << 15 )

#define	TCU_OSTMASK		( 1 << 15 )
#define	TCU_OSTMST		( 1 << 15 )
#define	TCU_OSTMCL		( 1 << 15 )

#define	TCU_HFLAG5		( 1 << 21 )
#define	TCU_HFLAG4		( 1 << 20 )
#define	TCU_HFLAG3		( 1 << 19 )
#define	TCU_HFLAG2		( 1 << 18 )
#define	TCU_HFLAG1		( 1 << 17 )
#define	TCU_HFLAG0		( 1 << 16 )
#define	TCU_HFLAG(x)	( 1 << (16 + (x)) )		// The range is from 0 to 5

#define	TCU_FFLAG5		TCU_TIMER5
#define	TCU_FFLAG4		TCU_TIMER4
#define	TCU_FFLAG3		TCU_TIMER3
#define	TCU_FFLAG2		TCU_TIMER2
#define	TCU_FFLAG1		TCU_TIMER1
#define	TCU_FFLAG0		TCU_TIMER0
#define	TCU_FFLAG(x)	( 1 << (x) )			// The range is from 0 to 5

// Each channel 
#define	TCU_INTR_FLAG(n)	( TCU_HFLAG(n) | TCU_FFLAG(n) )

//--------------------------------------------------------------------------
// TCSR Common Define
//--------------------------------------------------------------------------
#define TCSR_CLRZ				( 1 << 10 )
#define TCSR_PWM_SHUT_ABRUPT	( 1 << 9 )
#define TCSR_PWM_INIT_HIGH		( 1 << 8 )
#define TCSR_PWM_EN				( 1 << 7 )

#define	TCU_CLK_PRESCALE_CLK1			( 0x0 << 3)
#define	TCU_CLK_PRESCALE_CLK4			( 0x1 << 3)
#define	TCU_CLK_PRESCALE_CLK16			( 0x2 << 3)
#define	TCU_CLK_PRESCALE_CLK64			( 0x3 << 3)
#define	TCU_CLK_PRESCALE_CLK256			( 0x4 << 3)
#define	TCU_CLK_PRESCALE_CLK1024		( 0x5 << 3)
#define	TCU_CLK_PRESCALE_MASK			( 0x3F << 3)

#define	TCU_CLK_EXTAL					( 0x1 << 2)
#define	TCU_CLK_RTC						( 0x1 << 1)
#define	TCU_CLK_PCLK					( 0x1 << 0)
#define	TCU_CLK_MASK					( 7 )

//--------------------------------------------------------------------------
// TCU Channel Assigned Define
//--------------------------------------------------------------------------
#define	TCU_SYSTEM_TIMER_CHN		5
#define	TCU_BAT_CHK_CHN				4


#ifndef __MIPS_ASSEMBLER

#define TCU_CHANNEL_NUM			6
#define NUM_TIMER_TCU2			5

typedef struct
{
	unsigned short	TDFR;				// 0x00, 16 R/W
	unsigned short	TCURSV1;
	unsigned short	TDHR;				// 0x04, 16 R/W
	unsigned short	TCURSV2;
	unsigned short	TCNT;				// 0x08, 16 R/W
	unsigned short	TCURSV3;
	unsigned short	TCSR;				// 0x0C, 16 R/W
	unsigned short	TCURSV4;
} TCU_CHANNEL, *PTCU_CHANNEL;

typedef volatile struct
{
	unsigned int	TCURSV0[4];		// 0x00, ---

	unsigned char	TER;				// 0x10,  8 R
	unsigned char	TCURSV1[3];
	unsigned char	TESR;				// 0x14,  8 W
	unsigned char	TCURSV2[3];
	unsigned char	TECR;				// 0x18,  8 W
	unsigned char	TCURSV3[3];

	unsigned char	TSR;				// 0x1C,  8 R
	unsigned char	TCURSV4[3];

	unsigned int	TFR;				// 0x20,  32 R
	unsigned int	TFSR;				// 0x24,  32 W
	unsigned int	TFCR;				// 0x28,  32 W

	unsigned char	TSSR;				// 0x2C,  8 W
	unsigned char	TCURSV5[3];

	unsigned int	TMR;				// 0x30,  32 R
	unsigned int	TMSR;				// 0x34,  32 W
	unsigned int	TMCR;				// 0x38,  32 W

	unsigned char	TSCR;				// 0x3C,  8 W
	unsigned char	TCURSV6[3];

	unsigned short	TDFR0;				// 0x40, 16 R/W
	unsigned short	TCU0RSV1[1];
	unsigned short	TDHR0;				// 0x44, 16 R/W
	unsigned short	TCU0RSV2[1];
	unsigned short	TCNT0;				// 0x48, 16 R/W
	unsigned short	TCU0RSV3[1];
	unsigned short	TCSR0;				// 0x4C, 16 R/W
	unsigned short	TCU0RSV4[1];

	unsigned short	TDFR1;				// 0x50, 16 R/W
	unsigned short	TCU1RSV1[1];
	unsigned short	TDHR1;				// 0x54, 16 R/W
	unsigned short	TCU1RSV2[1];
	unsigned short	TCNT1;				// 0x58, 16 R/W
	unsigned short	TCU1RSV3[1];
	unsigned short	TCSR1;				// 0x5C, 16 R/W
	unsigned short	TCU1RSV4[1];

	unsigned short	TDFR2;				// 0x60, 16 R/W
	unsigned short	TCU2RSV1[1];
	unsigned short	TDHR2;				// 0x64, 16 R/W
	unsigned short	TCU2RSV2[1];
	unsigned short	TCNT2;				// 0x68, 16 R/W
	unsigned short	TCU2RSV3[1];
	unsigned short	TCSR2;				// 0x6C, 16 R/W
	unsigned short	TCU2RSV4[1];

	unsigned short	TDFR3;				// 0x70, 16 R/W
	unsigned short	TCU3RSV1[1];
	unsigned short	TDHR3;				// 0x74, 16 R/W
	unsigned short	TCU3RSV2[1];
	unsigned short	TCNT3;				// 0x78, 16 R/W
	unsigned short	TCU3RSV3[1];
	unsigned short	TCSR3;				// 0x7C, 16 R/W
	unsigned short	TCU3RSV4[1];

	unsigned short	TDFR4;				// 0x80, 16 R/W
	unsigned short	TCU4RSV1[1];
	unsigned short	TDHR4;				// 0x84, 16 R/W
	unsigned short	TCU4RSV2[1];
	unsigned short	TCNT4;				// 0x88, 16 R/W
	unsigned short	TCU4RSV3[1];
	unsigned short	TCSR4;				// 0x8C, 16 R/W
	unsigned short	TCU4RSV4[1];

	unsigned short	TDFR5;				// 0x90, 16 R/W
	unsigned short	TCU5RSV1[1];
	unsigned short	TDHR5;				// 0x94, 16 R/W
	unsigned short	TCU5RSV2[1];
	unsigned short	TCNT5;				// 0x98, 16 R/W
	unsigned short	TCU5RSV3[1];
	unsigned short	TCSR5;				// 0x9C, 16 R/W
	unsigned short	TCU5RSV4[1];
	
	unsigned int	TCURSV7 [20];
	
	unsigned int	TSTR;				// 0XF0,   16 R
	unsigned int	TSTSR;				// 0XF4W,  16 W
	unsigned int	TSTCR;				// 0XF8W,  16 W


} JZ4755_TCU, *PJZ4755_TCU;

typedef volatile struct
{
	unsigned int	TCURSV0[4];		// 0x00, ---

	unsigned char	TER;				// 0x10,  8 R
	unsigned char	TCURSV1[3];
	unsigned char	TESR;				// 0x14,  8 W
	unsigned char	TCURSV2[3];
	unsigned char	TECR;				// 0x18,  8 W
	unsigned char	TCURSV3[3];

	unsigned char	TSR;				// 0x1C,  8 R
	unsigned char	TCURSV4[3];

	unsigned int	TFR;				// 0x20,  32 R
	unsigned int	TFSR;				// 0x24,  32 W
	unsigned int	TFCR;				// 0x28,  32 W

	unsigned char	TSSR;				// 0x2C,  8 W
	unsigned char	TCURSV5[3];

	unsigned int	TMR;				// 0x30,  32 R
	unsigned int	TMSR;				// 0x34,  32 W
	unsigned int	TMCR;				// 0x38,  32 W

	unsigned char	TSCR;				// 0x3C,  8 W
	unsigned char	TCURSV6[3];

	TCU_CHANNEL		CHN[TCU_CHANNEL_NUM];	
	
	unsigned int	TCURSV7 [20];
	
	unsigned int	TSTR;				// 0XF0,   16 R
	unsigned int	TSTSR;				// 0XF4W,  16 W
	unsigned int	TSTCR;				// 0XF8W,  16 W			
} JZ4755_TCU2, *PJZ4755_TCU2;


#endif // __MIPS_ASSEMBLER

#endif // __JZ4755TCU_H__
