/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4755  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright Ingenic Semiconductor Co. Ltd 2008. All rights reserved.
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
 *  Maintain: 2008-11-29, by whxu
 *
 *******************************************************************************
 */
#if KEYTYPE == 2
#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>
#include <threadprio.h>
#ifdef USE_MIDWARE
#include "midware.h"
#endif

//#undef printf  //xltao add the line
//#define SERIAL_PORT_PRINTF 1 

//-----------------------------------------------------
typedef struct
{
	unsigned int  low_value;     //ad检测最低值
	unsigned int  high_value;    //ad检测最高值
	unsigned char key_phy_value; //物理键值
	unsigned char key_vir_value; //虚拟键值
} AD_Key_Dec;

//此结构体存放每个按键的初始化数据
AD_Key_Dec keydec[] =
{
#if 1 //xltao modify	
	//{10,  599, 0x01},    //menu, xltao, jz original style
	{10,  599, 0x10},    //menu, xltao modify for glwx key style  
	{600, 1200, 0x02},   //right
	{1200, 1800, 0x04},  //left
	{1800, 2300, 0x08},  //down
	{2300, 3000, 0x20},  //up
#else

	//{100,  600, 0x01},  //menu, xltao
	{10,  599, 0x10},     //menu
	{600, 1200, 0x08},    //voladd 
	{1200, 1800, 0x04},   //volsub
	//{1800, 2300, 0x02},   //down
	{1800, 2300, 0x23},   //down
	//{2300, 3000, 0x01},   //up
	{2300, 3000, 0x20},   //up

#endif
};
#define KEY_NUM		(sizeof(keydec) / sizeof(AD_Key_Dec))

//----------------------------------------------------------

#define KEY_DETECT_REPEAT_TIME 100
#define KEY_REPEAT_TIME 50
#define KEY_DELAY_TIME 1

#define POFF_EACH_TIME    100
#define POFF_TRY_TIME     20


//----------------------------------------------------------------------------
//	Key Task Define
#define KEY_TASK_STK_SIZE	1024
#define KEY_TASK_PRIO		KEY_THREAD_PRIO 

static OS_EVENT *key_sem;
static OS_TASK_STACK KeyTaskStack[KEY_TASK_STK_SIZE];

typedef void (*PFN_KEYHANDLE)(unsigned int key);

PFN_KEYHANDLE	UpKey = NULL, DownKey = NULL;

//#ifdef USE_MIDWARE
#ifdef USE_MIDWARE
volatile static MIDSRC keysrc;
#endif
volatile static unsigned int keyid;

static unsigned int Key_Auto_Scan(unsigned int keycode);

static void info_power_off(void)
{
#ifdef USE_MIDWARE
	keysrc.Src = SRC_POWER_OFF;
	keysrc.Event = EVENT_POWER_OFF;

	os_QueuePost(keysrc.CurEvent1 , (void *)&keyid);
	os_SemaphorePost(keysrc.CurEvent);
#endif
}

//----------------------------------------------------------------------------
#ifdef USE_MIDWARE
static void GetRequest(MIDSRCDTA *dat)
{
}

//----------------------------------------------------------------------------

static void Response(MIDSRCDTA *dat)
{
	unsigned int group = POWER_OFF_PIN / 32, bit = 1 << (POWER_OFF_PIN % 32);

	if (dat->Val == 1)
	{
		printf("Check can power off or not ! \n");
		ShutLcd(0);

		cli();
		while (1)
		{
			while (!(INREG32(A_GPIO_PXPIN(group)) & bit));
			os_TimeDelay(20);
			if (INREG32(A_GPIO_PXPIN(group)) & bit)
				jz_pm_hibernate();
		}
	}
	else
		printf("Can not power off !\n");
}
#endif
//----------------------------------------------------------------------------

static void UpKeyHandle(int key)
{
#ifdef USE_MIDWARE
	os_QueuePost(keysrc.CurEvent1, (void *)&keyid);

	keysrc.Src = SRC_KEY;
	os_SemaphorePost(keysrc.CurEvent);
	printf("Key Up! 0x%x\n", key);  //xltao add

	if (UpKey != NULL)
		UpKey(key);
#endif
	
}

//----------------------------------------------------------------------------

