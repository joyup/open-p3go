//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755RTC_H__
#define __JZ4755RTC_H__

//--------------------------------------------------------------------------
// Real Time Clock(RTC) Address Definition
//--------------------------------------------------------------------------
#define RTC_PHYS_ADDR			( 0x10003000 )
#define RTC_BASE_U_VIRTUAL		( 0xB0003000 )


//--------------------------------------------------------------------------
// RTC Registers Offset Definition
//--------------------------------------------------------------------------
#define RTC_RTCCR_OFFSET		( 0x00 )
#define RTC_RTCSR_OFFSET		( 0x04 )
#define RTC_RTCSAR_OFFSET		( 0x08 )
#define RTC_RTCGR_OFFSET		( 0x0C )

#define RTC_HCR_OFFSET			( 0x20 )
#define RTC_HWFCR_OFFSET		( 0x24 )
#define RTC_HRCR_OFFSET			( 0x28 )
#define RTC_HWCR_OFFSET			( 0x2C )
#define RTC_HWRSR_OFFSET		( 0x30 )
#define RTC_HSPR_OFFSET			( 0x34 )



//--------------------------------------------------------------------------
// RTC Registers Address Definition
//--------------------------------------------------------------------------
#define A_RTC_RTCCR			( RTC_BASE_U_VIRTUAL + RTC_RTCCR_OFFSET )
#define A_RTC_RTCSR			( RTC_BASE_U_VIRTUAL + RTC_RTCSR_OFFSET )
#define A_RTC_RTCSAR		( RTC_BASE_U_VIRTUAL + RTC_RTCSAR_OFFSET )
#define A_RTC_RTCGR			( RTC_BASE_U_VIRTUAL + RTC_RTCGR_OFFSET )

#define A_RTC_HCR			  ( RTC_BASE_U_VIRTUAL + RTC_HCR_OFFSET )
#define A_RTC_HWFCR			( RTC_BASE_U_VIRTUAL + RTC_HWFCR_OFFSET )
#define A_RTC_HRCR			( RTC_BASE_U_VIRTUAL + RTC_HRCR_OFFSET )
#define A_RTC_HWCR			( RTC_BASE_U_VIRTUAL + RTC_HWCR_OFFSET )
#define A_RTC_HWRSR			( RTC_BASE_U_VIRTUAL + RTC_HWRSR_OFFSET )
#define A_RTC_HSPR			( RTC_BASE_U_VIRTUAL + RTC_HSPR_OFFSET )


//--------------------------------------------------------------------------
// RTC Common Define
//--------------------------------------------------------------------------

// RTC Control Register(RTCCR) Common Define
#define RTCCR_READY			( 1 << 7 )
#define RTCCR_1HZ			( 1 << 6 )
#define RTCCR_1HZIE			( 1 << 5 )
#define RTCCR_AF			( 1 << 4 )
#define RTCCR_AIE			( 1 << 3 )
#define RTCCR_AE			( 1 << 2 )
#define RTCCR_SELEXC		( 1 << 1 )
#define RTCCR_RTCEN			( 1 << 0 )

// RTC Regulator Register(RTCGR) Common Define
#define	RTCGR_LOCK			( 1 << 31 )

#define SHIFT_ADC			16	// bit16 - bit25
#define SHIFT_DIV			0	// bit0 - bit15

#define RTCGR_ADC_MASK		( 0x3FF << SHIFT_ADC )
#define RTCGR_DIV_MASK		( 0xFFFF << SHIFT_DIV )

#define RTCGR_ADC_CYCLE		( (0 << SHIFT_ADC) & RTCGR_ADC_MASK )
#define RTCGR_DIV_1HZ		( (32767 << SHIFT_DIV) & RTCGR_DIV_MASK )
#define RTCGR_DIV_1KHZ		( (31 << SHIFT_DIV) & RTCGR_DIV_MASK )

// Hibernate Control Register (HCR)  Common Define
#define HCR_PD		( 1 << 0 )

// Hibernate Wakeup Status Register(HWRSR) Common Define
#define SHIFT_HWRSR_HR 5

#define HWRSR_HR		( 1 << SHIFT_HWRSR_HR )

// HIBERNATE Wakeup Control Register (HWCR) Common Define
#define HWCR_EALM		( 1 << 0 )

//Hibernate Reset Counter Register (HRCR) Common Define
#define SHIFT_HRCR  5
#define HRCR_MASK   (0x7f << SHIFT_HRCR)
// HIBERNATE Wakeup Status Register (HWRSR)
//RTC Alarm Status bit.
#define SHIFT_HWRSR_ALM   1
#define HWRSR_ALM_MASK   (1 << SHIFT_HWRSR)
//Wakeup Pin Status bit.
#define SHIFT_HWRSR_PIN   2
#define HWRSR_PIN_MASK   (1 << SHIFT_HWRSR_PIN)
//PAD PIN Reset.
#define SHIFT_HWRSR_PPR   3
#define HWRSR_PPR_MASK   (1 << SHIFT_HWRSR_PPR)

//Hibernate Reset.
#define SHIFT_HWRSR_HIB   4
#define HWRSR_HIB_MASK   (1 << SHIFT_HWRSR_HIB)


#define	COLD_BOOT_SIG		0x12345678


#ifndef __MIPS_ASSEMBLER

//2010.03.10 danny changed for test
//#define	WAIT_WRDY()	{ while ( !(INREG32(A_RTC_RTCCR) & RTCCR_READY) ); }
#define	WAIT_WRDY()	{ mdelay(1); }

#define	RTC_INREG32(x)		({			\
	unsigned int v = INREG32(x);		\
	while (v != INREG32(x))				\
		v = INREG32(x);					\
	v;									\
})

#define RTC_OUTREG32(x, y)		\
do {							\
	WAIT_WRDY();				\
	OUTREG32((x), (y));			\
	WAIT_WRDY();				\
}while(0)

#define RTC_SETREG32(x, y)		\
do {							\
	WAIT_WRDY();				\
	SETREG32((x), (y));			\
	WAIT_WRDY();				\
}while(0)

#define RTC_CLRREG32(x, y)		\
do {							\
	WAIT_WRDY();				\
	CLRREG32((x), (y));			\
	WAIT_WRDY();				\
}while(0)


typedef volatile struct
{
	unsigned int	RTCCR;				/* 0x00,  32 R/W   */
	unsigned int    RTCSR;				/* 0x04,  32 R/W */
	unsigned int	RTCSAR;				/* 0x08,  32 R/W */
	unsigned int	RTCGR;				/* 0x0C,  32 W   */

	unsigned int	RTCRSV00[4];		/* 0x10, 0x14, 0x18, 0x1C   */

	unsigned int	HCR;				/* 0x20,  32 W   */
	unsigned int	HWFCR;				/* 0x24,  32 W   */
	unsigned int	HRCR;				/* 0x28,  32 W   */
	unsigned int	HWCR;				/* 0x2C,  32 W   */
	unsigned int	HWRSR;				/* 0x30,  32 W   */
	unsigned int	HSPR;				/* 0x34,  32 W   */

} JZ4755_RTC, *PJZ4755_RTC;

typedef enum
{
	eYEAR = 0,
    eMONTH,
	eDAYOFWEEK,
    eDAY,
    eHOUR,
    eMINUTE,
    eSECOND
} DATEANDTIME, *PDATEANDTIME;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755RTC_H__
