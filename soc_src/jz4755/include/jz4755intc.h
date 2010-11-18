//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2009.
//

#ifndef __JZ4755INTC_H__
#define __JZ4755INTC_H__

//--------------------------------------------------------------------------
// Interrupt Controller(INTC) Address Definition
//--------------------------------------------------------------------------
#define INTC_PHYS_ADDR			( 0x10001000 )
#define INTC_BASE_U_VIRTUAL		( 0xB0001000 )


//--------------------------------------------------------------------------
// INTC Registers Offset Definition
//--------------------------------------------------------------------------
#define INTC_ICSR_OFFSET		( 0x00 )	// RW, 32, 0x00000000
#define INTC_ICMR_OFFSET		( 0x04 )	// RW, 32, 0xFFFFFFFF
#define INTC_ICMSR_OFFSET		( 0x08 )	//  W, 32, 0x????????
#define INTC_ICMCR_OFFSET		( 0x0C )	//  W, 32, 0x????????
#define INTC_ICPR_OFFSET		( 0x10 )	//  R, 32, 0x00000000


//--------------------------------------------------------------------------
// INTC Registers Address Definition
//--------------------------------------------------------------------------
#define A_INTC_ICSR				( INTC_BASE_U_VIRTUAL + INTC_ICSR_OFFSET )
#define A_INTC_ICMR				( INTC_BASE_U_VIRTUAL + INTC_ICMR_OFFSET )
#define A_INTC_ICMSR			( INTC_BASE_U_VIRTUAL + INTC_ICMSR_OFFSET )
#define A_INTC_ICMCR			( INTC_BASE_U_VIRTUAL + INTC_ICMCR_OFFSET )
#define A_INTC_ICPR				( INTC_BASE_U_VIRTUAL + INTC_ICPR_OFFSET )


//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------
#define IRQ_ETH			0
#define	IRQ_RSV2		1
#define	IRQ_RSV1		2
#define	IRQ_RSV0		3
#define IRQ_SFT			4
#define IRQ_I2C			5
#define IRQ_RTC			6
#define IRQ_UART2		7
#define IRQ_UART1		8
#define IRQ_UART0		9
#define IRQ_AIC			10
#define IRQ_PORTF		11
#define IRQ_PORTE		12
#define IRQ_PORTD		13
#define IRQ_PORTC		14
#define IRQ_PORTB		15
#define IRQ_PORTA		16
#define IRQ_BCH			17
#define IRQ_SADC		18
#define IRQ_CIM			19
#define IRQ_TSSI		20
#define IRQ_TCU2		21
#define IRQ_TCU1		22
#define IRQ_TCU0		23
#define IRQ_MSC1		24
#define IRQ_MSC0		25
#define IRQ_SSI 		26
#define IRQ_UDC			27
#define IRQ_DMA_B		28		// For DMA channel 4-7
#define IRQ_DMA_A		29		// For DMA channel 0-3
#define IRQ_IPU			30
#define IRQ_LCD  		31


//--------------------------------------------------------------------------
// Macro define for Readable
//--------------------------------------------------------------------------
#define	IRQ_SYSTEM_TIMER	IRQ_TCU1


//--------------------------------------------------------------------------
// Extended IRQ(EIRQ) Define
//--------------------------------------------------------------------------
// GPIO, from PORT-A to PORT-F
#define EIRQ_PORTF_BIT		(1 << IRQ_PORTF)
#define EIRQ_PORTE_BIT		(1 << IRQ_PORTE)
#define EIRQ_PORTD_BIT		(1 << IRQ_PORTD)
#define EIRQ_PORTC_BIT		(1 << IRQ_PORTC)
#define EIRQ_PORTB_BIT		(1 << IRQ_PORTB)
#define EIRQ_PORTA_BIT		(1 << IRQ_PORTA)
#define	EIRQ_PORT_A2F_BIT	( EIRQ_PORTA_BIT | EIRQ_PORTB_BIT	\
							| EIRQ_PORTC_BIT | EIRQ_PORTD_BIT	\
							| EIRQ_PORTE_BIT | EIRQ_PORTF_BIT )

// DMA, two DMA controller A and B
#define	EIRQ_DMA_A_BIT		(1 << IRQ_DMA_A)
#define	EIRQ_DMA_B_BIT		(1 << IRQ_DMA_B)
#define	EIRQ_DMA_A2B_BIT	(EIRQ_DMA_A_BIT | EIRQ_DMA_B_BIT)

// TCU2
#define	EIRQ_TCU2_BIT		(1 << IRQ_TCU2)

// All the EXTENDED IRQ BIT
#define	EIRQ_ALL_BIT	(EIRQ_PORT_A2F_BIT | EIRQ_DMA_A2B_BIT | EIRQ_TCU2_BIT)


// Extended IRQ Boundary define
#define	IRQ_INTC_BASE	0
#define	IRQ_INTC_MAX	32

#define	EIRQ_TCU2_BASE	(IRQ_INTC_MAX)
#define	EIRQ_TCU2_MAX	(EIRQ_TCU2_BASE + NUM_TIMER_TCU2)

#define	EIRQ_DMA_BASE	(EIRQ_TCU2_MAX)		// 32 + 5 = 37
#define	EIRQ_DMA_MAX	(EIRQ_DMA_BASE + NUM_DMA)

#define	EIRQ_GPIO_BASE	(EIRQ_DMA_MAX)		// 37 + 12 = 49
#define	EIRQ_GPIO_MAX	(EIRQ_GPIO_BASE + NUM_GPIO)

#define IRQ_MAX		(EIRQ_GPIO_MAX) 

//#define	IRQ_ETHERNET	32

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	ICSR;           /* 0,  32 R   */
	unsigned int    ICMR;           /* 4,  32 R/W */
	unsigned int	ICMSR;          /* 8,  32 W   */
	unsigned int	ICMCR;          /* C,  32 W   */
	unsigned int	ICPR;           /* 10, 32 R   */
} JZ4755_INTC, *PJZ4755_INTC;

typedef struct __INTR_IST__
{
	unsigned int		dwHwIntr;
	unsigned int		dwSwIntr;
	HANDLE		hIntrEvent;
	HANDLE		hIntrIST;
	unsigned int		dwIntrISTPriority;
	unsigned int		bISTExist;
	unsigned int		dwVK1;
	unsigned int		dwVK2;
} IntrIST, *PIntrIST;

typedef struct __INTR_IST_EX__
{
	unsigned int		dwGpioPin;
	unsigned int		dwGpioGrp;
	unsigned int		dwGpioBit;
	unsigned int		dwGpioTriggerType;

	unsigned int		dwHwIntr;
	unsigned int		dwSwIntr;
	HANDLE		hIntrEvent;
	HANDLE		hIntrIST;
	unsigned int		dwIntrISTPriority;
	unsigned int		bISTExist;
} IntrISTEx, *PIntrISTEx;

typedef struct __GPIO_IRQ_INFO__
{
	void *			pDescription;
	unsigned int	dwHwIrq;
	unsigned int	dwBit;
	unsigned int	dwReserved;
	unsigned int	dwIntcMskBit;
	unsigned int	dwGrp;
	unsigned int	bReserved;
} GPIO_IRQ_INFO, *PGPIO_IRQ_INFO;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755INTC_H__
