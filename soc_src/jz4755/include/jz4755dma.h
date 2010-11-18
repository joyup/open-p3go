//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4755DMA_H__
#define __JZ4755DMA_H__

//--------------------------------------------------------------------------
// DMA Address Definition
//--------------------------------------------------------------------------
#define DMA_PHYS_ADDR			( 0x13020000 )
#define DMA_BASE_U_VIRTUAL		( 0xB3020000 )

//--------------------------------------------------------------------------
// DMA Registers Offset Definition
//--------------------------------------------------------------------------

#define DMA_DSA_OFFSET			( 0x00 )        // 32, R/W, DMA source address register 
#define DMA_DTA_OFFSET			( 0x04 )        // 32, R/W, DMA destination address register 
#define DMA_DTC_OFFSET			( 0x08 )        // 32, R/W, DMA transfer count register 
#define DMA_DRT_OFFSET			( 0x0C )        // 32, R/W, DMA request source register
#define DMA_DCS_OFFSET			( 0x10 )        // 32, R/W, DMA channel control/status register
#define DMA_DCM_OFFSET			( 0x14 )        // 32, R/W, DMA command register
#define DMA_DDA_OFFSET			( 0x18 )        // 32, R/W, DMA descriptor address register

#define DMA_DMAC_OFFSET			( 0x00 )		// 32, R/W, DMA control Register.
#define DMA_DIRQP_OFFSET		( 0x04 )		// 32, R/W, DMA interrupt pending Register.
#define DMA_DDR_OFFSET			( 0x08 )		// 32, R/W, DMA doorbell Register.
#define DMA_DDRS_OFFSET			( 0x0C )		// 32, R/W, DMA doorbell set Register.
#define DMA_DCKE_OFFSET			( 0x10 )		// 32, R/W, DMA clock enable Register.

//--------------------------------------------------------------------------
// DMA Registers Address Definition
//--------------------------------------------------------------------------
#define A_DMA_DSA(x)				( DMA_BASE_U_VIRTUAL + DMA_DSA_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DTA(x)				( DMA_BASE_U_VIRTUAL + DMA_DTA_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) )
#define A_DMA_DTC(x)				( DMA_BASE_U_VIRTUAL + DMA_DTC_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) )  
#define A_DMA_DRT(x)				( DMA_BASE_U_VIRTUAL + DMA_DRT_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DCS(x)				( DMA_BASE_U_VIRTUAL + DMA_DCS_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DCM(x)				( DMA_BASE_U_VIRTUAL + DMA_DCM_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DDA(x)				( DMA_BASE_U_VIRTUAL + DMA_DDA_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
                    				
#define A_DMA_DSD(x)				( DMA_BASE_U_VIRTUAL + 0xc0 + 0x4*(x) + 0x100*((x)/6) )
 
#define A_DMA_DMAC(x)				( DMA_BASE_U_VIRTUAL + DMA_DMAC_OFFSET  + 0x100*((x)+3) )
#define A_DMA_DIRQP(x)				( DMA_BASE_U_VIRTUAL + DMA_DIRQP_OFFSET + 0x100*((x)+3) )
#define A_DMA_DDR(x)				( DMA_BASE_U_VIRTUAL + DMA_DDR_OFFSET  + 0x100*((x)+3) )
#define A_DMA_DDRS(x)				( DMA_BASE_U_VIRTUAL + DMA_DDRS_OFFSET + 0x100*((x)+3) ) 
#define A_DMA_DCKE(x)				( DMA_BASE_U_VIRTUAL + DMA_DCKE_OFFSET + 0x100*((x)+3) )

//------------------------------------------------------------------------------
// Register DRT Register field descriptions
//------------------------------------------------------------------------------

