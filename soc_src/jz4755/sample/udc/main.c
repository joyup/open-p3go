/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/
#include <os_api.h>
#include <nand_api.h>

/* File local definitions */
#define  TASK_START_PRIO 40
#define  TASK_STK_SIZE   1024 *128                      /* Size of each task's stacks (# of WORDs) */


#define NAND_512_TEST 	1

OS_TASK_STACK   TaskStartStk[TASK_STK_SIZE];
unsigned char os_malloc_completed = 0;
unsigned char os_init_completed = 0;
/* ********************************************************************* */
/* Local functions */
void __umoddi3()
{;}

void __udivdi3(){;}

void TaskStart (void *data)
{
	JZ_StartTicker(10);	/* os_cfg.h */
	init_udc_nand();
    while(1)
	{
		os_TimeDelayHMSM(0, 0, 1, 0);     /* Wait one second */        
    }
}


/* ********************************************************************* */
/* Global functions */

void APP_vMain (void)
{
	heapInit();
	os_malloc_completed = 1;
	os_init_completed = 1;
	printf("MINIOS:Start test----\n");	
	os_TaskCreate(TaskStart, 
				(void *)0, 
				(void *)&TaskStartStk[TASK_STK_SIZE - 1], 
				TASK_START_PRIO);
	
	os_SystemStart();                              /* Start multitasking */
	
	while(1);
}
/* ********************************************************************* */
