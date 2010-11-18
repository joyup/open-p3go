/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
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
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn>
 *
 *  Create:   2008-06-26, by dsqiu
 *
 *  Maintain: 2008-06-26, by jgao
 *
 *
 *******************************************************************************
 */
const static char *regstr[] = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};
#include <mipsregs.h>
#include <excpt.h>
#include <os_api.h>

typedef struct
{
	unsigned int sp;
	unsigned int ra;
 	PFun_Exception_Modal_Handler except_handle;
}Except_Struct,*PExcept_Struct;
static PFun_Exception_Modal_Handler main_except_handle = 0;
#define malloc(x) alloc(x)
#define free(x) deAlloc(x)
static Except_Struct g_pExcept[10] = {0};
static unsigned int g_MaxExceptNum = 10;
static unsigned int g_CurExcept = 0;

void ReadWord(char argc,char **argv)
{
	unsigned int addr;

	addr = strtoul(argv[0],0,0);

	if(argc > 0)
	{
		printf("\n");
		printf("0x%x:0x%x",addr,*(unsigned int *)addr);
	}
}

void WriteWord(char argc,char **argv)
{
	unsigned int addr = 0x82000000;

	unsigned int val;
	if(argc > 0)
	{
		addr = strtoul(argv[0], 0, 0);
		val = strtoul(argv[1], 0, 0);
		printf("\n");
		printf("pre wirte 0x%x:0x%x",addr,*(unsigned int *)addr);
		*(unsigned int *)addr = val;
		printf("after wirte 0x%x:0x%x",addr,*(unsigned int *)addr);
	}
}


void setmainexcpt(void *p)
{
    main_except_handle = p;
}
extern void shelltask();


int InitExcept(unsigned int x)
{
	//g_pExcept = (PExcept_Struct)malloc(x * sizeof(Except_Struct));
	//if(g_pExcept == 0)
	//	return 0;
	memset(g_pExcept,0,10 * sizeof(Except_Struct));
	g_MaxExceptNum = 10;
  g_CurExcept = 0;
#if RELEASE==0
  init_command();
  request_command(ReadWord,"ReadWord");
  request_command(WriteWord,"WriteWord");
#endif
	return 1;
}
void DeinitExcept()
{
	if(g_pExcept)
		free(g_pExcept);
}
int AddExcept(PFun_Exception_Modal_Handler pFun,unsigned int *sr)
{
	unsigned int save;
	int ret;
	save = spin_lock_irqsave();
	g_CurExcept = 0;
	for(ret = g_MaxExceptNum - 1;ret >= 0;ret--)
	{
		printf("g_pExcept[%d].sp = %x\n",ret,g_pExcept[ret].sp);
		if(g_pExcept[ret].sp != 0)
		{
			g_CurExcept = ret + 1;
			break;
		}
	}
	printf("g_CurExcept = %d\n",g_CurExcept);
	if(g_CurExcept >= g_MaxExceptNum){
		 printf("AddExcept error = %d\n",g_CurExcept);
		 return -1;
	}
	g_pExcept[g_CurExcept].ra = sr[0];
	g_pExcept[g_CurExcept].sp = sr[1];
	g_pExcept[g_CurExcept].except_handle = pFun;
	g_CurExcept++;
	printf("register return fun = %x %d sp = %x ra = %x\n",pFun,g_CurExcept,sr[1],sr[0]);

	ret = g_CurExcept;
	spin_unlock_irqrestore(save);
	return ret;
}
void DecExcept(int d,unsigned int *sr)
{
	unsigned int save;
	save = spin_lock_irqsave();

	printf("%s %d\n",__FUNCTION__,d);

    if(g_CurExcept <= 0) {
    	spin_unlock_irqrestore(save);
    	return ;
		}
    if(d > g_CurExcept) {
    	spin_unlock_irqrestore(save);
    	return;
    }
    d--;
    if(d < 0){
    	 spin_unlock_irqrestore(save);
    	 return;
    }
    sr[0] = g_pExcept[d].ra;
  	sr[1] = g_pExcept[d].sp;


    //memcpy(&(g_pExcept[d]),&(g_pExcept[d+1]),sizeof(Except_Struct) * (g_CurExcept - d - 1));
    //g_CurExcept--;
    g_pExcept[d].sp            = 0;
    g_pExcept[d].ra            = 0;
    g_pExcept[d].except_handle = 0;
    spin_unlock_irqrestore(save);
}
void c_except_handler(unsigned int *sp)
{
	unsigned int i;
	unsigned int epc = read_c0_epc();
	if(epc != 0)
	{
	printf("-----------------------------------------------------\n");
	for(i = 0;i < 4;i++)
	{
		printf("%08x:\t%08x\n",(epc - 4*4 + i*4),*(unsigned int *)((epc - 4 * 4 + i * 4)| 0xa0000000));
  	}
  	for(i = 0;i < 4;i++)
	{
		printf("%08x:\t%08x\n",(epc + i*4),*(unsigned int *)((epc + i*4)| 0xa0000000));
  	}
	printf("-----------------------------------------------------\n");
  }
	printf("CAUSE=%08x EPC=%08x\n", read_c0_cause(), read_c0_epc());
	for (i=0;i<32;i++) {
		if (i % 4 == 0)
			printf("\n");
		printf("%4s %08x ", regstr[i], sp[i]);
	}
	printf("\n");
#if RELEASE==0
		i = (unsigned int)shelltask;
		write_32bit_cp0_register(CP0_EPC,i);
		__asm__ __volatile__("eret\n\t");
#endif
	if(g_CurExcept > 0)
	{
		int sp1 = 0,sp2 = 0x7fffffff,save = 0;
		for(i = 0;i < g_CurExcept;i++)
		{
			if(g_pExcept[i].sp != 0)
			{
				sp1 = (int)(g_pExcept[i].sp - sp[29]);
				printf("g_pExcept[%d].sp = %x sp = %x\n",i,g_pExcept[i].sp,sp[29]);
				if((sp1 <= sp2)&& (sp1 > 0))
				{
					sp2 = sp1;
					save = i;
				}
		  }
		}
		if(sp2 == 0x7fffffff)
			hw_reset();
		printf("save = %d\n",save);
		i = (unsigned int)(g_pExcept[save].except_handle);
		printf("except return %x %d %x %x\n",i,g_CurExcept,g_pExcept[g_CurExcept].sp,g_pExcept[g_CurExcept].ra);
		if(i)
		{
			g_pExcept[save].sp            = 0;
		  g_pExcept[save].ra            = 0;
		  g_pExcept[save].except_handle = 0;
		}

		//memcpy(&(g_pExcept[save]),&(g_pExcept[save+1]),sizeof(Except_Struct) * (g_CurExcept - save - 1));
		//g_CurExcept--;



    }else if(main_except_handle){
		i = (unsigned int)main_except_handle;
    }else{
        printf("c_except_handler: while(1)");
#if DRIVER_ONLY
		while(1);
#else
		hw_reset();
#endif
    }


	write_32bit_cp0_register(CP0_EPC,i);
	__asm__ __volatile__("eret\n\t");
}

