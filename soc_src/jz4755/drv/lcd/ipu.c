/********************** BEGIN LICENSE BLOCK ************************************
 *
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
 *  Author:   <jbyu@ingenic.cn>
 *
 *  Create:   2008-10-15, by jbyu
 *
 *******************************************************************************
 */

#include <bsp.h>
#include <jz4755.h>
#include <dm.h>
#include <function.h>
#include <os_api.h>
#include <lcdc.h>
#include <ipu_app.h>

#define IPU_LUT_LEN							(32)
#define FB_LCD_WIDTH						lcd_get_width()
#define FB_LCD_HEIGHT						lcd_get_height()
#define IPU_OPEN							(1 << 0)
#define IPU_INIT							(1 << 1)
#define IPU_SET_STATE_BIT					(25)		// 7 bit
#define IPU_SET_STATE_MASK					(0x7f << IPU_SET_STATE_BIT)
#define SET_BIT(x)							(1 << (IPU_SET_STATE_BIT+x))
#define IPU_CHANGE_BUF						(SET_BIT(0))
#define IPU_SET_CTRL						(SET_BIT(1))
#define IPU_SET_DAT_FMT						(SET_BIT(2))
#define IPU_SET_CSC							(SET_BIT(3))
#define IPU_RATIO_MUL						(100000)
//#define IPU_FUNC_EX

#define IPU_INTC_DISABLE()					OUTREG32(A_INTC_ICMSR, 1 << 30);
#define IPU_INTC_ENABLE()					OUTREG32(A_INTC_ICMCR, 1 << 30);

static int ipu_inited = 0, ipu_size_cfg = 0;
static int ipu_rtable_len;

unsigned int fb_w, fb_h;
unsigned int rsize_w = 0, rsize_h = 0;
unsigned int ipu_state = 0;

static img_param_t *img;
static struct Ration2m *ipu_ratio_table;
static rsz_lut h_lut[IPU_LUT_LEN];
static rsz_lut v_lut[IPU_LUT_LEN];
static int ipu_change_buf = 0;
static unsigned int *py_stride, *pu_stride, *pv_stride, *pout_stride;

void get_fbaddr_info()
{
	fb_w = FB_LCD_WIDTH;
	fb_h = FB_LCD_HEIGHT;
}

/*----------------------------------------------------------------------------------*/
#if 0
char *ipu_regs_name[] = {
    "REG_CTRL",         /* 0x0 */
    "REG_STATUS",       /* 0x4 */
    "REG_D_FMT",        /* 0x8 */
    "REG_Y_ADDR",       /* 0xc */
    "REG_U_ADDR",       /* 0x10 */
    "REG_V_ADDR",       /* 0x14 */
    "REG_IN_FM_GS",     /* 0x18 */
    "REG_Y_STRIDE",     /* 0x1c */
    "REG_UV_STRIDE",    /* 0x20 */
    "REG_OUT_ADDR",     /* 0x24 */
    "REG_OUT_GS",       /* 0x28 */
    "REG_OUT_STRIDE",   /* 0x2c */
    "RSZ_COEF_INDEX",   /* 0x30 */
    "REG_CSC_C0_COEF",  /* 0x34 */
    "REG_CSC_C1_COEF",  /* 0x38 */
    "REG_CSC_C2_COEF",  /* 0x3c */
    "REG_CSC_C3_COEF",  /* 0x40 */
};

int jz47_dump_ipu_regs(int num)
{
	int i, total;

	if (num >= 0)
	{
	  //printf ("ipu_reg: %s: 0x%x\n", ipu_regs_name[num >> 2], REG32(ipu_vbase + num));
		return (1);
	}

	if (num == -1)
	{
		total = sizeof (ipu_regs_name) / sizeof (char *);
		for (i = 0; i < total; i++)
		{
			printf ("ipu_reg: %s: 0x%x\r\n", ipu_regs_name[i], INREG32((unsigned int *)IPU_BASE_U_VIRTUAL + i));
		}
	}

	if (num == -2)
	{
		for (i = 0; i < IPU_LUT_LEN; i++)
		{
			printf ("ipu_H_LUT(%d): 0x%x\n", i, INREG32(A_IPU_HRSZ_COEF_LUT + i * 4));
		}

		for (i = 0; i < IPU_LUT_LEN; i++)
		{
			printf ("ipu_V_LUT(%d): 0x%x\n", i, INREG32(A_IPU_VRSZ_COEF_LUT + i * 4));
		}
	}

	return (1);
}


