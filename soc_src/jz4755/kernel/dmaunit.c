
#include <jz4755.h>

//------------------------------------------------------------------------
#define DMA_STRIDE_CPY_CHANNEL YUV_COPY_CHANNEL


//------------------------------------------------------------------------
#define PHYSADDR(x) (((unsigned int)x) & 0x1fffffff)
#define UNCACHE(x) ((unsigned int)x | 0xa0000000)
static unsigned int g_des_space[3 * 8] __attribute__( (aligned(128)));
static unsigned int *g_desc ;
//ÒªÇó32Bit¶ÔÆë

void yuv_copy_nowait(unsigned int *tar,unsigned int *src,int *t_stride,int *s_stride,int *size,int *line)
{
	int i;

	while (!(INREG32(A_DMA_DCS(DMA_STRIDE_CPY_CHANNEL)) & DCS_CT) ); //error
	for(i = 0;i < 3;i++)
	{
		g_desc[8*i+1] = PHYSADDR(src[i]);
		g_desc[8*i+2] = PHYSADDR(tar[i]);
		g_desc[8*i+3] = (((unsigned int)&g_desc[8 * (i + 1)] & 0xff0)<<20 ) | ((*(line + i))<<16) | (*(size + i) );
		g_desc[8*i+4] = ((*(t_stride + i)) << 16) | (*(s_stride + i));
	}	
	OUTREG32(A_DMA_DDA(DMA_STRIDE_CPY_CHANNEL),PHYSADDR(g_desc));
	OUTREG32(A_DMA_DRT(DMA_STRIDE_CPY_CHANNEL), DRT_AUTO);
	CLRREG32(A_DMA_DCS(DMA_STRIDE_CPY_CHANNEL),(DCS_TT | DCS_CT));
	OUTREG32(A_DMA_DDRS(DMA_STRIDE_CPY_CHANNEL / 6), (1 << (DMA_STRIDE_CPY_CHANNEL % 6)));//add
	SETREG32(A_DMA_DCS(DMA_STRIDE_CPY_CHANNEL), DCS_CTE);

	

}
void yuv_copy_nowait_init()
{
	int i;
	g_desc=(unsigned int*)(((unsigned int)(g_des_space)) | 0xa0000000);
	for(i = 0;i < 3;i++)
	{
		g_desc[i * 8 + 0] = (DES_DAI | DES_SAI | DES_SP_32BIT | DES_DP_32BIT | DES_TSZ_32BIT | DCD_STRIDE_EN | DES_LINK_EN);
		g_desc[i * 8 + 5] = DRT_AUTO;
	}
	g_desc[(--i) * 8 + 0] &= ~(DES_LINK_EN);
			
	CLRREG32(A_CPM_CLKGR, ( 1 << 12 ));
	SETREG32(A_DMA_DMAC(DMA_STRIDE_CPY_CHANNEL / 6),DMAC_DMA_EN);
	
	OUTREG32(A_DMA_DCKE(DMA_STRIDE_CPY_CHANNEL / 6),(1 << (DMA_STRIDE_CPY_CHANNEL % 6)));//Open channel clock
	CLRREG32(A_DMA_DMAC(DMA_STRIDE_CPY_CHANNEL / 6), (DMAC_HALT | DMAC_ADDR_ERR));//Ensure DMAC.AR = 0,DMAC.HLT = 0
	CLRREG32(A_DMA_DCS(DMA_STRIDE_CPY_CHANNEL), DCS_AR | DCS_HLT | DCS_TT | DCS_INV); // Ensure DCSn.AR = 0, DCSn.HLT = 0, DCSn.TT = 0, DCSn.INV = 0

	OUTREG32(A_DMA_DTC(DMA_STRIDE_CPY_CHANNEL), 0);//DTCn = 0
	CLRREG32(A_DMA_DCS(DMA_STRIDE_CPY_CHANNEL), DCS_CTE);
	CLRREG32(A_DMA_DCS(DMA_STRIDE_CPY_CHANNEL), DCS_NDES);	
	SETREG32(A_DMA_DCS(DMA_STRIDE_CPY_CHANNEL), DCS_DES8);
		
}
#define DMA_CPY_CHANNEL CODEC_COPY_CHANNEL
void dma_nowait_cpyinit()
{
	CLRREG32(A_CPM_CLKGR, ( 1 << 12 ));
	SETREG32(A_DMA_DMAC(DMA_CPY_CHANNEL / 6),DMAC_DMA_EN);
	
	OUTREG32(A_DMA_DCKE(DMA_CPY_CHANNEL / 6),(1 << (DMA_CPY_CHANNEL % 6)));//Open channel clock
	CLRREG32(A_DMA_DMAC(DMA_CPY_CHANNEL / 6), (DMAC_HALT | DMAC_ADDR_ERR));//Ensure DMAC.AR = 0,DMAC.HLT = 0
	CLRREG32(A_DMA_DCS(DMA_CPY_CHANNEL), DCS_AR | DCS_HLT | DCS_TT | DCS_INV); // Ensure DCSn.AR = 0, DCSn.HLT = 0, DCSn.TT = 0, DCSn.INV = 0

	OUTREG32(A_DMA_DTC(DMA_CPY_CHANNEL), 0);//DTCn = 0
	CLRREG32(A_DMA_DCS(DMA_CPY_CHANNEL), DCS_CTE);
	SETREG32(A_DMA_DCS(DMA_CPY_CHANNEL), DCS_NDES);
}
void dma_copy_nowait(void *tar,void *src,int size)
{
	int timeout = 0x1000000;
	
	while ((!(INREG32(A_DMA_DCS(DMA_CPY_CHANNEL)) & DCS_TT)) && (timeout--));
	CLRREG32(A_DMA_DCS(DMA_CPY_CHANNEL), DCS_CTE);
	OUTREG32(A_DMA_DSA(DMA_CPY_CHANNEL), PHYSADDR((unsigned long)src));
	OUTREG32(A_DMA_DTA(DMA_CPY_CHANNEL), PHYSADDR((unsigned long)tar));
	OUTREG32(A_DMA_DTC(DMA_CPY_CHANNEL), size / 32);	            	    
	OUTREG32(A_DMA_DRT(DMA_CPY_CHANNEL), DRT_AUTO);
	OUTREG32(A_DMA_DCM(DMA_CPY_CHANNEL), (DCM_SAI| DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT | DCM_TSZ_32BYTE));
	CLRREG32(A_DMA_DCS(DMA_CPY_CHANNEL),(DCS_TT));
	SETREG32(A_DMA_DCS(DMA_CPY_CHANNEL), DCS_CTE | DCS_NDES);
}
void dma_copy_wait(void *tar,void *src,int size)
{
	int timeout = 0x1000000;
		
	CLRREG32(A_DMA_DCS(DMA_CPY_CHANNEL), DCS_CTE);
	OUTREG32(A_DMA_DSA(DMA_CPY_CHANNEL), PHYSADDR((unsigned long)src));
	OUTREG32(A_DMA_DTA(DMA_CPY_CHANNEL), PHYSADDR((unsigned long)tar));
	OUTREG32(A_DMA_DTC(DMA_CPY_CHANNEL), size / 32);	            	    
	OUTREG32(A_DMA_DRT(DMA_CPY_CHANNEL), DRT_AUTO);
	OUTREG32(A_DMA_DCM(DMA_CPY_CHANNEL), (DCM_SAI| DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT | DCM_TSZ_32BYTE));
	CLRREG32(A_DMA_DCS(DMA_CPY_CHANNEL),(DCS_TT));
	SETREG32(A_DMA_DCS(DMA_CPY_CHANNEL), DCS_CTE | DCS_NDES);
	while ((!(INREG32(A_DMA_DCS(DMA_CPY_CHANNEL)) & DCS_TT)) && (timeout--));
	
}