#define DRT_DREQn			( 0 )
#define DRT_NAND			( 1 )
#define DRT_BCH_EN			( 2 )
#define DRT_BCH_DE			( 3 )
#define	DRT_AUTO			( 8 )
#define DRT_TSSI			( 9 )
#define	DRT_UART3_TX		( 14 )
#define	DRT_UART3_RX		( 15 )
#define	DRT_UART2_TX		( 16 )
#define	DRT_UART2_RX		( 17 )
#define	DRT_UART1_TX		( 18 )
#define	DRT_UART1_RX		( 19 )
#define	DRT_UART0_TX		( 20 )
#define	DRT_UART0_RX		( 21 )
#define	DRT_SSI_TX			( 22 )
#define	DRT_SSI_RX			( 23 )
#define	DRT_AIC_TX			( 24 )
#define	DRT_AIC_RX			( 25 )
#define	DRT_MSC0_TX			( 26 )
#define	DRT_MSC0_RX			( 27 )
#define	DRT_TCU				( 28 )
#define	DRT_SADC			( 29 )
#define	DRT_MSC1_TX			( 30 )
#define	DRT_MSC1_RX			( 31 )
#define	DRT_SSI1_TX			( 32 )
#define	DRT_SSI1_RX			( 33 )
#define	DRT_PM_TX			( 34 )
#define	DRT_PM_RX			( 35 )


//------------------------------------------------------------------------------
// Register DCS Register field descriptions
//------------------------------------------------------------------------------
#define	DCS_NDES			( 1 << 31 )
#define	DCS_DES8			( 1 << 30 )
#define	DCS_CDOA			( 1 << 16 )
#define DCS_BERR			( 1 << 7 )
#define	DCS_INV				( 1 << 6 )
#define	DCS_AR				( 1 << 4 )
#define	DCS_TT				( 1 << 3 )
#define	DCS_HLT				( 1 << 2 )
#define	DCS_CT				( 1 << 1 )
#define	DCS_CTE				( 1 << 0 )


//------------------------------------------------------------------------------
// Register DCM Register field descriptions
//------------------------------------------------------------------------------
#define DCM_EACKS			( 1 << 31 )
#define DCM_EACKM			( 1 << 30 )
#define DCM_ERDM_LL			( 0 << 28 )
#define DCM_ERDM_FE			( 1 << 28 )
#define DCM_ERDM_HL			( 2 << 28 )
#define DCM_ERDM_RE			( 3 << 28 )

#define DCM_BLAST			( 1 << 25 )

#define	DCM_SAI				( 1 << 23 )
#define	DCM_DAI				( 1 << 22 )

#define	DCM_RDIL_IGN		( 0 << 16 )
#define	DCM_RDIL_UNIT2		( 1 << 16 )
#define	DCM_RDIL_UNIT4		( 2 << 16 )
#define	DCM_RDIL_UNIT8		( 3 << 16 )
#define	DCM_RDIL_UNIT12		( 4 << 16 )
#define	DCM_RDIL_UNIT16		( 5 << 16 )
#define	DCM_RDIL_UNIT20		( 6 << 16 )
#define	DCM_RDIL_UNIT24		( 7 << 16 )
#define	DCM_RDIL_UNIT28		( 8 << 16 )
#define	DCM_RDIL_UNIT32		( 9 << 16 )
#define	DCM_RDIL_UNIT48		( 10 << 16 )
#define	DCM_RDIL_UNIT60		( 11 << 16 )
#define	DCM_RDIL_UNIT64		( 12 << 16 )
#define	DCM_RDIL_UNIT124		( 13 << 16 )
#define	DCM_RDIL_UNIT128		( 14 << 16 )
#define	DCM_RDIL_UNIT200		( 15 << 16 )

#define DCM_SP_MASK				( 3 << 14 )
#define	DCM_SP_32BIT			( 0 << 14 )
#define	DCM_SP_08BIT			( 1 << 14 )
#define	DCM_SP_16BIT			( 2 << 14 )

#define DCM_DP_MASK					( 3 << 12 )
#define	DCM_DP_32BIT				( 0 << 12 )
#define	DCM_DP_08BIT				( 1 << 12 )
#define	DCM_DP_16BIT				( 2 << 12 )