int print_img(void)
{
	printf ("ipu_ctrl[%#x]\r\n" , img->ipu_ctrl);
	printf ("ipu_d_fmt[%#x]\r\n" , img->ipu_d_fmt);
	printf ("in_width[%#x]\r\n" , img->in_width);
	printf ("in_height[%#x]\r\n" , img->in_height);
	printf ("in_bpp[%#x]\r\n" , img->in_bpp);
	printf ("out_width[%#x]\r\n" , img->out_width);
	printf ("out_height[%#x]\r\n" , img->out_height);
	printf ("y_buf[%#x]\r\n" , img->y_buf);
	printf ("u_buf[%#x]\r\n" , img->u_buf);
	printf ("v_buf[%#x]\r\n" , img->v_buf);
	printf ("out_buf[%#x]\r\n" , img->out_buf);
	printf ("y_t_addr[%#x]\r\n" , img->y_t_addr);
	printf ("u_t_addr[%#x]\r\n" , img->u_t_addr);
	printf ("v_t_addr[%#x]\r\n" , img->v_t_addr);
	printf ("out_t_addr[%#x]\r\n" , img->out_t_addr);

}
#endif

// set ipu data format.
static int jz47_set_ipu_csc_cfg(int outW, int outH, int Wdiff, int Hdiff)
{
	unsigned int in_fmt = img->ipu_d_fmt & IN_FMT_MASK;
	unsigned int out_fmt = img->ipu_d_fmt & OUT_FMT_MASK;

	switch (in_fmt)
	{
		case IN_FMT_YUV420:
		    Hdiff = (Hdiff + 1) & ~1;
		    Wdiff = (Wdiff + 1) & ~1;
		    break;

		case IN_FMT_YUV422:
		    Wdiff = (Wdiff + 1) & ~1;
		    break;

		case IN_FMT_YUV444:
		case IN_FMT_YUV411:
		    break;
		default:
		  printf("Error: Input data format isn't support\n");
		  return (-1);
	}

	switch(out_fmt)
	{
		case OUT_FMT_RGB888:
			outW = outW << 2;
			break;
		case OUT_FMT_RGB555:
			outW = outW << 1;
			break;
		case OUT_FMT_RGB565:
			outW = outW << 1;
			break;
	}

	// Set GS register
	OUTREG32(A_IPU_IN_FM_GS, IN_FM_W(img->in_width) | IN_FM_H((img->in_height - Hdiff) & ~0x1));
	OUTREG32(A_IPU_OUT_GS, OUT_FM_W(outW) | OUT_FM_H(outH));

	// Set out stirde
	if (img->stride != 0)
	{
		OUTREG32(A_IPU_OUT_STRIDE, img->stride->out);
//		OUTREG32(A_IPU_OUT_STRIDE, fb_w * img->in_bpp >> 3);
	}
	else
	{
//		OUTREG32(A_IPU_OUT_STRIDE, outW);
		OUTREG32(A_IPU_OUT_STRIDE, fb_w * img->in_bpp >> 3);
	}

	// set Format
	OUTREG32(A_IPU_D_FMT, img->ipu_d_fmt);

	// set CSC parameter
	if ((in_fmt != IN_FMT_YUV444) && (out_fmt != OUT_FMT_YUV422))
	{
		img->ipu_ctrl |= CSC_EN;

		OUTREG32(A_IPU_CSC_C0_COEF, YUV_CSC_C0);
		OUTREG32(A_IPU_CSC_C1_COEF, YUV_CSC_C1);
		OUTREG32(A_IPU_CSC_C2_COEF, YUV_CSC_C2);
		OUTREG32(A_IPU_CSC_C3_COEF, YUV_CSC_C3);
		OUTREG32(A_IPU_CSC_C4_COEF, YUV_CSC_C4);
		OUTREG32(A_IPU_CSC_OFSET_PARA, CHROM(YUV_CSC_CHROM) | LUMA(YUV_CSC_LUMA));
	}

	return (0);
}


