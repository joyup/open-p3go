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
#if KEYTYPE == 1
#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>
#include <threadprio.h>
#ifdef USE_MIDWARE
	#include "midware.h"
#endif
#include <key.h>

// Just a Joke
#if (KEY_DOWN_VALUE == GPIO_HIGH_LEVEL)
	#ifdef KEY_SPECIAL_PIN
		#error	The hardware schematic is too bad!!!
	#endif
#endif

//----------------------------------------------------------------------------
//	Key Define
static unsigned int key_define[] =
{
	KEY_GAME_UP_PIN, KEY_GAME_DOWN_PIN, KEY_GAME_LEFT_PIN, KEY_GAME_RIGHT_PIN,
	KEY_GAME_A_PIN, KEY_GAME_B_PIN, KEY_GAME_L_PIN, KEY_GAME_R_PIN, 
	KEY_GAME_START_PIN, KEY_GAME_SELECT_PIN, KEY_GAME_BRIGHT_PIN
};

static unsigned int key_find[] =
{
	PK_UP, PK_DOWN, PK_LEFT, PK_RIGHT,
	PK_A, PK_B, PK_L, PK_R,
	PK_START, PK_SELECT, PK_BRIGHT
};

#define KEY_NUM		(sizeof(key_define) / sizeof(key_define[0]))

//----------------------------------------------------------------------------
//	Key Common Define
#define KEY_DETECT_REPEAT_TIME 50 //100
#define KEY_REPEAT_TIME 10 // 50
#define KEY_DELAY_TIME 1

#define POFF_EACH_TIME		100
#define POFF_TRY_TIME		20


#if (KEY_DOWN_VALUE == GPIO_LOW_LEVEL)
	#define	KEY_DOWN	GPIO_LOW_LEVEL
	#define KEY_UP		GPIO_HIGH_LEVEL
#else
	#define	KEY_DOWN	GPIO_HIGH_LEVEL
	#define KEY_UP		GPIO_LOW_LEVEL
#endif


//----------------------------------------------------------------------------
//	Key Task Define
#define KEY_TASK_STK_SIZE	1024
#define KEY_TASK_PRIO		KEY_THREAD_PRIO //1

static OS_EVENT *key_sem;
static OS_TASK_STACK KeyTaskStack[KEY_TASK_STK_SIZE];

//typedef void (*PFN_KEYHANDLE)(unsigned int key);

PFN_KEYHANDLE	UpKey = NULL, DownKey = NULL;

#ifdef USE_MIDWARE

volatile static MIDSRC keysrc;
volatile static unsigned int keyid;

//----------------------------------------------------------------------------

static void info_power_off(void)
{
	keysrc.Src = SRC_POWER_OFF;
	keysrc.Event = EVENT_POWER_OFF;

	os_QueuePost(keysrc.CurEvent1, (void *)&keyid);
	os_SemaphorePost(keysrc.CurEvent);
}

//----------------------------------------------------------------------------

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
			mdelay(200);
			if (INREG32(A_GPIO_PXPIN(group)) & bit)
				jz_pm_hibernate();
		}
	}
	else
		printf("Can not power off !\n");
}

//----------------------------------------------------------------------------

static void UpKeyHandle(int key)
{
	os_QueuePost(keysrc.CurEvent1, (void *)&keyid);

	keysrc.Src = SRC_KEY;
	os_SemaphorePost(keysrc.CurEvent);
	printf("Key Up! 0x%x\n", key);

	if (UpKey != NULL)
		UpKey(key);
}

//----------------------------------------------------------------------------

static int DownKeyHandle(int key)
{
	unsigned char err;

	os_QueuePost(keysrc.CurEvent1, (void *)&keyid);

	keysrc.Src = SRC_KEY_DOWN;
	os_SemaphorePost(keysrc.CurEvent);
	printf("Key Down! 0x%x\n", key);

	if (DownKey != NULL)
		DownKey(key);
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
		#ifdef USE_MIDWARE
			DownKeyHandle(0x40);
			UpKeyHandle(0x40);
		#endif
	}
}

#endif	// USE_MIDWARE

//----------------------------------------------------------------------------

unsigned int get_gpio_pin(unsigned int pin)
{
	bool	flag = 0;
	unsigned int group = pin / 32, bit = 1 << (pin % 32);

#ifdef KEY_SPECIAL_PIN
	if (pin == KEY_SPECIAL_PIN)
		flag = 1;
#endif

#if (KEY_DOWN_VALUE == GPIO_HIGH_LEVEL)
	flag = 1;
#endif

	if (INREG32(A_GPIO_PXPIN(group)) & bit)
		return (flag ? KEY_DOWN : KEY_UP);
	else
		return (flag ? KEY_UP : KEY_DOWN);
}

//----------------------------------------------------------------------------

static void key_interrupt_handler(unsigned int arg)
{
	disable_irq(EIRQ_GPIO_BASE + POWER_OFF_PIN);

	for (arg = 0; arg < KEY_NUM; arg++)
		disable_irq(EIRQ_GPIO_BASE + key_define[arg]);

	os_TaskChangePriority(KEY_TASK_PRIO + 150, KEY_TASK_PRIO);
	os_SemaphorePost(key_sem);
}