#define DCM_TSZ_MASK			( 0x7 << 8)
#define	DCM_TSZ_32BIT			( 0 << 8 )
#define	DCM_TSZ_08BIT			( 1 << 8 )
#define	DCM_TSZ_16BIT			( 2 << 8 )
#define	DCM_TSZ_16BYTE			( 3 << 8 )
#define	DCM_TSZ_32BYTE			( 4 << 8 )

#define	DCM_BLOCK_MODE			( 1 << 7 )
#define	DCM_SINGLE_MODE			( 0 << 7 )
#define DCD_STRIDE_EN			( 1 << 5 )
#define	DCM_DESC_VALID			( 1 << 4 )
#define	DCM_DESC_VALID_MODE		( 1 << 3 )
#define	DCM_VALID_INTR_EN		( 1 << 2 )
#define	DCM_TRANS_INTR_EN		( 1 << 1 )
#define	DCM_LINK_EN				( 1 << 0 )

//------------------------------------------------------------------------------
// Register DMAC field descriptions
//------------------------------------------------------------------------------
#define DMAC_FAST_MSC		( 1 << 31 )
#define DMAC_FAST_SSI		( 1 << 30 )
#define DMAC_FAST_TSSI		( 1 << 29 )
#define DMAC_FAST_UART		( 1 << 28 )
#define DMAC_FAST_AIC		( 1 << 27 )

#define	DMAC_PM_0			( 0 << 8 )
#define	DMAC_PM_1			( 1 << 8 )
#define	DMAC_PM_2			( 2 << 8 )
#define	DMAC_PM_3			( 3 << 8 )

#define	DMAC_HALT			( 1 << 3 )
#define	DMAC_ADDR_ERR		( 1 << 2 )
#define	DMAC_INT_EN			( 1 << 1 )
#define	DMAC_DMA_EN			( 1 << 0 )

//------------------------------------------------------------------------------
// Register DIRQP field descriptions
//------------------------------------------------------------------------------
#define DIRQP_CIRQ5			( 1 << 5 )
#define DIRQP_CIRQ4			( 1 << 4 )
#define DIRQP_CIRQ3			( 1 << 3 )
#define DIRQP_CIRQ2			( 1 << 2 )
#define DIRQP_CIRQ1			( 1 << 1 )
#define DIRQP_CIRQ0			( 1 << 0 )

//------------------------------------------------------------------------------
// Register DCKE field descriptions
//------------------------------------------------------------------------------
#define DMAC_DCKE_DCKE5		( 1 << 5 )
#define DMAC_DCKE_DCKE4		( 1 << 4 )
#define DMAC_DCKE_DCKE3		( 1 << 3 )
#define DMAC_DCKE_DCKE2		( 1 << 2 )
#define DMAC_DCKE_DCKE1		( 1 << 1 )
#define DMAC_DCKE_DCKE0		( 1 << 0 )
//------------------------------------------------------------------------------
// Descriptor DES0 structure descriptions
//------------------------------------------------------------------------------
#define DES_EACKS			( 1 << 31 )
#define DES_EACKM			( 1 << 30 )

#define DES_ERDM_MASK		( 3 << 28)
#define DES_ERDM_LL			( 0 << 28 )
#define DES_ERDM_FE			( 1 << 28 )
#define DES_ERDM_HL			( 2 << 28 )
#define DES_ERDM_RE			( 3 << 28 )

#define DES_EOPM			( 1 << 27 )
#define DES_BLAST			( 1 << 25 )
#define	DES_SAI				( 1 << 23 )
#define	DES_DAI				( 1 << 22 )