static int init_ipu_ratio_table(void)
{
	unsigned int i, j, cnt;
	unsigned int r, min;
	int diff;

	if (ipu_ratio_table == 0)
	{
		printf("no find ratio table!\n");
		return (-1);
	}
	//	get_ipu_ratio_table( (void **)&ipu_ratio_table, (IPU_LUT_LEN) * (IPU_LUT_LEN)*sizeof(struct Ration2m) );
	// orig table, first calculate
	for (i = 1; i <= (IPU_LUT_LEN); i++)
	{
		for (j = 1; j <= (IPU_LUT_LEN); j++)
		{
			ipu_ratio_table[(i - 1) * IPU_LUT_LEN + j - 1].ratio = i * IPU_RATIO_MUL / j;
			ipu_ratio_table[(i - 1) * IPU_LUT_LEN + j - 1].n = i;
			ipu_ratio_table[(i - 1) * IPU_LUT_LEN + j - 1].m = j;
		}
	}

	// Eliminate the ratio greater than 1:2
	for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++)
	{
		if (ipu_ratio_table[i].ratio < IPU_RATIO_MUL / 2)
		{
	    	ipu_ratio_table[i].n = ipu_ratio_table[i].m = -1;
	    }
    }
	// eliminate the same ratio
	for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++)
	{
		for (j = i + 1; j < (IPU_LUT_LEN) * (IPU_LUT_LEN); j++)
		{
			diff = ipu_ratio_table[i].ratio - ipu_ratio_table[j].ratio;
			if (diff > -100 && diff < 100)
			{
				ipu_ratio_table[j].n = -1;
				ipu_ratio_table[j].m = -1;
			}
		}
	}

	// reorder ipu_ratio_table
	cnt = 0;
	for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++)
	{
		if (ipu_ratio_table[i].n != -1)
		{
			if (cnt != i)
			{
				ipu_ratio_table[cnt] = ipu_ratio_table[i];
			}
			cnt++;
		}
	}
	ipu_rtable_len = cnt;
	//printf ("ipu_rtable_len = %d\n", ipu_rtable_len);
	return (0);
}

static int find_ipu_ratio_factor(unsigned int ratio, unsigned int up)
{
	unsigned int i, sel;
	unsigned int diff, min = IPU_RATIO_MUL;

	sel = ipu_rtable_len;

	for (i = 0; i < ipu_rtable_len; i++)
	{
		if ((up == 0) && (ipu_ratio_table[i].n & 1 != 0))
		{
			continue;
		}

		if (ratio > ipu_ratio_table[i].ratio)
		{
			diff = ratio - ipu_ratio_table[i].ratio;
		}
		else
		{
			diff = ipu_ratio_table[i].ratio - ratio;
		}

		if (diff < min || i == 0)
		{
			min = diff;
			sel = i;
		}
	}

	return (sel);
}
#ifndef IPU_FUNC_EX
static int resize_lut_cal(int srcN, int dstM, int upScale, rsz_lut lut[]);
static int resize_out_cal(int insize, int outsize, int srcN, int dstM, int upScale, int *diff);
#else
static int (*resize_lut_cal)(int srcN, int dstM, int upScale, rsz_lut lut[]);
static int (*resize_out_cal)(int insize, int outsize, int srcN, int dstM, int upScale, int *diff);

void set_ipu_cal_func(void *lut_cal, void *out_cal)
{
	resize_lut_cal = lut_cal;
	resize_out_cal = out_cal;
}
#endif

