//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755UDC_H__
#define __JZ4755UDC_H__

//--------------------------------------------------------------------------
// UDC Address Definition
//--------------------------------------------------------------------------
#define UDC_PHYS_ADDR			( 0x13040000 )
#define UDC_BASE_U_VIRTUAL		( 0xB3040000 )


//--------------------------------------------------------------------------
// UDC Registers Offset Definition
//--------------------------------------------------------------------------
// Common Registers
#define UDC_FADDR_OFFSET		( 0x00 )	// RW, 8, Function Address
#define UDC_POWER_OFFSET		( 0x01 )	// RW, 8, Power Management

#define UDC_INTRIN_OFFSET		( 0x02 )	// R ,16, Interrupt IN
#define UDC_INTROUT_OFFSET		( 0x04 )	// R ,16, Interrupt OUT
#define UDC_INTRINE_OFFSET		( 0x06 )	// RW,16, Interrupt IN ENABLE
#define UDC_INTROUTE_OFFSET		( 0x08 )	// RW,16, Interrupt OUT ENABLE

#define UDC_INTRUSB_OFFSET		( 0x0A )	//  W, 8, Interrupt USB
#define UDC_INTRUSBE_OFFSET		( 0x0B )	// RW, 8, Interrupt USB ENABLE

#define UDC_FRAME_OFFSET		( 0x0C )	// R ,16, Frame
#define UDC_INDEX_OFFSET		( 0x0E )	// RW, 8, Index
#define UDC_TESTMODE_OFFSET		( 0x0F )	// RW, 8, Test Mode

// Indexed Registers
#define UDC_CSR0_OFFSET			( 0x12 )	// RW, 8, CSR0
#define UDC_COUNT0_OFFSET		( 0x18 )	// RW, 8, Count0

#define UDC_INMAXP_OFFSET		( 0x10 )	// RW,16, INMAXP
#define UDC_INCSR_OFFSET		( 0x12 )	// RW,16, INCSR
//#define USB_REG_INCSRH		(USB_BASE + 0x13) /* EP1-2 IN CSR MSB 8-bit */
#define UDC_OUTMAXP_OFFSET		( 0x14 )	// RW,16, OUTMAXP
#define UDC_OUTCSR_OFFSET		( 0x16 )	// R ,16, OUTCSR
//#define USB_REG_OUTCSRH		(USB_BASE + 0x17) /* EP1 OUT CSR MSB 8-bit */
#define UDC_OUTCOUNT_OFFSET		( 0x18 )	// R ,16, OUTCOUNT

#define UDC_FIFO_EP0_OFFSET		( 0x20 )	// RW,32
#define UDC_FIFO_EP1_OFFSET		( 0x24 )	// RW,32	BULK: IN & OUT
#define UDC_FIFO_EP2_OFFSET		( 0x28 )	// RW,32	INTR: IN & OUT


// Additional Registers
#define UDC_EPINFO_OFFSET		( 0x78 )	// R , 8
#define UDC_RAMINFO_OFFSET		( 0x79 )	// R , 8

#define	UDC_INTR_OFFSET			( 0x200 )	// DMA pending interrupts
#define	UDC_CNTL1_OFFSET		( 0x204 )	// DMA channel 1 control
#define	UDC_ADDR1_OFFSET		( 0x208 )	// DMA channel 1 AHB memory addr
#define	UDC_COUNT1_OFFSET		( 0x20C )	// DMA channel 1 byte count
#define	UDC_CNTL2_OFFSET		( 0x214 )	// DMA channel 2 control
#define	UDC_ADDR2_OFFSET		( 0x218 )	// DMA channel 2 AHB memory addr
#define	UDC_COUNT2_OFFSET		( 0x21C )	// DMA channel 2 byte count

//--------------------------------------------------------------------------
// UDC Registers Address Definition
//--------------------------------------------------------------------------
#define A_UDC_FADDR		( UDC_BASE_U_VIRTUAL + UDC_FADDR_OFFSET )	// 8
#define A_UDC_POWER		( UDC_BASE_U_VIRTUAL + UDC_POWER_OFFSET )	// 8

#define A_UDC_INTRIN	( UDC_BASE_U_VIRTUAL + UDC_INTRIN_OFFSET )	// 16
#define A_UDC_INTROUT	( UDC_BASE_U_VIRTUAL + UDC_INTROUT_OFFSET )	// 16

#define A_UDC_INTRINE	( UDC_BASE_U_VIRTUAL + UDC_INTRINE_OFFSET )	// 16
#define A_UDC_INTROUTE	( UDC_BASE_U_VIRTUAL + UDC_INTROUTE_OFFSET )// 16