#define DES_RDIL_MASK		( 0xF << 16)
#define	DES_RDIL_IGN		( 0 << 16 )
#define	DES_RDIL_UNIT2		( 1 << 16 )
#define	DES_RDIL_UNIT4		( 2 << 16 )
#define	DES_RDIL_UNIT8		( 3 << 16 )
#define	DES_RDIL_UNIT12		( 4 << 16 )
#define	DES_RDIL_UNIT16		( 5 << 16 )
#define	DES_RDIL_UNIT20		( 6 << 16 )
#define	DES_RDIL_UNIT24		( 7 << 16 )
#define	DES_RDIL_UNIT28		( 8 << 16 )
#define	DES_RDIL_UNIT32		( 9 << 16 )
#define	DES_RDIL_UNIT48		( 10 << 16 )
#define	DES_RDIL_UNIT60		( 11 << 16 )
#define	DES_RDIL_UNIT64		( 12 << 16 )
#define	DES_RDIL_UNIT124	( 13 << 16 )
#define	DES_RDIL_UNIT128	( 14 << 16 )
#define	DES_RDIL_UNIT200	( 15 << 16 )

#define DES_SP_MASK			( 3 << 14)
#define	DES_SP_32BIT		( 0 << 14 )
#define	DES_SP_08BIT		( 1 << 14 )
#define	DES_SP_16BIT		( 2 << 14 )

#define DES_DP_MASK				( 3 << 12)
#define	DES_DP_32BIT			( 0 << 12 )
#define	DES_DP_08BIT			( 1 << 12 )
#define	DES_DP_16BIT			( 2 << 12 )

#define DES_TSZ_MASK			( 7 << 8)
#define	DES_TSZ_32BIT			( 0 << 8 )
#define	DES_TSZ_08BIT			( 1 << 8 )
#define	DES_TSZ_16BIT			( 2 << 8 )
#define	DES_TSZ_16BYTE			( 3 << 8 )
#define	DES_TSZ_32BYTE			( 4 << 8 )

#define	DES_TM					( 1 << 7 )
#define DCD_STRIDE_EN			( 1 << 5 )
#define	DES_DESC_VALID			( 1 << 4 )
#define	DES_DESC_VALID_MODE		( 1 << 3 )
#define	DES_VALID_INT_EN		( 1 << 2 )
#define	DES_TRANS_INT_EN		( 1 << 1 )
#define	DES_LINK_EN				( 1 << 0 )

#define	DMA_CHANNEL_OFFSET		( 0x20 )
#define	DMA_CHANNEL_NUM			( 6 )
#define DMA_CONTROLLER_OFFSET	( 0X100 )
#define DMA_CONTROLLER_NUM		( 2 )
#define NUM_DMA (DMA_CONTROLLER_NUM*DMA_CHANNEL_NUM)

#define __dmac_enable_module() SETREG32(A_DMA_DMAC(MSC_DMA_CHANNEL / 6),(DMAC_FAST_MSC | DMAC_DMA_EN));

#ifndef __MIPS_ASSEMBLER