static int jz47_set_ipu_resize(int *outWp, int *outHp,
                         int *Wdiff, int *Hdiff)
{
	int W = 0, H = 0, Hsel = 0, Wsel = 0;
	int srcN, dstM, width_up, height_up;
	int Height_lut_max, Width_lut_max;
	int i;

	rsize_w = ((img->out_width + img->out_x) > fb_w) ? (fb_w  - img->out_x) : img->out_width;
	rsize_h = ((img->out_height + img->out_y) > fb_h) ? (fb_h - img->out_y) : img->out_height;

	rsize_w = (rsize_w > 2 * img->in_width) ? (2 * img->in_width) : rsize_w;
	rsize_h = (rsize_h > 2 * img->in_height) ? (2 * img->in_height) : rsize_h;
	*Wdiff = *Hdiff = 0;

	img->ipu_ctrl &= ~(VRSZ_EN | HRSZ_EN | H_SCALE | V_SCALE);
	// Don't need do resize ?
	if ((img->in_width == rsize_w) && (img->in_height == rsize_h))
	{
		img->out_height = *outHp = rsize_h;
		img->out_width = *outWp = rsize_w;
		return (0);
	}
	else
	{
		if (img->in_width != rsize_w)
		{
			img->ipu_ctrl |= HRSZ_EN;
		}

		if (img->in_height != rsize_h)
		{
			img->ipu_ctrl |= VRSZ_EN;
		}
	}

	// init the resize factor table
	init_ipu_ratio_table();

	width_up = (rsize_w >= img->in_width);
	height_up = (rsize_h >= img->in_height);
	img->ipu_ctrl |= (rsize_w > img->in_width) ?  H_SCALE : 0;
	img->ipu_ctrl |= (rsize_h > img->in_height) ? V_SCALE : 0;

	// get the resize factor
	if (W != rsize_w)
	{
		Wsel = find_ipu_ratio_factor(img->in_width * IPU_RATIO_MUL /rsize_w, 1);
//		printf ("horizontal resize: sel = %d, n=%d, m=%d\n", Wsel, ipu_ratio_table[Wsel].n,
//	        ipu_ratio_table[Wsel].m);
		W = rsize_w;
	}

	if (H != rsize_h)
	{
		Hsel = find_ipu_ratio_factor(img->in_height * IPU_RATIO_MUL /rsize_h, height_up);
//		printf ("Vertical resize: sel = %d, n=%d, m=%d\n", Hsel, ipu_ratio_table[Hsel].n,
//	        ipu_ratio_table[Hsel].m);
		H = rsize_h;
	}

	// set IPU_INDEX register
	Width_lut_max  = width_up  ? ipu_ratio_table[Wsel].m : ipu_ratio_table[Wsel].n;
	Height_lut_max = height_up ? ipu_ratio_table[Hsel].m : ipu_ratio_table[Hsel].n;
	OUTREG32(A_IPU_RSZ_COEF_INDEX, ((Height_lut_max - 1) << VE_IDX_H_BIT) | ((Width_lut_max - 1)  << HE_IDX_W_BIT));

	// calculate out W/H and LUT
	srcN = ipu_ratio_table[Wsel].n;
	dstM = ipu_ratio_table[Wsel].m;
	*outWp = resize_out_cal( img->in_width, rsize_w, srcN, dstM, width_up, Wdiff);
	//resize_out_cal( img->in_width, rsize_w, srcN, dstM, width_up, Wdiff);
	resize_lut_cal(srcN, dstM, width_up, h_lut);

	srcN = ipu_ratio_table[Hsel].n;
	dstM = ipu_ratio_table[Hsel].m;
	*outHp = resize_out_cal( img->in_height, rsize_h, srcN, dstM, height_up, Hdiff);
	//resize_out_cal( img->in_height, rsize_h, srcN, dstM, height_up, Hdiff);
	resize_lut_cal(srcN, dstM, height_up, v_lut);

	img->out_height = *outHp;
	img->out_width = *outWp;

	// set IPU LUT register
	SETREG32(A_IPU_VRSZ_COEF_LUT, LUT_START);
	for (i = 0; i < Height_lut_max; i++)
	{
		if ((height_up == 0) && ((i & 1) == 0))
	 	{
			if ((v_lut[i].out_n == 0) && (v_lut[i+1].out_n == 1))
			{
				v_lut[i].out_n = 1;
				v_lut[i+1].out_n = 0;
			}
	 	}

		OUTREG32(A_IPU_VRSZ_COEF_LUT, (v_lut[i].coef << W_COEF_BIT)
	        | (v_lut[i].in_n << IN_EN_BIT) | (v_lut[i].out_n  << OUT_EN_BIT));
	}

	SETREG32(A_IPU_HRSZ_COEF_LUT, LUT_START);
	for (i = 0; i < Width_lut_max; i++)
	{
	 	OUTREG32(A_IPU_HRSZ_COEF_LUT, (h_lut[i].coef << W_COEF_BIT)
	        | (h_lut[i].in_n << IN_EN_BIT) | (h_lut[i].out_n  << OUT_EN_BIT));
	}

	return (0);
}

