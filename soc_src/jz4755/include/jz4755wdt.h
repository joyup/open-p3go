//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755WDT_H__
#define __JZ4755WDT_H__

//--------------------------------------------------------------------------
// Watchdog Timer(WDT) Address Definition
//--------------------------------------------------------------------------
#define WDT_PHYS_ADDR			( 0x10002000 )
#define WDT_BASE_U_VIRTUAL		( 0xB0002000 )


//--------------------------------------------------------------------------
// WDT Registers Offset Definition
//--------------------------------------------------------------------------
#define WDT_TDR_OFFSET		( 0x00 )	// RW, 16
#define WDT_TCER_OFFSET		( 0x04 )	// RW, 8
#define WDT_TCNT_OFFSET		( 0x08 )	// RW, 16
#define WDT_TCSR_OFFSET		( 0x0C )	// RW, 16


//--------------------------------------------------------------------------
// TCU Registers Address Definition
//--------------------------------------------------------------------------
#define A_WDT_TDR				( WDT_BASE_U_VIRTUAL + WDT_TDR_OFFSET	 )
#define A_WDT_TCER				( WDT_BASE_U_VIRTUAL + WDT_TCER_OFFSET	 )
#define A_WDT_TCNT				( WDT_BASE_U_VIRTUAL + WDT_TCNT_OFFSET	 )
#define A_WDT_TCSR				( WDT_BASE_U_VIRTUAL + WDT_TCSR_OFFSET	 )


//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------

// TDR
#define	WDT_CLK_PRESCALE_CLK1			( 0x0 << 3)
#define	WDT_CLK_PRESCALE_CLK4			( 0x1 << 3)
#define	WDT_CLK_PRESCALE_CLK16			( 0x2 << 3)
#define	WDT_CLK_PRESCALE_CLK64			( 0x3 << 3)
#define	WDT_CLK_PRESCALE_CLK256			( 0x4 << 3)
#define	WDT_CLK_PRESCALE_CLK1024		( 0x5 << 3)
#define	WDT_CLK_PRESCALE_MASK			( 0x3F << 3)

#define	WDT_CLK_EXTAL					( 0x1 << 2)
#define	WDT_CLK_RTC						( 0x1 << 1)
#define	WDT_CLK_PCLK					( 0x1 << 0)
#define	WDT_CLK_MASK					( 7 )

// TCER
#define	WDT_ENABLE			( 1 << 0 )

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned short TDR;				/* 0x00,  16 R/W   */
	unsigned short WDTRSV0;
	unsigned char TCER;				/* 0x04,  8 R/W   */
	unsigned char WDTRSV1[3];
	unsigned short TCNT;				/* 0x08,  16 R/W   */
	unsigned short WDTRSV2;
	unsigned short TCSR;				/* 0x0C,  16 R/W   */
	unsigned short WDTRSV3;
} JZ4755_WDT, *PJZ4755_WDT;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755WDT_H__