typedef volatile struct
{
	unsigned int	DSA0;		// 0x00
	unsigned int	DTA0;		// 0x04
	unsigned int	DTC0;		// 0x08
	unsigned int	DRT0;		// 0x0C
	unsigned int	DCS0;		// 0x10
	unsigned int	DCM0;		// 0x14
	unsigned int	DDA0;		// 0x18
	unsigned int	DMA0RSV00[1];	// 0x1C

	unsigned int	DSA1;		// 0x20
	unsigned int	DTA1;		// 0x24
	unsigned int	DTC1;		// 0x28
	unsigned int	DRT1;		// 0x2C
	unsigned int	DCS1;		// 0x30
	unsigned int	DCM1;		// 0x34
	unsigned int	DDA1;		// 0x38
	unsigned int	DMA1RSV00[1];	// 0x3C

	unsigned int	DSA2;		// 0x40
	unsigned int	DTA2;		// 0x44
	unsigned int	DTC2;		// 0x48
	unsigned int	DRT2;		// 0x4C
	unsigned int	DCS2;		// 0x50
	unsigned int	DCM2;		// 0x54
	unsigned int	DDA2;		// 0x58
	unsigned int	DMA2RSV00[1];	// 0x5C

	unsigned int	DSA3;		// 0x60
	unsigned int	DTA3;		// 0x64
	unsigned int	DTC3;		// 0x68
	unsigned int	DRT3;		// 0x6C
	unsigned int	DCS3;		// 0x70
	unsigned int	DCM3;		// 0x74
	unsigned int	DDA3;		// 0x78
	unsigned int	DMA3RSV00[1];	// 0x7C

	unsigned int	DSA4;		// 0x80
	unsigned int	DTA4;		// 0x84
	unsigned int	DTC4;		// 0x88
	unsigned int	DRT4;		// 0x8C
	unsigned int	DCS4;		// 0x90
	unsigned int	DCM4;		// 0x94
	unsigned int	DDA4;		// 0x98
	unsigned int	DMA4RSV00[1];	// 0x9C

	unsigned int	DSA5;		// 0xA0
	unsigned int	DTA5;		// 0xA4
	unsigned int	DTC5;		// 0xA8
	unsigned int	DRT5;		// 0xAC
	unsigned int	DCS5;		// 0xB0
	unsigned int	DCM5;		// 0xB4
	unsigned int	DDA5;		// 0xB8
	unsigned int	DMA5RSV00[1];	// 0xBC
	
	unsigned int	DSD0;		// 0xC0
	unsigned int	DSD1;		// 0xC4
	unsigned int	DSD2;		// 0xC8
	unsigned int	DSD3;		// 0xCC
	unsigned int	DSD4;		// 0xD0
	unsigned int	DSD5;		// 0xD4
	unsigned int	DMA6RSV00[2];	// 0xDC
	unsigned int	DMA7RSV00[8];	// 0xE0, 0xFC

	unsigned int	DSA6;		// 0x100          
	unsigned int	DTA6;		// 0x104          
	unsigned int	DTC6;		// 0x108          
	unsigned int	DRT6;		// 0x10C          
	unsigned int	DCS6;		// 0x110          
	unsigned int	DCM6;		// 0x114          
	unsigned int	DDA6;		// 0x118          
	unsigned int	DMA8RSV00[1];	// 0x1C      
	                                             
	unsigned int	DSA7;		// 0x120          
	unsigned int	DTA7;		// 0x124          
	unsigned int	DTC7;		// 0x128          
	unsigned int	DRT7;		// 0x12C          
	unsigned int	DCS7;		// 0x130          
	unsigned int	DCM7;		// 0x134          
	unsigned int	DDA7;		// 0x138          
	unsigned int	DMA9RSV00[1];	// 0x13C      
	                                             
	unsigned int	DSA8;		// 0x140          
	unsigned int	DTA8;		// 0x144          
	unsigned int	DTC8;		// 0x148          
	unsigned int	DRT8;		// 0x14C          
	unsigned int	DCS8;		// 0x150          
	unsigned int	DCM8;		// 0x154          
	unsigned int	DDA8;		// 0x158          
	unsigned int	DMA10RSV00[1];	// 0x15C      
	                                             
	unsigned int	DSA9;		// 0x160          
	unsigned int	DTA9;		// 0x164          
	unsigned int	DTC9;		// 0x168          
	unsigned int	DRT9;		// 0x16C          
	unsigned int	DCS9;		// 0x170          
	unsigned int	DCM9;		// 0x174          
	unsigned int	DDA9;		// 0x178          
	unsigned int	DMA11RSV00[1];	// 0x17C      
	                                             
	unsigned int	DSA10;		// 0x180          
	unsigned int	DTA10;		// 0x184          
	unsigned int	DTC10;		// 0x188          
	unsigned int	DRT10;		// 0x18C          
	unsigned int	DCS10;		// 0x190          
	unsigned int	DCM10;		// 0x194          
	unsigned int	DDA10;		// 0x198          
	unsigned int	DMA12RSV00[1];	// 0x19C      
	                                             
	unsigned int	DSA11;		// 0x1A0          
	unsigned int	DTA11;		// 0x1A4          
	unsigned int	DTC11;		// 0x1A8          
	unsigned int	DRT11;		// 0x1AC          
	unsigned int	DCS11;		// 0x1B0          
	unsigned int	DCM11;		// 0x1B4          
	unsigned int	DDA11;		// 0x1B8
	unsigned int	DMA13RSV00[1];	// 0x1BC           
	                                             
	unsigned int	DSD6;		// 0x1C0          
	unsigned int	DSD7;		// 0x1C4          
	unsigned int	DSD8;		// 0x1C8          
	unsigned int	DSD9;		// 0x1CC          
	unsigned int	DSD10;		// 0x1D0          
	unsigned int	DSD11;		// 0x1D4          
	unsigned int	DMA14RSV00[2];	// 0x1DC      
	unsigned int	DMA15RSV00[8];	// 0x1E0, 0x1FC
	
	
	unsigned int	DMA16RSV00[64];	// 0x200, 0x2FC
    
    
	unsigned int	DMAC0;		// 0x300
	unsigned int	DIRQP0;		// 0x304
	unsigned int	DDR0;		// 0x308
	unsigned int	DDRS0;		// 0x30C
	unsigned int	DCKE0;		// 0x310
	unsigned int	DMA17RSV00[59];	// 0x314, 0x3FC
	
	
	unsigned int	DMAC1;		// 0x400
	unsigned int	DIRQP1;		// 0x404
	unsigned int	DDR1;		// 0x408
	unsigned int	DDRS1;		// 0x40C
	unsigned int	DCKE1;		// 0x410
} JZ4755_DMA, *PJZ4755_DMA;
    