static int jz47_set_ipu_buf(void)
{
	int ret;
	unsigned int py_buf;
	unsigned int pu_buf;
	unsigned int pv_buf;
	unsigned int py_t_buf;
	unsigned int pu_t_buf;
	unsigned int pv_t_buf;
	unsigned int out_buf;
	PIPU_CTRL2 pipu_ctrl = (PIPU_CTRL2)(&img->ipu_ctrl);

	py_buf = PHYS((unsigned int)img->y_buf);
	pu_buf = PHYS((unsigned int)img->u_buf);
	pv_buf = PHYS((unsigned int)img->v_buf);

	if (pipu_ctrl->spage_map != 0)
	{
		if ((py_t_buf == 0) || (pu_t_buf == 0) || (pv_t_buf == 0))
		{
			printf(" Can not found source map table, use no map now!\r\n");
			pipu_ctrl->spage_map = 0;
		}
		else
		{
			py_t_buf = PHYS((unsigned int)img->y_t_addr);
			pu_t_buf = PHYS((unsigned int)img->u_t_addr);
			pv_t_buf = PHYS((unsigned int)img->v_t_addr);

			py_buf = py_t_buf & 0xfff;
			pu_buf = pu_t_buf & 0xfff;
			pv_buf = pv_t_buf & 0xfff;

			// set phy table addr
			OUTREG32(A_IPU_Y_PHY_T_ADDR, py_t_buf);
			OUTREG32(A_IPU_U_PHY_T_ADDR, pu_t_buf);
			OUTREG32(A_IPU_V_PHY_T_ADDR, pv_t_buf);
		}
	}
	else
	{
		if ((py_buf == 0) || (pu_buf == 0) || (pv_buf == 0))
		{
			printf ("++ Can not found buffer(0x%x,0x%x,0x%x) physical addr since addr errors +++\n",
		          (unsigned int)img->y_buf, (unsigned int)img->u_buf, (unsigned int)img->v_buf);
			return (-1);
		}
	}

	//set Y,U,V addr register
	OUTREG32(A_IPU_Y_ADDR, py_buf);
	OUTREG32(A_IPU_U_ADDR, pu_buf);
	OUTREG32(A_IPU_V_ADDR, pv_buf);

	// set out put
	if ((pipu_ctrl->dpage_map != 0) && (pipu_ctrl->lcdc_sel == 0))
	{
		if (PHYS(img->out_t_addr) == 0)
		{
			printf(" Can not found destination map table, use no map now!\r\n");
			pipu_ctrl->dpage_map = 0;

			if (PHYS(img->out_buf) == 0)
			{
				printf("Can not found the destination buf[%#x]\r\n",  img->out_buf);
				return (-1);
			}
			else
			{
				OUTREG32(A_IPU_OUT_ADDR, PHYS(img->out_buf));
			}
		}
		else
		{
			OUTREG32(A_IPU_OUT_ADDR, PHYS(img->out_t_addr) & 0xfff);
			OUTREG32(A_IPU_OUT_PHY_T_ADDR, PHYS(img->out_t_addr));
		}
	}
	else
	{
		pipu_ctrl->dpage_map = 0;
		if (pipu_ctrl->lcdc_sel == 0)
		{
			if (PHYS(img->out_buf) == 0)
			{
				printf("Can not found the destination buf[%#x]\r\n",  img->out_buf);
				return (-1);
			}
			else
			{
				out_buf = (unsigned int)img->out_buf + (img->out_y * fb_w + img->out_x) * img->in_bpp / 8;
				OUTREG32(A_IPU_OUT_ADDR, PHYS(out_buf));
			}
		}
	}

	// flush the dcache
	// __dcache_writeback_all();

	return (0);
}

void get_ipu_addr(unsigned int* addr )
{
	*addr++ = A_IPU_Y_ADDR;
	*addr++ = A_IPU_U_ADDR;
	*addr++ = A_IPU_V_ADDR;
	*addr++ = A_IPU_OUT_ADDR;
	*addr++ = A_IPU_Y_STRIDE;
	*addr++ = A_IPU_UV_STRIDE;
	*addr++ = A_IPU_CONTROL;
	*addr = A_IPU_STATUS;
}

#ifndef IPU_FUNC_EX
static int resize_out_cal(int insize, int outsize, int srcN, int dstM,
                    int upScale, int *diff)
{
	unsigned int calsize, delta;
	unsigned int tmp, tmp2;

	delta = IPU_RATIO_MUL;
	insize *= IPU_RATIO_MUL;

	do
	{
		tmp = ((insize - delta) * dstM / srcN) & (~(IPU_RATIO_MUL - 1));
		tmp2 = tmp  * srcN / dstM;
		if (upScale)
		{
			if (tmp2 == insize - delta)
			{
				calsize = tmp / IPU_RATIO_MUL + 1;
			}
			else
			{
				calsize = tmp / IPU_RATIO_MUL + 2;
			}
		}
		else   // down Scale
		{
			if (tmp2 == insize - delta)
			{
				calsize = tmp / IPU_RATIO_MUL;
			}
			else
			{
		    	calsize = tmp / IPU_RATIO_MUL + 1;
		    }
		}

		delta += IPU_RATIO_MUL;

	} while (calsize > outsize);

	*diff = delta / IPU_RATIO_MUL - 2;

	return (calsize);
}