static int DownKeyHandle(int key)
{
	unsigned char err;
#ifdef USE_MIDWARE
	os_QueuePost(keysrc.CurEvent1, (void *)&keyid);

	keysrc.Src = SRC_KEY_DOWN;
	os_SemaphorePost(keysrc.CurEvent);
	printf("Key Down! 0x%x\n", key);  //xltao add

	if (DownKey != NULL)
		DownKey(key);
#endif
	
}

//----------------------------------------------------------------------------

void check_power_off(void)
{
	unsigned int i;
	unsigned int group = POWER_OFF_PIN / 32, bit = 1 << (POWER_OFF_PIN % 32);

	for (i = 0; i < POFF_TRY_TIME ; i ++)
	{
		os_TimeDelayHMSM(0, 0, 0, POFF_EACH_TIME);
		if (INREG32(A_GPIO_PXPIN(group)) & bit)
			break;
		printf("Power key keep low! \n");
	}

	if ( i >= POFF_TRY_TIME )
	{
		info_power_off();
	}
	else
	{
		DownKeyHandle(0x40);
		UpKeyHandle(0x40);
	}
}

//#endif	// USE_MIDWARE

static unsigned int Key_Auto_Scan(unsigned int keycode)
{
	 int i;
	 for(i=0; i<KEY_NUM; i++)
	 {
		if((keydec[i].high_value>=keycode) && (keycode>=keydec[i].low_value))
		{
			return keydec[i].key_phy_value;
		}
	 }
	 return 0;
}

//----------------------------------------------------------------------------

static void key_interrupt_handler(unsigned int arg)
{
	disable_irq(EIRQ_GPIO_BASE + POWER_OFF_PIN);  //xltao add 
	disable_irq(EIRQ_GPIO_BASE + GPIO_KEY_PREV_PIN);  //xltao add 
	#if (!SERIAL_PORT_PRINTF) 
	disable_irq(EIRQ_GPIO_BASE + GPIO_KEY_NEXT_PIN);  //xltao add 
	#endif
	disable_irq(EIRQ_GPIO_BASE + AD_KEY_INT_PIN);

	printf("================fuck================\n");
	
	os_TaskChangePriority( KEY_TASK_PRIO + 150 , KEY_TASK_PRIO);
	os_SemaphorePost(key_sem);
}

//----------------------------------------------------------------------------
unsigned int value_adkey()
{
	unsigned int key_value ;
	int key_level1,key_level2;
	
	key_level1 =__gpio_get_pin(AD_KEY_INT_PIN);
 	key_value = read_adkey();
	key_level2 =__gpio_get_pin(AD_KEY_INT_PIN);
	
	if ((key_level1==key_level2)&&(key_level1==0))
		return key_value; 
	else 
		return 0;      		
}
//-----------------xltao add start----------------------
unsigned int get_key()
{
		unsigned int key_value=0;
			
	
		if (!__gpio_get_pin(AD_KEY_INT_PIN)){ 	
			key_value = value_adkey();
			key_value = Key_Auto_Scan(key_value);
			printf("==============fuck1==============\n");

		}else{

			#if (!SERAIL_PORT_PRINTF)	
			if (!__gpio_get_pin(GPIO_KEY_NEXT_PIN)){ //next
				key_value = 0x08; 
			}
			#endif

			if (!__gpio_get_pin(GPIO_KEY_PREV_PIN)){ //prev
				key_value = 0x20; 
			}
		}
		return key_value;
}
//-----------------xltao add end------------------------
void KeyTaskEntry(void *arg)
{
	unsigned char err;
	unsigned short i,run,count = 0;
	unsigned int group = POWER_OFF_PIN / 32, bit = 1 << (POWER_OFF_PIN % 32);
	unsigned int  key,oldkey,upkey = 0;	
	unsigned int  keyrepeat = KEY_DETECT_REPEAT_TIME / KEY_DELAY_TIME;
	unsigned int  key_value;
	
	
	while(1)
	{

		os_TaskChangePriority( KEY_TASK_PRIO, KEY_TASK_PRIO + 150 );
		os_SemaphorePend(key_sem, 0, &err);

		if (!(INREG32(A_GPIO_PXPIN(group)) & bit))
		{
			check_power_off();
			goto out;
		}

		//---------------------------------------	
		#if 0
		#else
			key_value = get_key();
		#endif
		//---------------------------------------
		oldkey|=key_value;
		run = 1;	
		count = 0;
		keyrepeat = KEY_DETECT_REPEAT_TIME / KEY_DELAY_TIME;

		key_value=0;
		while(run)
		{
			os_TimeDelay(KEY_DELAY_TIME);
			key = 0;
			//----------------------------------
			#if 0
			#else
				//mdelay(20);  //xltao add the line
				//mdelay(2);  //xltao add the line
				key_value = get_key();
			#endif
			//----------------------------------
			key |= key_value;
			
			if(key ^ oldkey)
			{
				oldkey = key;
				continue;
			}
			else
			{
				if (key)
				{
					if (key & (~upkey))
					{
						DownKeyHandle(key & (~upkey));
						printf("key pressed is %x\n", key & (~upkey));
					}
					else
					{
						if((key ^ upkey) & upkey)
						{
							UpKeyHandle((key ^ upkey) & upkey);
							printf("key realeased is %x\n", (key ^ upkey) & upkey);
						}
					}

					if (key == upkey)
					{
						count++;
						if (count > keyrepeat)
						{
							count = 0;
						    upkey = 0;
							keyrepeat = KEY_REPEAT_TIME / KEY_DELAY_TIME;
						}
					}
					else
					{
						count = 0;
					    upkey = key;
					}
				}
				else
				{
					if (upkey)
					{
						UpKeyHandle(upkey);
						printf("key realeased is %x\n", upkey);
					}
					run = 0;
					upkey =0;
				}
			}
		}
		


out:
		enable_irq(EIRQ_GPIO_BASE + GPIO_KEY_PREV_PIN);  //xltao add
		#if (!SERIAL_PORT_PRINTF) 
		enable_irq(EIRQ_GPIO_BASE + GPIO_KEY_NEXT_PIN);  //xltao add
		#endif
		enable_irq(EIRQ_GPIO_BASE + POWER_OFF_PIN);
		enable_irq(EIRQ_GPIO_BASE + AD_KEY_INT_PIN);
	}
}
//----------------------------------------------------------------------------