#define 	DMA_NAND_COPY_CHANNEL			SSFDC_DMA_CHANNEL

void dma_nand_cpyinit()
{
	CLRREG32(A_CPM_CLKGR, ( 1 << 12 ));
	SETREG32(A_DMA_DMAC(DMA_NAND_COPY_CHANNEL / 6),DMAC_DMA_EN);
	
	OUTREG32(A_DMA_DCKE(DMA_NAND_COPY_CHANNEL / 6),(1 << (DMA_NAND_COPY_CHANNEL % 6)));//Open channel clock
	CLRREG32(A_DMA_DMAC(DMA_NAND_COPY_CHANNEL / 6), (DMAC_HALT | DMAC_ADDR_ERR));//Ensure DMAC.AR = 0,DMAC.HLT = 0
	CLRREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL), DCS_AR | DCS_HLT | DCS_TT | DCS_INV); // Ensure DCSn.AR = 0, DCSn.HLT = 0, DCSn.TT = 0, DCSn.INV = 0

	OUTREG32(A_DMA_DTC(DMA_NAND_COPY_CHANNEL), 0);//DTCn = 0
	CLRREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL), DCS_CTE);
	SETREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL), DCS_NDES);
}

void dma_nand_copy_wait(void *tar,void *src,int size)
{
	int timeout = 0x1000000;
	
	if(((unsigned int)src < 0xa0000000) && size)
		 dma_cache_wback_inv((unsigned long)src, size);
	
	if(((unsigned int)tar < 0xa0000000) && size)
		dma_cache_wback_inv((unsigned long)tar, size);
	
	CLRREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL), DCS_CTE);
	OUTREG32(A_DMA_DSA(DMA_NAND_COPY_CHANNEL), PHYSADDR((unsigned long)src));
	OUTREG32(A_DMA_DTA(DMA_NAND_COPY_CHANNEL), PHYSADDR((unsigned long)tar));
	OUTREG32(A_DMA_DTC(DMA_NAND_COPY_CHANNEL), size / 32);	            	    
	OUTREG32(A_DMA_DRT(DMA_NAND_COPY_CHANNEL), DRT_AUTO);
	OUTREG32(A_DMA_DCM(DMA_NAND_COPY_CHANNEL), (DCM_SAI| DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT | DCM_TSZ_32BYTE));
	CLRREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL),(DCS_TT));
	SETREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL), DCS_CTE | DCS_NDES);
	while ((!(INREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL)) & DCS_TT)) && (timeout--));
	
}