unsigned int ReadKeyDirect(void)
{
	unsigned int key, i;
	key = 0;
	
	for (i = 0; i < KEY_NUM; i++)
	{
		if ( get_gpio_pin(key_define[i]) == KEY_DOWN )
			key |= key_find[i];
	}

	unsigned int ret = 0;

    if(key & PK_UP){    // 向上
        ret |= 0x01;
	}
    if(key & PK_DOWN){    // 向下
        ret |= 0x02;
    }
    if(key & PK_LEFT){    // LEFT
        ret |= 0x04;
    }
    if(key & PK_RIGHT){    // RIGHT
        ret |= 0x08;
    }
    if(key & PK_L){    // L
        ret |= 0x10;
    }
    if(key & PK_R){    // R
        ret |= 0x20;
    }	
    if(key & PK_A){    // A
        ret |= 0x40;
	}
    if(key & PK_B){    // B
        ret |= 0x80;
	}	
    if(key & PK_START){    // menu
        ret |= 0x200;
    }
	if(key & PK_SELECT){
		ret |= 0x400;
	}
	//Special function keys
	if(key == (PK_SELECT | PK_R)){    // menu
		ret = 0x800;
	}

	if(key & PK_BRIGHT){
		ret |= 0x1000;
	}
	return ret;
}

//----------------------------------------------------------------------------

static void KeyTaskEntry(void *arg)
{
	unsigned char err;
	unsigned short i, run, count = 0;
	unsigned int key, oldkey, upkey = 0;
	unsigned int keyrepeat = KEY_DETECT_REPEAT_TIME / KEY_DELAY_TIME;
	unsigned int group, bit;

	printf("Key Install\r\n");
	while (1)
	{
		os_TaskChangePriority(KEY_TASK_PRIO, KEY_TASK_PRIO + 150);
		os_SemaphorePend(key_sem, 0, &err);

#ifdef USE_MIDWARE
		group = POWER_OFF_PIN / 32;
		bit = 1 << (POWER_OFF_PIN % 32);

		if (!(INREG32(A_GPIO_PXPIN(group)) & bit))
		{
			check_power_off();
			goto out;
		}
#endif
		oldkey = 0;
		for (i = 0; i < KEY_NUM; i++)
		{
#if 1
			if (get_gpio_pin(key_define[i]) == KEY_DOWN)
				oldkey |= key_find[i];
#else
			group = key_define[i] / 32;
			bit = 1 << (key_define[i] % 32);
			if (!(INREG32(A_GPIO_PXPIN(group)) & bit))
				oldkey |= key_find[i];
#endif
		}

		run = 1;
		count = 0;
		keyrepeat = KEY_DETECT_REPEAT_TIME / KEY_DELAY_TIME;
		while (run)
		{
			os_TimeDelay(KEY_DELAY_TIME);
			key = 0;
			for (i = 0; i < KEY_NUM; i++)
			{
#if 1
				if ( get_gpio_pin(key_define[i]) == KEY_DOWN )
					key |= key_find[i];
#else
				if ( __gpio_get_pin(key_define[i]) == 0 )
					key |= key_find[i];
#endif
			}

			if (key ^ oldkey)
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
						#ifdef USE_MIDWARE
							DownKeyHandle(key & (~upkey));
						#endif
						printf("key pressed is %x\n", key & (~upkey));
					}
					else
					{
						if((key ^ upkey) & upkey)
						{
							#ifdef USE_MIDWARE
								UpKeyHandle((key ^ upkey) & upkey);
							#endif
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
						#ifdef USE_MIDWARE
							UpKeyHandle(upkey);
						#endif
						printf("key realeased is %x\n", upkey);
					}
					run = 0;
					upkey =0;
				}
			}
		}

out:
		enable_irq(EIRQ_GPIO_BASE + POWER_OFF_PIN);
		for(i = 0; i < KEY_NUM; i++)
			enable_irq(EIRQ_GPIO_BASE + key_define[i]);
	}
}

//----------------------------------------------------------------------------

void KeyInit(void)
{
	unsigned int i;
	unsigned int group, bit;

#ifdef USE_MIDWARE

	keysrc.GetRequest = GetRequest;
	keysrc.Response = Response;
	keysrc.Name = "KEY";
	RegisterMidSrc((PMIDSRC)&keysrc);

	printf("Register Midware SRC Key! %d \n", keysrc.ID);
	keyid = keysrc.ID;

#endif

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

	for (i = 0; i < KEY_NUM; i++)
	{
#if (KEY_DOWN == GPIO_LOW_LEVEL)
	#ifdef KEY_SPECIAL_PIN
		if (key_define[i] == KEY_SPECIAL_PIN)
			gpio_intr_init(GITT_R_EDGE, key_define[i]);
		else
	#endif
			gpio_intr_init(GITT_F_EDGE, key_define[i]);

#else
	#ifdef KEY_SPECIAL_PIN
		if (key_define[i] == KEY_SPECIAL_PIN)
			gpio_intr_init(GITT_F_EDGE, key_define[i]);
		else
	#endif
			gpio_intr_init(GITT_R_EDGE, key_define[i]);
#endif
		__gpio_disable_pull(key_define[i]);
		request_irq(EIRQ_GPIO_BASE + key_define[i], key_interrupt_handler, 0);
	}
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

//----------------------------------------------------------------------------

//2009.01.09 danny added
void KeyInterruptDisable()
{
	int i;
	disable_irq(EIRQ_GPIO_BASE + POWER_OFF_PIN);

	for (i = 0; i < KEY_NUM; i++)
		disable_irq(EIRQ_GPIO_BASE + key_define[i]);
}

void KeyInterruptEnable()
{
	int i;
	enable_irq(EIRQ_GPIO_BASE + POWER_OFF_PIN);
	for(i = 0; i < KEY_NUM; i++)
		enable_irq(EIRQ_GPIO_BASE + key_define[i]);
}
#endif