static int resize_lut_cal(int srcN, int dstM, int upScale, rsz_lut lut[])
{
	int i, t, x;
	unsigned int w_coef, factor, factor2;

	if (upScale)
	{
		for (i = 0, t = 0; i < dstM; i++)
		{
			factor = (i * srcN * IPU_RATIO_MUL) / dstM;
			factor2 = factor - factor / IPU_RATIO_MUL * IPU_RATIO_MUL;
			w_coef = IPU_RATIO_MUL  - factor2;
			lut[i].coef = (unsigned int)(512 * w_coef / IPU_RATIO_MUL) & W_COEF_MASK;
			// calculate in & out
			lut[i].out_n = 1;
			if (t <= factor)
			{
				lut[i].in_n = 1;
				t += IPU_RATIO_MUL;
			}
			else
			{
				lut[i].in_n = 0;
			}
		} // end for
	}
	else
	{
		for (i = 0, t = 0, x = 0; i < srcN; i++)
		{
			factor = (t * srcN + 1) * IPU_RATIO_MUL / dstM;
			if (dstM == 1)
			{
				// calculate in & out
				lut[i].coef = (i == 0) ? 256 : 0;
				lut[i].out_n = (i == 0) ? 1  : 0;
			}
			else
			{
				if (((t * srcN + 1) / dstM - i) >= 1)
				{
					lut[i].coef = 0;
				}
				else
				{
					if (factor - i * IPU_RATIO_MUL == 0)
					{
						lut[i].coef = 512;
						t++;
					}
					else
					{
						factor2 = (t * srcN ) / dstM * IPU_RATIO_MUL;
						factor = factor - factor2;
						w_coef = IPU_RATIO_MUL  - factor;
						lut[i].coef = (unsigned int)(512 * w_coef / IPU_RATIO_MUL) & W_COEF_MASK;
						t++;
					}
				}
			}
			// calculate in & out
			lut[i].in_n = 1;
			if (dstM != 1)
			{
				if (((x * srcN + 1) / dstM - i) >= 1)
				{
					lut[i].out_n = 0;
				}
				else
				{
					lut[i].out_n = 1;
					x++;
				}
			}

		}	// for end down Scale
	}	// else upScale

	return (0);
}
#endif

static void ipu_interrupt_handler(void)
{
	unsigned int ctrl;
//	if (ipu_change_buf != 0)
	{
		if ((INREG32(A_IPU_STATUS) & OUT_END) == 0)
		{
			return;
		}

		CLRREG32(A_IPU_STATUS, OUT_END);
		ctrl = INREG32(A_IPU_CONTROL);
		if (!(ctrl & FIELD_SEL))
		{
			// jz47_set_ipu_buf();
			OUTREG32(A_IPU_Y_ADDR, PHYS((unsigned int)img->y_buf));
			OUTREG32(A_IPU_U_ADDR, PHYS((unsigned int)img->u_buf));
			OUTREG32(A_IPU_V_ADDR, PHYS((unsigned int)img->v_buf));
			OUTREG32(A_IPU_ADDR_CTRL, YUV_READY);
		}
		else
		{
			OUTREG32(A_IPU_Y_ADDR, PHYS((unsigned int)img->y_buf+img->stride->y));
			OUTREG32(A_IPU_U_ADDR, PHYS((unsigned int)img->u_buf)+img->stride->u);
			OUTREG32(A_IPU_V_ADDR, PHYS((unsigned int)img->v_buf)+img->stride->v);
			OUTREG32(A_IPU_ADDR_CTRL, YUV_READY);
		}
		// ipu_change_buf = 0;
		//OUTREG32(A_IPU_Y_STRIDE, *py_stride);
		//OUTREG32(A_IPU_UV_STRIDE, U_STRIDE(*pu_stride) | V_STRIDE(*pv_stride));

		SETREG32(A_IPU_CONTROL, IPU_RUN);
		//printf(" %s\n", __FUNCTION__);

		//IPU_INTC_DISABLE();
	}
}

int ipu_open(void)
{
	ipu_state |= IPU_OPEN;

	CLRREG32(A_IPU_STATUS, OUT_END);
	SETREG32(A_IPU_CONTROL, IPU_RUN);

	return (0);
}

int ipu_close(void)
{
	if ((ipu_state & IPU_OPEN) == 0)
	{
		return (0);
	}

	CLRREG32(A_IPU_CONTROL, IPU_RUN);
	ipu_state &= ~IPU_OPEN;

	return (0);
}