void dma_nand_set_wait(void *tar,unsigned char src,unsigned int size)
{
	unsigned int setdata[16];
	unsigned int *ptemp;
	ptemp = (unsigned int *)UNCACHE(((unsigned int)(&setdata)+ 31)& (~31));
	*ptemp = (unsigned int) ((src << 24) | (src << 16) | (src << 8) | src);
	
	if(((unsigned int)tar < 0xa0000000) && size)
		dma_cache_wback_inv((unsigned long)tar, size);
		
	CLRREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL), DCS_CTE);
	OUTREG32(A_DMA_DSA(DMA_NAND_COPY_CHANNEL), PHYSADDR((unsigned long)ptemp));
	OUTREG32(A_DMA_DTA(DMA_NAND_COPY_CHANNEL), PHYSADDR((unsigned long)tar));
	OUTREG32(A_DMA_DTC(DMA_NAND_COPY_CHANNEL), size / 32);	            	    
	OUTREG32(A_DMA_DRT(DMA_NAND_COPY_CHANNEL), DRT_AUTO);
	OUTREG32(A_DMA_DCM(DMA_NAND_COPY_CHANNEL),(DCM_DAI | DCM_SP_32BIT | DCM_DP_32BIT| DCM_TSZ_32BYTE));
	CLRREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL),(DCS_TT));
	SETREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL), DCS_CTE | DCS_NDES);
	while (!(INREG32(A_DMA_DCS(DMA_NAND_COPY_CHANNEL)) & DCS_TT));
}