#define A_UDC_INTRUSB	( UDC_BASE_U_VIRTUAL + UDC_INTRUSB_OFFSET )	// 8
#define A_UDC_INTRUSBE	( UDC_BASE_U_VIRTUAL + UDC_INTRUSBE_OFFSET )// 8

#define A_UDC_FRAME		( UDC_BASE_U_VIRTUAL + UDC_FRAME_OFFSET )	// 16

#define A_UDC_INDEX		( UDC_BASE_U_VIRTUAL + UDC_INDEX_OFFSET )	// 8
#define A_UDC_TESTMODE	( UDC_BASE_U_VIRTUAL + UDC_TESTMODE_OFFSET )// 8

#define A_UDC_CSR0		( UDC_BASE_U_VIRTUAL + UDC_CSR0_OFFSET )	// 8
#define A_UDC_COUNT0	( UDC_BASE_U_VIRTUAL + UDC_COUNT0_OFFSET )	// 8

#define A_UDC_INMAXP	( UDC_BASE_U_VIRTUAL + UDC_INMAXP_OFFSET )	// 16
#define A_UDC_INCSR		( UDC_BASE_U_VIRTUAL + UDC_INCSR_OFFSET )	// 16

#define A_UDC_OUTMAXP	( UDC_BASE_U_VIRTUAL + UDC_OUTMAXP_OFFSET )	// 16
#define A_UDC_OUTCSR	( UDC_BASE_U_VIRTUAL + UDC_OUTCSR_OFFSET )	// 16

#define A_UDC_OUTCOUNT	( UDC_BASE_U_VIRTUAL + UDC_OUTCOUNT_OFFSET )// 16

#define A_UDC_FIFO_EP0	( UDC_BASE_U_VIRTUAL + UDC_FIFO_EP0_OFFSET )// 16
#define A_UDC_FIFO_EP1	( UDC_BASE_U_VIRTUAL + UDC_FIFO_EP1_OFFSET )// 16
#define A_UDC_FIFO_EP2	( UDC_BASE_U_VIRTUAL + UDC_FIFO_EP2_OFFSET )// 16
                        
#define A_UDC_EPINFO	( UDC_BASE_U_VIRTUAL + UDC_EPINFO_OFFSET )  
#define A_UDC_RAMINFO	( UDC_BASE_U_VIRTUAL + UDC_RAMINFO_OFFSET )	
                               
#define	A_UDC_INTR		( UDC_BASE_U_VIRTUAL + UDC_INTR_OFFSET )	
#define	A_UDC_CNTL1	    ( UDC_BASE_U_VIRTUAL + UDC_CNTL1_OFFSET	)	
#define	A_UDC_ADDR1	    ( UDC_BASE_U_VIRTUAL + UDC_ADDR1_OFFSET	)	
#define	A_UDC_COUNT1	( UDC_BASE_U_VIRTUAL + UDC_COUNT1_OFFSET )	
#define	A_UDC_CNTL2	    ( UDC_BASE_U_VIRTUAL + UDC_CNTL2_OFFSET	)	
#define	A_UDC_ADDR2	    ( UDC_BASE_U_VIRTUAL + UDC_ADDR2_OFFSET	)	
#define	A_UDC_COUNT2    ( UDC_BASE_U_VIRTUAL + UDC_COUNT2_OFFSET )	


//--------------------------------------------------------------------------
// Power Common Define
//--------------------------------------------------------------------------
// Power register bit masks
#define USB_POWER_SUSPENDM	0x01
#define USB_POWER_RESUME	0x04
#define USB_POWER_HSMODE	0x10
#define USB_POWER_HSENAB	0x20
#define USB_POWER_SOFTCONN	0x40

// Interrupt register bit masks
#define USB_INTR_SUSPEND	0x01
#define USB_INTR_RESUME		0x02
#define USB_INTR_RESET		0x04

#define USB_INTR_EP0		0x0001
#define USB_INTR_INEP1		0x0002
#define USB_INTR_INEP2		0x0004
#define USB_INTR_OUTEP1		0x0002

/* CSR0 bit masks */
#define USB_CSR0_OUTPKTRDY	0x01
#define USB_CSR0_INPKTRDY	0x02
#define USB_CSR0_SENTSTALL	0x04
#define USB_CSR0_DATAEND	0x08
#define USB_CSR0_SETUPEND	0x10
#define USB_CSR0_SENDSTALL	0x20
#define USB_CSR0_SVDOUTPKTRDY	0x40
#define USB_CSR0_SVDSETUPEND	0x80

