//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755OST_H__
#define __JZ4755OST_H__

//--------------------------------------------------------------------------
// Operating System Timer Unit (OST) Address Definition
//--------------------------------------------------------------------------
#define OST_PHYS_ADDR			( 0x10002000 )
#define OST_BASE_U_VIRTUAL		( 0xB0002000 )

//--------------------------------------------------------------------------
// OST Registers Offset Definition
//--------------------------------------------------------------------------

#define	OST_DR_OFFSET			( 0xe0 )	// RW, 32, 0x????
#define	OST_CNT_OFFSET			( 0xe8 )	// RW, 32, 0x????
#define	OST_CSR_OFFSET			( 0xeC )	// RW, 16, 0x0000

//--------------------------------------------------------------------------
// OST Registers Address Definition
//--------------------------------------------------------------------------
#define A_OST_DR				( OST_BASE_U_VIRTUAL + OST_DR_OFFSET )
#define A_OST_CNT				( OST_BASE_U_VIRTUAL + OST_CNT_OFFSET )
#define A_OST_CSR				( OST_BASE_U_VIRTUAL + OST_CSR_OFFSET )

//--------------------------------------------------------------------------
// CSR Common Define
//--------------------------------------------------------------------------
#define OST_CNT_MD						( 1 << 15 )
#define OST_PWM_SHUT_ABRUPT				( 1 << 9 )

#define	OST_CLK_PRESCALE_CLK1			( 0x0 << 3)
#define	OST_CLK_PRESCALE_CLK4			( 0x1 << 3)
#define	OST_CLK_PRESCALE_CLK16			( 0x2 << 3)
#define	OST_CLK_PRESCALE_CLK64			( 0x3 << 3)
#define	OST_CLK_PRESCALE_CLK256			( 0x4 << 3)
#define	OST_CLK_PRESCALE_CLK1024		( 0x5 << 3)
#define	OST_CLK_EXTAL					( 0x1 << 2)
#define	OST_CLK_RTC						( 0x1 << 1)
#define	OST_CLK_PCLK					( 0x1 << 0)


#ifndef __MIPS_ASSEMBLER

typedef struct
{   unsigned int    RSV[56];			// 0x00 ,0xdf
	unsigned int	DR;					// 0xe0, 32 R/W
	unsigned int	CNT;				// 0xe8, 32 R/W
	unsigned short	CSR;				// 0xeC, 16 R/W
} JZ4755_OST, *PJZ4755_OST;


#endif // __MIPS_ASSEMBLER

#endif // __JZ4755OST_H__