#define RESTORE_PROCESS_REGISTER(x,sr) \
    do{ 								\
    	__asm__ __volatile__(			\
    	"lw $31,0x00(%0)\n\t"			\
    	:: "r" (&sr));          \
    	__asm__ __volatile__(			\
    	"lw $3,(%0)\n\t"			\
    	:: "r" (&x));          \
    	__asm__ __volatile__(			\
    	".set noat    \n\t"		\
    		"lw $29,0x04(%0)\n\t"			\
    		"lw $1,0x00($29)\n\t"			\
			"lw $2,0x04($29)\n\t"			\
			"lw $4,0x0C($29)\n\t"			\
			"lw $5,0x10($29)\n\t"			\
			"lw $6,0x14($29)\n\t"			\
			"lw $7,0x18($29)\n\t"			\
			"lw $8,0x1C($29)\n\t"			\
			"lw $9,0x20($29)\n\t"			\
			"lw $10,0x24($29)\n\t"			\
			"lw $11,0x28($29)\n\t"			\
			"lw $12,0x2C($29)\n\t"			\
			"lw $13,0x30($29)\n\t"			\
			"lw $14,0x34($29)\n\t"			\
			"lw $15,0x38($29)\n\t"			\
			"lw $16,0x3C($29)\n\t"			\
			"lw $17,0x40($29)\n\t"			\
			"lw $18,0x44($29)\n\t"			\
			"lw $19,0x48($29)\n\t"			\
			"lw $20,0x4C($29)\n\t"			\
			"lw $21,0x50($29)\n\t"			\
			"lw $22,0x54($29)\n\t"			\
			"lw $23,0x58($29)\n\t"			\
			"lw $24,0x5C($29)\n\t"			\
			"lw $25,0x60($29)\n\t"			\
			"lw $27,0x68($29)\n\t"			\
			"lw $28,0x6C($29)\n\t"			\
			"lw $30,0x74($29)\n\t"			\
			"lw $31,0x78($29)\n\t"			\
			"addu $29,128    \n\t"			\
			"move $2,$3 \n\t"  \
			"jr $31 \n\t"							\
			"nop\n\t"									\
			".set at    \n\t"		\
			:     										\
			: "r" (sr)); \
    }while(0)


void excpt_exit(unsigned int x,unsigned int *sr)
{
		printf("ra = %x sp = %x\n",sr[0],sr[1]);

		RESTORE_PROCESS_REGISTER(x,sr);
		//STORE_REG_RET();
	  //RESTORE_PROCESS_REGISTER(x);
}