typedef volatile struct
{   
	unsigned int	DSA;		// 0x00, Source Address Register, R/W, 32bit
	unsigned int	DTA;		// 0x04, Destination Address Register, R/W, 32bit
	unsigned int	DTC;		// 0x08, Transfer Count Register, R/W, 32bit (only used 24bits)
	unsigned int	DRT;		// 0x0C, Request Source Type Register, R/W, 32bit
	unsigned int	DCS;		// 0x10, DMA Channel Control / Status Register, R/W, 32bit
	unsigned int	DCM;		// 0x14, DMA Command Register, R/W, 32bit
	unsigned int	DDA;		// 0x18, DMA Descriptor Address Register, R/W, 32bit
	unsigned int	DMARSV00[1];	// 0x1C
} JZ4755_DMA_CHN, *PJZ4755_DMA_CHN;

typedef volatile struct
{   
  	unsigned int	DMAC;		// 0x00
	unsigned int	DIRQP;		// 0x04
	unsigned int	DDR;		// 0x08
	unsigned int	DDRS;		// 0x0C
	unsigned int	DCKE;		// 0x10
	unsigned int	DMARSV00[59];	// 0x14, 0xFC
} JZ4755_DMA_CTL, *PJZ4755_DMA_CTL;
 
typedef struct
{   
	JZ4755_DMA_CHN			ChnA[DMA_CHANNEL_NUM];
	volatile unsigned int	DSDA[DMA_CHANNEL_NUM];                   
	volatile unsigned int	DMA1RSV00[2];	// 0x1DC      
	volatile unsigned int	DMA2RSV00[8];	// 0x1E0, 0x1FC
	
	JZ4755_DMA_CHN			ChnB[DMA_CHANNEL_NUM];
	volatile unsigned int	DSDB[DMA_CHANNEL_NUM];         
	volatile unsigned int	DMA3RSV00[2];	// 0x1DC      
	volatile unsigned int	DMA4RSV00[8];	// 0x1E0, 0x1FC
	
	volatile unsigned int	DMA5RSV00[64];	// 0x200, 0x2FC
		
    JZ4755_DMA_CTL		Ctl[DMA_CONTROLLER_NUM];

} JZ4755_DMA2, *PJZ4755_DMA2;
    
    
    
/***************************************************************************
 * DMAC Operation
 ***************************************************************************/
    
/* n is the DMA channel (0 - 5) */
#define DMAC_DMACR_PM_BIT	8  /* channel priority mode */
#define DMAC_DMACR_PM_MASK	(0x03 << DMAC_DMACR_PM_BIT)
    
#endif // __MIPS_ASSEMBLER
    
#endif // __JZ4755DMA_H__
