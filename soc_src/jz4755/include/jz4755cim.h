//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2009.
//

#ifndef __JZ4755CIM_H__
#define __JZ4755CIM_H__

//--------------------------------------------------------------------------
// Camera Interface Module(CIM) Address Definition
//--------------------------------------------------------------------------
#define CIM_PHYS_ADDR			( 0x13060000 )
#define CIM_BASE_U_VIRTUAL		( 0xB3060000 )


//--------------------------------------------------------------------------
// CIM Registers Offset Definition
//--------------------------------------------------------------------------
#define CIMCFG_OFFSET			0x00
#define CIMCR_OFFSET			0x04
#define CIMST_OFFSET			0x08
#define CIMIID_OFFSET			0x0C
#define CIMRXFIFO_OFFSET		0x10
#define CIMDA_OFFSET			0x20
#define CIMFA_OFFSET			0x24
#define CIMFID_OFFSET			0x28
#define CIMCMD_OFFSET			0x2C
#define CIMSIZE_OFFSET			0x30
#define CIMOFFSET_OFFSET		0x34

//--------------------------------------------------------------------------
// CIM Registers Address Definition
//--------------------------------------------------------------------------
#define A_CIMCFG			( CIM_BASE_U_VIRTUAL + CIMCFG_OFFSET )
#define A_CIMCR				( CIM_BASE_U_VIRTUAL + CIMCR_OFFSET )
#define A_CIMST				( CIM_BASE_U_VIRTUAL + CIMST_OFFSET )
#define A_CIMIID			( CIM_BASE_U_VIRTUAL + CIMIID_OFFSET )
#define A_CIMRXFIFO			( CIM_BASE_U_VIRTUAL + CIMRXFIFO_OFFSET )
#define A_CIMDA				( CIM_BASE_U_VIRTUAL + CIMDA_OFFSET )
#define A_CIMFA				( CIM_BASE_U_VIRTUAL + CIMFA_OFFSET )
#define A_CIMFID			( CIM_BASE_U_VIRTUAL + CIMFID_OFFSET )
#define A_CIMCMD			( CIM_BASE_U_VIRTUAL + CIMCMD_OFFSET )
#define A_CIMSIZE			( CIM_BASE_U_VIRTUAL + CIMSIZE_OFFSET )
#define A_CIMOFFSET			( CIM_BASE_U_VIRTUAL + CIMOFFSET_OFFSET )


//--------------------------------------------------------------------------
// Common Define
//--------------------------------------------------------------------------
// CIM Configuration Register(CIMCFG) Common Define

#define SHIFT_ORDER	( 18 )		// bit18 - bit19
#define CIMCFG_ORDER_MASK	( 0x3 << SHIFT_ORDER )

#define	CIMCFG_ORDER_YCbCr	( 0x0 << SHIFT_ORDER )
#define	CIMCFG_ORDER_YCrCb	( 0x1 << SHIFT_ORDER )
#define	CIMCFG_ORDER_CbCrY	( 0x2 << SHIFT_ORDER )
#define	CIMCFG_ORDER_CrCbY	( 0x3 << SHIFT_ORDER )


#define SHIFT_DF	( 16 )		// bit16 - bit17
#define CIMCFG_DF_MASK	( 0x3 << SHIFT_DF)

#define	CIMCFG_DF_RESERVED		( 0x0 << SHIFT_DF )
#define	CIMCFG_DF_YCbCr444		( 0x1 << SHIFT_DF )
#define	CIMCFG_DF_YCbCr422		( 0x2 << SHIFT_DF )
#define	CIMCFG_DF_ITU656_YCbCr422	( 0x3 << SHIFT_DF )


#define	CIMCFG_INV_DAT		( 1 << 15 )
#define	CIMCFG_VSP			( 1 << 14 )
#define	CIMCFG_HSP			( 1 << 13 )
#define	CIMCFG_PCP			( 1 << 12 )

#define SHIFT_BURST_TYPE	( 10 )		// bit10 - bit11
#define CIMCFG_BURST_MASK	( 0x3 << SHIFT_BURST_TYPE )