/* Endpoint CSR register bits */
#define USB_INCSRH_AUTOSET	0x80
#define USB_INCSRH_ISO		0x40
#define USB_INCSRH_MODE		0x20
#define USB_INCSRH_DMAREQENAB	0x10
#define USB_INCSRH_DMAREQMODE	0x04
#define USB_INCSR_CDT		0x40
#define USB_INCSR_SENTSTALL	0x20
#define USB_INCSR_SENDSTALL	0x10
#define USB_INCSR_FF		0x08
#define USB_INCSR_UNDERRUN	0x04
#define USB_INCSR_FFNOTEMPT	0x02
#define USB_INCSR_INPKTRDY	0x01
#define USB_OUTCSRH_AUTOCLR	0x80
#define USB_OUTCSRH_ISO		0x40
#define USB_OUTCSRH_DMAREQENAB	0x20
#define USB_OUTCSRH_DNYT	0x10
#define USB_OUTCSRH_DMAREQMODE	0x08
#define USB_OUTCSR_CDT		0x80
#define USB_OUTCSR_SENTSTALL	0x40
#define USB_OUTCSR_SENDSTALL	0x20
#define USB_OUTCSR_FF		0x10
#define USB_OUTCSR_DATAERR	0x08
#define USB_OUTCSR_OVERRUN	0x04
#define USB_OUTCSR_FFFULL	0x02
#define USB_OUTCSR_OUTPKTRDY	0x01

/* Testmode register bits */
#define USB_TEST_SE0NAK		0x01
#define USB_TEST_J		0x02
#define USB_TEST_K		0x04
#define USB_TEST_PACKET		0x08

/* DMA control bits */
#define USB_CNTL_ENA		0x01
#define USB_CNTL_DIR_IN		0x02
#define USB_CNTL_MODE_1		0x04
#define USB_CNTL_INTR_EN	0x08
#define USB_CNTL_EP(n)		((n) << 4)
#define USB_CNTL_BURST_0	(0 << 9)
#define USB_CNTL_BURST_4	(1 << 9)
#define USB_CNTL_BURST_8	(2 << 9)
#define USB_CNTL_BURST_16	(3 << 9)

//--------------------------------------------------------------------------
// XXX Registers Address Definition
//--------------------------------------------------------------------------
#define A_XXX_XXXXX			( UDC_BASE_U_VIRTUAL + XXX_XXXXX_OFFSET )

//--------------------------------------------------------------------------
// XXXXX(XXXXX) Common Define
//--------------------------------------------------------------------------
#define XXXXX_YYYYY			( 0 << 31 )

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned char	FADDR;				/* 0x00,  8 R/W   */
	unsigned char	POWER;				/* 0x01,  8 R/W */
	unsigned short	INTRIN;				/* 0x02,  16 R/W */

	unsigned short	INTROUT;			/* 0x04,  16 RW   */
	unsigned short	INTRINE;			/* 0x06,  16 RW   */

	unsigned short	INTROUTE;			/* 0x08,  16 RW   */
	unsigned char	INTRUSB;			/* 0x0A,  8  RW   */
	unsigned char	INTRUSBE;			/* 0x0B,  8  RW   */

	unsigned short	FRAME;				/* 0x0C,  16 RW   */
	unsigned char	INDEX;				/* 0x0E,  8  RW   */
	unsigned char	TESTMODE;			/* 0x0F,  8  RW   */

	unsigned short	INMAXP;				/* 0x10,  16 RW   */
	unsigned short	INCSR;				/* 0x12,  16 RW   */

	unsigned short	OUTMAXP;			/* 0x14,  16 RW   */
	unsigned short	OUTCSR;				/* 0x16,  16 RW   */

	unsigned short	OUTCOUNT;			/* 0x18,  16 RW   */
	unsigned short	RSV00;				/* 0x1A,  16 RW   */

	unsigned int	RSV01;				/* 0x1C,  32 RW   */

	unsigned int	RSV02[4];			/* 0x20,  32 RW   */
	unsigned int	RSV03[4];			/* 0x30,  32 RW   */
	unsigned int	RSV04[4];			/* 0x40,  32 RW   */
	unsigned int	RSV05[4];			/* 0x50,  32 RW   */
	unsigned int	RSV06[4];			/* 0x60,  32 RW   */

	unsigned int	RSV07[2];			/* 0x70,  32 RW   */

	unsigned char	EPINFO;				/* 0x78,  8  RW   */
	unsigned char	RAMINFO;			/* 0x79,  8  RW   */
	unsigned short	RSV08;				/* 0x7A,  16 RW   */

} JZ4755_UDC, *PJZ4755_UDC;

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755UDC_H__