int ipu_init(img_param_t *pimg)
{
	int ret = -1;
	int in_fmt;
	int out_fmt;
	int outW, outH, Wdiff, Hdiff;

	printf("++ IPU init ++\r\n");

#ifdef IPU_FUNC_EX
	if (!pimg || !resize_lut_cal || !resize_out_cal)
#else
	if (!pimg)
#endif
	{
		printf("ipu_init: parameter error\r\n");
		return (ret);
	}

 	get_fbaddr_info();
 	img = pimg;
	outW = img->out_width;
	outH = img->out_height;
	in_fmt = img->ipu_d_fmt & IN_FMT_MASK;
	out_fmt = img->ipu_d_fmt & OUT_FMT_MASK;
	ipu_ratio_table = img->ratio_table;
	py_stride = &(img->stride->y);
	pu_stride = &(img->stride->u);
	pv_stride = &(img->stride->v);
	pout_stride = &(img->stride->out);
	//img->ipu_ctrl &= (LCDC_SEL | FM_IRQ_EN | ADDR_SEL | FIELD_SEL | DISP_SEL | FIELD_CONF_EN);

	if (ipu_inited == 0)
	{
		CLRREG32(A_CPM_CLKGR, CLKGR_STOP_IPU); 			// run ipu clock

		request_irq(IRQ_IPU, ipu_interrupt_handler, 0);
		//IPU_INTC_DISABLE();

		CLRREG32(A_IPU_CONTROL, IPU_RUN);
		SETREG32(A_IPU_CONTROL, CHIP_EN);
		SETREG32(A_IPU_CONTROL, IPU_RST);				// reset controller
		CLRREG32(A_IPU_CONTROL, IPU_RST);
		while ((INREG32(A_IPU_STATUS) & OUT_END) == 0); // wait the end flag
	}
	else
	{
		while ((INREG32(A_IPU_CONTROL) & IPU_RUN) && ((INREG32(A_IPU_STATUS) & OUT_END) == 0)); // wait the end flag
		SETREG32(A_IPU_CONTROL, IPU_RST);				// reset controller
		CLRREG32(A_IPU_CONTROL, IPU_RST);
		while ((INREG32(A_IPU_STATUS) & OUT_END) == 0); // wait the end flag
	}


	if ((in_fmt == IN_FMT_YUV444) && (out_fmt != OUT_FMT_YUV422))
	{
		img->ipu_ctrl &= ~SPKG_SEL;
	}

	// set IPU resize field
	jz47_set_ipu_resize(&outW, &outH, &Wdiff, &Hdiff);

	// set CSC parameter and format
	ret = jz47_set_ipu_csc_cfg(outW, outH, Wdiff, Hdiff);
	if (ret != 0)
	{
	    return (ret);
	}

	// set the input and out put buffer
	if (jz47_set_ipu_buf() < 0)
	{
		ret = -1;
		return (ret);
	}

	// set  stride
	if (img->stride == 0)
	{
		if (img->ipu_ctrl & SPKG_SEL)
		{
			OUTREG32(A_IPU_Y_STRIDE, img->in_width * 2);
		}
		else
		{

			OUTREG32(A_IPU_Y_STRIDE, img->in_width);
		}

		switch(((PIPU_DFMT2)&(img->ipu_d_fmt))->in_fmt)
		{
			case IN_FMT_YUV420:
			case IN_FMT_YUV422:
				OUTREG32(A_IPU_UV_STRIDE, U_STRIDE(img->in_width / 2) | V_STRIDE(img->in_width / 2));
				break;

			case IN_FMT_YUV444:
				OUTREG32(A_IPU_UV_STRIDE, U_STRIDE(img->in_width) | V_STRIDE(img->in_width));
				break;

			case IN_FMT_YUV411:
				OUTREG32(A_IPU_UV_STRIDE, U_STRIDE(img->in_width / 4) | V_STRIDE(img->in_width / 4));
				break;

			default:
				printf("Error: Input data format isn't support\n");
				return (-1);
				break;
		}
	}
	else
	{
		OUTREG32(A_IPU_Y_STRIDE, *py_stride);
		OUTREG32(A_IPU_UV_STRIDE, U_STRIDE(*pu_stride) | V_STRIDE(*pv_stride));
	}

	//CLRREG32(A_IPU_STATUS, OUT_END);

	// set the ctrl
	OUTREG32(A_IPU_CONTROL, img->ipu_ctrl | CHIP_EN);

	ipu_inited = 1;
//	jz47_dump_ipu_regs(-1);
	return (0);
}