#define	CIMCFG_BURST_INCR4	( 0x0 << SHIFT_BURST_TYPE )
#define	CIMCFG_BURST_INCR8	( 0x1 << SHIFT_BURST_TYPE )
#define	CIMCFG_BURST_INCR16	( 0x2 << SHIFT_BURST_TYPE )


#define	CIMCFG_DUMMY		( 1 << 9 )
#define	CIMCFG_EXT_VSYNC	( 1 << 8 )	// In CCIR656 Mode Only

#define SHIFT_PACK			( 0x4 )		// bit4 - bit6
#define CIMCFG_PACK_MASK	( 0x7 << SHIFT_PACK )

#define	CIMCFG_PACK_1234	( 0x0 << SHIFT_PACK )
#define	CIMCFG_PACK_2341	( 0x1 << SHIFT_PACK )
#define	CIMCFG_PACK_3412	( 0x2 << SHIFT_PACK )
#define	CIMCFG_PACK_4123	( 0x3 << SHIFT_PACK )
#define	CIMCFG_PACK_4321	( 0x4 << SHIFT_PACK )
#define	CIMCFG_PACK_3214	( 0x5 << SHIFT_PACK )
#define	CIMCFG_PACK_2143	( 0x6 << SHIFT_PACK )
#define	CIMCFG_PACK_1432	( 0x7 << SHIFT_PACK )

#define	CIMCFG_BYPASS		( 1 << 2 )

#define	SHIFT_DSM			( 0x0 )
#define	CIMCFG_DSM_MASK		( 0x3 << SHIFT_DSM )

#define	CIMCFG_DSM_CCIR656_PRO	( 0x0 << SHIFT_DSM )
#define	CIMCFG_DSM_CCIR656_INT	( 0x1 << SHIFT_DSM )
#define	CIMCFG_DSM_GATED_CLK	( 0x2 << SHIFT_DSM )
#define	CIMCFG_DSM_RESERVED		( 0x3 << SHIFT_DSM )

// CIM Control Register(CIMCR) Common Define
#define	SHIFT_EEOF_LINE			( 20 )
#define	CIMCR_EEOF_LINE(n)			( (n) << SHIFT_EEOF_LINE )
#define	CIMCR_EEOF_LINE_MASK		( 0xFFF << SHIFT_EEOF_LINE )

#define	SHIFT_FRC				( 16 )
#define	CIMCR_FRC_MASK			( 0xF << SHIFT_FRC )
#define	CIMCR_FRC_1OF(n)		( ((n) - 1) << SHIFT_FRC )

#define	CIMCR_VDDM_INT_EN		( 1 << 13 )
#define	CIMCR_DMA_SOFM_INT_EN	( 1 << 12 )
#define	CIMCR_DMA_EOFM_INT_EN	( 1 << 11 )
#define	CIMCR_DMA_STOPM_INT_EN	( 1 << 10 )
#define	CIMCR_RXF_TRIG_INT_EN	( 1 << 9 )
#define	CIMCR_RXF_OFM_INT_EN	( 1 << 8 )
#define	CIMCR_DMA_SYNC			( 1 << 7 )

#define	SHIFT_RXF_TRIG			( 3 )
#define	CIMCR_RXF_TRIG_MASK		( 0xF << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_1BURST	( 0x0 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_2BURST	( 0x1 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_3BURST	( 0x2 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_4BURST	( 0x3 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_5BURST	( 0x4 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_6BURST	( 0x5 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_7BURST	( 0x6 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_8BURST	( 0x7 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_9BURST	( 0x8 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_10BURST	( 0x9 << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_11BURST	( 0xA << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_12BURST	( 0xB << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_13BURST	( 0xC << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_14BURST	( 0xD << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_15BURST	( 0xE << SHIFT_RXF_TRIG )
#define	CIMCR_RXF_TRIG_16BURST	( 0xF << SHIFT_RXF_TRIG )

#define	CIMCR_DMA_EN			( 1 << 2 )
#define	CIMCR_RXF_RST			( 1 << 1 )
#define	CIMCR_CIM_EN			( 1 << 0 )