#define KeyID (('K' << 24) | ('E' << 16) | ('Y' << 16) | (':' << 16))
void KeyInit(void)
{
	unsigned int i;
	unsigned int group, bit;

#ifdef USE_MIDWARE

	keysrc.GetRequest = GetRequest;
	keysrc.Response = Response;
	keysrc.Name = "KEY";
	RegisterMidSrc((PMIDSRC)&keysrc);

	printf("Register Midware SRC Key! %d \n",keysrc.ID);
	keyid = keysrc.ID;

#endif

	printf("key Init+++++++++++++++++++++++++++++\n");
	// Step-1: Create Sem
	key_sem = os_SemaphoreCreate(0);
	
	// Step-2: Create Task
	os_TaskCreate(	KeyTaskEntry,
					(void *)0,
					(void *)&KeyTaskStack[KEY_TASK_STK_SIZE - 1],
					KEY_TASK_PRIO);
	
	// Step-3: Mask Intr and Config the Intr source
	gpio_intr_init(GITT_F_EDGE, POWER_OFF_PIN);
	__gpio_disable_pull(POWER_OFF_PIN);
	// Request Irq
	request_irq(EIRQ_GPIO_BASE + POWER_OFF_PIN, key_interrupt_handler, 0);

	#if 1 //xltao add start
	//prev key
	gpio_intr_init(GITT_F_EDGE, GPIO_KEY_PREV_PIN);  
	request_irq(EIRQ_GPIO_BASE + GPIO_KEY_PREV_PIN, key_interrupt_handler, 0);

	#if  (!SERIAL_PORT_PRINTF) 
	//next key	
	gpio_intr_init(GITT_F_EDGE, GPIO_KEY_NEXT_PIN);  
	request_irq(EIRQ_GPIO_BASE + GPIO_KEY_NEXT_PIN, key_interrupt_handler, 0);
	#endif
	#endif

	adkey_init(); //enable sadc
	gpio_intr_init(GITT_F_EDGE, AD_KEY_INT_PIN);
	__gpio_disable_pull(AD_KEY_INT_PIN);

	request_irq(EIRQ_GPIO_BASE + AD_KEY_INT_PIN , key_interrupt_handler, 0);

}

//----------------------------------------------------------------------------

int Key_GetPowerOn(void)
{
	unsigned int group = POWER_OFF_PIN / 32, bit = 1 << (POWER_OFF_PIN % 32);

	if (INREG32(A_GPIO_PXPIN(group)) & bit)
		return (1);
	else
		return (0);
}

#endif