int ipu_flush(void)
{
	ipu_init(img);
}

int jz47_ipu_resize(void)
{
	int ret = -1;
	int outW, outH, Wdiff, Hdiff;
	unsigned int out_buf;

	get_fbaddr_info();

	while ((INREG32(A_IPU_CONTROL) & IPU_RUN) && ((INREG32(A_IPU_STATUS) & OUT_END) == 0)); // wait the end flag

	SETREG32(A_IPU_CONTROL, IPU_RST);				// reset controller
	CLRREG32(A_IPU_CONTROL, IPU_RST);
	while ((INREG32(A_IPU_STATUS) & OUT_END) == 0); // wait the end flag

	// set IPU resize field
	jz47_set_ipu_resize(&outW, &outH, &Wdiff, &Hdiff);

	// set CSC parameter and format
	ret = jz47_set_ipu_csc_cfg(outW, outH, Wdiff, Hdiff);
	if (ret != 0)
	{
	    return (ret);
	}


	OUTREG32(A_IPU_Y_STRIDE, *py_stride);
	OUTREG32(A_IPU_UV_STRIDE, U_STRIDE(*pu_stride) | V_STRIDE(*pv_stride));

	// set the input and out put buffer
	if (jz47_set_ipu_buf() < 0)
	{
		ret = -1;
		return (ret);
	}

	// set the ctrl
	OUTREG32(A_IPU_CONTROL, img->ipu_ctrl | CHIP_EN);

	return (0);
}

int ipu_ioctl(void *buff, unsigned int cmd)
{
	switch (cmd)
	{
		case IOCTL_IPU_CHANGE_BUFF:
			//ipu_change_buf = 1;
			//while (INREG32(A_IPU_ADDR_CTRL));

			if(!INREG32(A_IPU_ADDR_CTRL))
			{
				OUTREG32(A_IPU_Y_ADDR, PHYS((unsigned int)img->y_buf));
				OUTREG32(A_IPU_U_ADDR, PHYS((unsigned int)img->u_buf));
				OUTREG32(A_IPU_V_ADDR, PHYS((unsigned int)img->v_buf));
				OUTREG32(A_IPU_ADDR_CTRL, YUV_READY);
	        }
			//IPU_INTC_ENABLE();
			break;

		case IOCTL_IPU_SET_BUFF:
			jz47_set_ipu_buf();
			break;

		case IOCTL_IPU_START:
			SETREG32(A_IPU_CONTROL, IPU_RUN);
			img->ipu_ctrl |= IPU_RUN;
			break;

		case IOCTL_IPU_STOP:
			CLRREG32(A_IPU_CONTROL, IPU_RUN);
			img->ipu_ctrl &= ~IPU_RUN;
			break;

		case IOCTL_IPU_SET_CTRL_REG:
//			ipu_state |= IPU_SET_CTRL;
			break;

		case IOCTL_IPU_SET_FMT_REG:
			ipu_state |= IPU_SET_DAT_FMT;
			break;

		case IOCTL_IPU_SET_CSC:
			ipu_state |= IPU_SET_CSC;
			break;

		case IOCTL_IPU_SET_STRIDE:
			break;

		case IOCTL_IPU_FB_SIZE:
			fb_w = (unsigned int)(unsigned int *)buff++;
			fb_h = (unsigned int)(unsigned int *)buff;
			break;

		case IOCTL_IPU_PENDING_OUTEND:
			while ((INREG32(A_IPU_STATUS) & OUT_END) == 0);
			CLRREG32(A_IPU_STATUS, OUT_END);
			break;

		default:
			printf("ipu not support ioctl cmd[%#x]\r\n", cmd);
			return (-1);
			break;
	}

	return (0);
}

int ipu_deinit(void)
{
	if ((ipu_state & IPU_OPEN) != 0)
	{
		CLRREG32(A_IPU_CONTROL, IPU_RUN);
	}

	free_irq(IRQ_IPU);

	// disable the ipu module
	CLRREG32(A_IPU_CONTROL, CHIP_EN);

	// stop the ipu clock
	SETREG32(A_CPM_CLKGR, CLKGR_STOP_IPU);

	ipu_inited = 0;

	ipu_state = 0;

	return (0);
}

int ipu_poweron(void)
{
	return (0);
}

int ipu_poweroff(void)
{
	return (0);
}

