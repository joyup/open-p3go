/*
 *  XBurst  mobile_tv  Project  V1.0.0
 *  Copyright (C) 2006 - 2007 Ingenic Semiconductor Inc.
 *  Author: <xliu@ingenic.cn>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  2007-12-27, xliu
 *  Created.
 */

#include <bsp.h>
#include <os_api.h>
#include "nand_api.h"
#include "function.h"

unsigned char os_malloc_completed = 0;
unsigned char os_init_completed = 0;
unsigned int *pps3_boot = (unsigned int *)BOOT_COMMON_PARAMETER_ADDR;

#define	LOADER_TASK_PRI		10
#define	TASK_STK_SIZE		1024 * 2
static	OS_TASK_STACK		LoaderTaskStk[TASK_STK_SIZE];
extern void usb_main(void *data);

void StartLoaderTask(void)
{
    unsigned int  size = TASK_STK_SIZE;
    OS_TASK_STACK *ptos = &LoaderTaskStk[TASK_STK_SIZE - 1];
    OS_TASK_STACK *pbos = &LoaderTaskStk[0];

    os_TaskCreateEx(usb_main,
                    (void *)0,
                    ptos,
                    LOADER_TASK_PRI,
                    LOADER_TASK_PRI,
                    pbos,
                    size,
                    (void *)0,
                    OS_TASK_CREATE_EX_STK_CHK | OS_TASK_CREATE_EX_STK_CLR);
}

// Ö÷³ÌÐò
void APP_vMain(void)
{
    os_Init();
    heapInit();

    StartLoaderTask();
    os_SystemStart();                              /* Start multitasking */

    while(1);
}
// END