// CIM Status Register(CIMST) Common Define
#define	CIMSR_DMA_EEOF			( 1 << 7 )
#define	CIMSR_DMA_SOF			( 1 << 6 )	// NOTES: Write 0 to clear
#define	CIMSR_DMA_EOF			( 1 << 5 )
#define	CIMSR_DMA_STOP			( 1 << 4 )
#define	CIMSR_DMA_OF			( 1 << 3 )
#define	CIMSR_DMA_RF_TRIG		( 1 << 2 )
#define	CIMSR_DMA_RF_EMPTY		( 1 << 1 )
#define	CIMSR_DMA_VDD			( 1 << 0 )

// CIM Interrupt ID Register(CIMIID) Common Define

// CIM RXFIFO Register(CIMRXFIFO) Common Define

// CIM Descriptor Address Register(CIMDA) Common Define

// CIM Frame Buffer Address Register(CIMDA) Common Define

// CIM DMA Command Register(CIMCMD) Common Define
#define	CIMCMD_SOFINT			( 1 << 31 )
#define	CIMCMD_EOFINT			( 1 << 30 )
#define	CIMCMD_EEOFINT			( 1 << 29 )
#define	CIMCMD_STOP				( 1 << 28 )
#define	CIMCMD_OFRCVEN			( 1 << 27 )

#define	SHIFT_LEN				( 0 )
#define	CIMCMD_LEN_MASK			( 0xFFFFFF )

#define	CIMSIZE(lpf,ppl)					((lpf << 16)|(ppl))
#define	CIMOFFSET(v_offset,h_offset)		((v_offset << 16)|(h_offset))

#define	CIM_GPIO_PIN		( GPIO_0 | GPIO_1 | GPIO_2 | GPIO_3 | GPIO_4 | GPIO_5		\
							| GPIO_6 | GPIO_7 | GPIO_8 | GPIO_9 | GPIO_10 | GPIO_11 )

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	CIMCFG;			// 0x00, 32, RW
	unsigned int	CIMCR;			// 0x04, 32, RW
	unsigned int	CIMST;			// 0x08, 32, RW
	unsigned int	CIMIID;			// 0x0C, 32, R
	unsigned int	CIMRESERVED[4];	// 0x10, 32, R
	unsigned int	CIMDA;			// 0x20, 32, RW
	unsigned int	CIMFA;			// 0x24, 32, R
	unsigned int	CIMFID;			// 0x28, 32, R
	unsigned int	CIMCMD;			// 0x2C, 32, R
	unsigned int	CIMSIZE;		// 0x30, 32, R
	unsigned int	CIMOFFSET;		// 0x34, 32, R
} JZ4755_CIM, *PJZ4755_CIM;

typedef struct __CAMERA_DMA_DESCRIPTOR__
{
	unsigned int	dwDescriptorPhyAddr;
	unsigned int	dwFrameBufferPhyAddr;
	unsigned int	dwFrameID;
	unsigned int	dwDMACommand;
} CAMERA_DMA_DESCRIPTOR, *PCAMERA_DMA_DESCRIPTOR;

typedef struct __CAMERA_PHY_ADDR__
{
	unsigned int	dwDescriptorPhyAddr;
	unsigned int	dwDescriptorSize;
	unsigned int	dwFrameBufferPhyAddr;
	unsigned int	dwFrameBufferSize;
	unsigned int	dwDisplayBufferPhyAddr;
	unsigned int	dwDisplayBufferSize;
} CAMERA_PHY_ADDR, *PCAMERA_PHY_ADDR;

#define __gpio_as_cim()							\
do														\
{														\
	OUTREG32(A_GPIO_PXFUNS(4), CIM_GPIO_PIN);			\
	OUTREG32(A_GPIO_PXSELC(4), CIM_GPIO_PIN);			\
	OUTREG32(A_GPIO_PXTRGC(4), CIM_GPIO_PIN);			\
	OUTREG32(A_GPIO_PXPES(4),  CIM_GPIO_PIN);			\
} while (0)

#endif // __MIPS_ASSEMBLER

#endif // __JZ4755CIM_H__
