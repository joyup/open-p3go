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
*/

#ifndef   __OS_API_H__
#define   __OS_API_H__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


// perish printf
#if RELEASE
//#undef printf
//#define printf(x,args...)
#endif

// macros
#ifndef  NULL
#define  NULL                      ((void*)0)
#endif

#define  OS_TASK_STACK           unsigned int
#define  OS_TICKS_PER_SEC        100 

typedef  unsigned int               OS_CPU_SR;
#define  OS_ENTER_CRITICAL()     (cpu_sr = OSCPUSaveSR()) 
#define  OS_EXIT_CRITICAL()      (OSCPURestoreSR(cpu_sr))


#define  OS_NO_ERROR                     0u
#define  OS_ERROR_EVENT_TYPE             1u
#define  OS_ERROR_PEND_ISR               2u
#define  OS_ERROR_POST_NULL_PTR          3u
#define  OS_ERROR_PEVENT_NULL            4u
#define  OS_ERROR_POST_ISR               5u
#define  OS_ERROR_QUERY_ISR              6u
#define  OS_ERROR_INVALID_OPT            7u
#define  OS_ERROR_TASK_WAITING           8u
#define  OS_ERROR_PDATA_NULL             9u
#define  OS_TIMEOUT                     10u
#define  OS_TASK_NOT_EXIST              11u
#define  OS_ERROR_EVENT_NAME_TOO_LONG   12u
#define  OS_ERROR_FLAG_NAME_TOO_LONG    13u
#define  OS_ERROR_TASK_NAME_TOO_LONG    14u
#define  OS_ERROR_PNAME_NULL            15u
#define  OS_ERROR_TASK_CREATE_ISR       16u
#define  OS_ERROR_PEND_LOCKED           17u
#define  OS_MAIL_BOX_FULL               20u
#define  OS_QUEUE_FULL                  30u
#define  OS_QUEUE_EMPTY                 31u
#define  OS_TASK_PRIORITY_ALREADY_EXIST 40u
#define  OS_TASK_PRIORITY_ERROR         41u
#define  OS_TASK_PRIORITY_INVALID       42u
#define  OS_SEMAPHORE_OVERFLOW          50u
#define  OS_TASK_DELETE_ERROR           60u
#define  OS_TASK_DELETE_IDLE            61u
#define  OS_TASK_DELETE_REQ             62u
#define  OS_TASK_DELETE_ISR             63u
#define  OS_NO_MORE_TCB                 70u
#define  OS_TIME_NOT_DLY                80u
#define  OS_TIME_INVALID_MINUTES        81u
#define  OS_TIME_INVALID_SECONDS        82u
#define  OS_TIME_INVALID_MILLI          83u
#define  OS_TIME_ZERO_DLY               84u
#define  OS_TASK_SUSPEND_PRIORITY       90u
#define  OS_TASK_SUSPEND_IDLE           91u
#define  OS_TASK_RESUME_PRIORITY       100u
#define  OS_TASK_NOT_SUSPENDED         101u
#define  OS_MEMORY_INVALID_PART        110u
#define  OS_MEMORY_INVALID_BLKS        111u
#define  OS_MEMORY_INVALID_SIZE        112u
#define  OS_MEMORY_NO_FREE_BLKS        113u
#define  OS_MEMORY_FULL                114u
#define  OS_MEMORY_INVALID_PBLK        115u
#define  OS_MEMORY_INVALID_PMEM        116u
#define  OS_MEMORY_INVALID_PDATA       117u
#define  OS_MEMORY_INVALID_ADDR        118u
#define  OS_MEMORY_NAME_TOO_LONG       119u
#define  OS_ERROR_NOT_MUTEX_OWNER      120u
#define  OS_TASK_OPT_ERROR             130u
#define  OS_ERROR_DEL_ISR              140u
#define  OS_ERROR_CREATE_ISR           141u
#define  OS_FLAG_INVALID_PGRP          150u
#define  OS_FLAG_ERROR_WAIT_TYPE       151u
#define  OS_FLAG_ERROR_NOT_RDY         152u
#define  OS_FLAG_INVALID_OPT           153u
#define  OS_FLAG_GRP_DEPLETED          154u
#define  OS_ERROR_PIP_LOWER            160u
#define  OS_ERROR_TMR_INVALID_DLY      170u
#define  OS_ERROR_TMR_INVALID_PERIOD   171u
#define  OS_ERROR_TMR_INVALID_OPT      172u
#define  OS_ERROR_TMR_INVALID_NAME     173u
#define  OS_ERROR_TMR_NON_AVAIL        174u
#define  OS_ERROR_TMR_INACTIVE         175u
#define  OS_ERROR_TMR_INVALID_DEST     176u
#define  OS_ERROR_TMR_INVALID_TYPE     177u
#define  OS_ERROR_TMR_INVALID          178u
#define  OS_ERROR_TMR_ISR              179u
#define  OS_ERROR_TMR_NAME_TOO_LONG    180u
#define  OS_ERROR_TMR_INVALID_STATE    181u
#define  OS_ERROR_TMR_STOPPED          182u
#define  OS_ERROR_TMR_NO_CALLBACK      183u

// operation options
#define  OS_DELETE_NO_PEND     0u
#define  OS_DELETE_ALWAYS      1u

#define  OS_MAIL_BOX_POST_EX_NONE      0x00u
#define  OS_MAIL_BOX_POST_EX_BROADCAST 0x01u
#define  OS_MAIL_BOX_POST_EX_FRONT     0x02u
#define  OS_MAIL_BOX_POST_EX_NO_SCHED  0x04u

#define  OS_TASK_CREATE_EX_NONE        0x0000u
#define  OS_TASK_CREATE_EX_STK_CHK     0x0001u
#define  OS_TASK_CREATE_EX_STK_CLR     0x0002u
#define  OS_TASK_CREATE_EX_SAVE_FP     0x0004u

//add by hlyu
#define  OS_PRIO_SELF              0xFFu

// structs
typedef struct os_event
{
    unsigned char         byEventType;
    void                 *pEventPtr;
    unsigned short        wEventCount;
    unsigned short        wEventGroup;
    unsigned short        wEventTable[16];
    unsigned char         sEventName[16];
} OS_EVENT;


typedef struct os_flag_group
{
    unsigned char         byFlagType;
    void                 *pFlagWaitList;
    unsigned short        wFlags;
    unsigned char         sFlagName[16];
} OS_FLAG_GROUP;


typedef struct os_flag_node
{
    void                 *pFlagNodeNext;
    void                 *pFlagNodePrev;
    void                 *pFlagNodeTCB;
    void                 *pFlagNodeFlagGroup;
    unsigned short        wFlagNodeFlags;
    unsigned char         byFlagNodeWaitType;
} OS_FLAG_NODE;


typedef struct os_mail_box_data
{
    void                 *pMsg;
    unsigned short        pwEventTable[16];
    unsigned short        wEventGroup;
} OS_MAIL_BOX_DATA;


typedef struct os_memory
{
    void                 *pMemAddr;
    void                 *pMemFreeList;
    unsigned int          dwMemBlockSize;
    unsigned int          dwMemNBlocks;
    unsigned int          dwMemNFree;
    unsigned char         sMemName[16];
} OS_MEMORY;


typedef struct os_memory_data
{
    void                 *pAddr;
    void                 *pFreeList;
    unsigned int          dwBlockSize;
    unsigned int          dwNBlocks;
    unsigned int          dwNFree;
    unsigned int          dwNUsed;
} OS_MEMORY_DATA;


typedef struct os_mutex_data
{
    unsigned short        pwEventTable[16];
    unsigned short        wEventGroup;
    unsigned char         byValue;
    unsigned char         byOwnerPriority;
    unsigned char         byMutexPIP;
} OS_MUTEX_DATA;


typedef struct os_queue
{
    struct os_queue    *pstQueuePtr;
    void                **ppQueueStart;
    void                **ppQueueEnd;
    void                **ppQueueIn;
    void                **ppQueueOut;
    unsigned short        wQueueSize;
    unsigned short        wQueueEntries;
} OS_QUEUE;


typedef struct os_queue_data
{
    void                 *pMsg;
    unsigned short        wNMsgs;
    unsigned short        wQueueSize;
    unsigned short        pwEventTable[16];
    unsigned short        wEventGrp;
} OS_QUEUE_DATA;


typedef struct os_semaphore_data
{
    unsigned short        wCount;
    unsigned short        pwEventTable[16];
    unsigned short        wEventGroup;
} OS_SEMAPHORE_DATA;


typedef struct os_stack_data
{
    unsigned int          dwFree;
    unsigned int          dwUsed;
} OS_STACK_DATA;


typedef struct os_tcb
{
    OS_TASK_STACK      *pdwTCBStkPtr;
    void                 *pTCBExtPtr;
    OS_TASK_STACK      *pdwTCBStackBottom;
    unsigned int          dwTCBStkSize;
    unsigned short        wTCBOpt;
    unsigned short        wSTCBId;
    struct os_tcb      *pstTCBNext;
    struct os_tcb      *pstTCBPrev;
    OS_EVENT           *pstTCBEventPtr;
    void                 *pTCBMsg;
    OS_FLAG_NODE       *pstTCBFlagNode;
    unsigned short        wTCBFlagsReady;
    unsigned short        wTCBDelay;
    unsigned char         byTCBStatus;
    unsigned char         byTCBPendTimeout;
    unsigned char         byTCBPriority;
    unsigned char         byTCBX;
    unsigned char         byTCBY;
    unsigned short        wTCBBitX;
    unsigned short        wTCBBitY;
    unsigned char         byTCBRequestDelete;
    unsigned int          dwTCBCtxSwCtr;
    unsigned int          dwTCBCyclesTimeout;
    unsigned int          dwTCBCyclesStart;
    OS_TASK_STACK      *pdwTCBStackBase;
    unsigned int          dwTCBStackUsed;
    unsigned char         sTCBTaskName[16];
} OS_TCB;


// global variables
extern  OS_TCB         *g_OS_TCB_Current;


// functions
unsigned char       os_EventGetName             (OS_EVENT             *pevent, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

void                os_EventSetName             (OS_EVENT             *pevent, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

unsigned short      os_FlagAccept               (OS_FLAG_GROUP        *pgrp, 
                                                   unsigned short          flags, 
                                                   unsigned char           wait_type, 
                                                   unsigned char          *err);

OS_FLAG_GROUP    *os_FlagCreate               (unsigned short          flags, 
                                                   unsigned char          *err);

OS_FLAG_GROUP    *os_FlagDelete               (OS_FLAG_GROUP        *pgrp, 
                                                   unsigned char           opt, 
                                                   unsigned char          *err);

unsigned char       os_FlagGetName              (OS_FLAG_GROUP        *pgrp, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

void                os_FlagSetName              (OS_FLAG_GROUP        *pgrp, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

unsigned short      os_FlagPend                 (OS_FLAG_GROUP        *pgrp, 
                                                   unsigned short          flags, 
                                                   unsigned char           wait_type, 
                                                   unsigned short          timeout, 
                                                   unsigned char          *err);

unsigned short      os_FlagGetCausedFlags       (void);

unsigned short      os_FlagPost                 (OS_FLAG_GROUP        *pgrp, 
                                                   unsigned short          flags, 
                                                   unsigned char           opt, 
                                                   unsigned char          *err);

unsigned short      os_FlagQuery                (OS_FLAG_GROUP        *pgrp, 
                                                   unsigned char          *err);

void               *os_MailBoxAccept            (OS_EVENT             *pevent);

OS_EVENT         *os_MailBoxCreate            (void                   *msg);

OS_EVENT         *os_MailBoxDelete            (OS_EVENT             *pevent, 
                                                   unsigned char           opt, 
                                                   unsigned char          *err);

void               *os_MailBoxPend              (OS_EVENT             *pevent, 
                                                   unsigned short          timeout, 
                                                   unsigned char          *err);

unsigned char       os_MailBoxPost              (OS_EVENT             *pevent, 
                                                   void                   *msg);

unsigned char       os_MailBoxPostEx            (OS_EVENT             *pevent, 
                                                   void                   *msg, 
                                                   unsigned char           opt);

unsigned char       os_MailBoxQuery             (OS_EVENT             *pevent, 
                                                   OS_MAIL_BOX_DATA     *p_mbox_data);

OS_MEMORY        *os_MemoryCreate             (void                   *addr, 
                                                   unsigned int            nblks, 
                                                   unsigned int            blksize, 
                                                   unsigned char          *err);

void               *os_MemoryGetBlock           (OS_MEMORY            *pmem, 
                                                   unsigned char          *err);

unsigned char       os_MemoryGetName            (OS_MEMORY            *pmem, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

void                os_MemorySetName            (OS_MEMORY            *pmem, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

unsigned char       os_MemoryReleaseBlock       (OS_MEMORY            *pmem, 
                                                   void                   *pblk);

unsigned char       os_MemoryQuery              (OS_MEMORY            *pmem, 
                                                   OS_MEMORY_DATA       *p_mem_data);

unsigned char       os_MutexAccept              (OS_EVENT             *pevent, 
                                                   unsigned char          *err);

OS_EVENT         *os_MutexCreate              (unsigned char           prio, 
                                                   unsigned char          *err);

OS_EVENT         *os_MutexDelete              (OS_EVENT             *pevent, 
                                                   unsigned char           opt, 
                                                   unsigned char          *err);

void                os_MutexPend                (OS_EVENT             *pevent, 
                                                   unsigned short          timeout, 
                                                   unsigned char          *err);

unsigned char       os_MutexPost                (OS_EVENT             *pevent);

unsigned char       os_MutexQuery               (OS_EVENT             *pevent, 
                                                   OS_MUTEX_DATA        *p_mutex_data);

void               *os_QueueAccept              (OS_EVENT             *pevent, 
                                                   unsigned char          *err);

OS_EVENT         *os_QueueCreate              (void                  **start, 
                                                   unsigned short          size);

OS_EVENT         *os_QueueDelete              (OS_EVENT             *pevent, 
                                                   unsigned char           opt, 
                                                   unsigned char          *err);

unsigned char       os_QueueFlush               (OS_EVENT             *pevent);

void               *os_QueuePend                (OS_EVENT             *pevent, 
                                                   unsigned short          timeout, 
                                                   unsigned char          *err);

unsigned char       os_QueuePost                (OS_EVENT             *pevent, 
                                                   void                   *msg);

unsigned char       os_QueuePostIntoFront       (OS_EVENT             *pevent, 
                                                   void                   *msg);

unsigned char       os_QueuePostEx              (OS_EVENT             *pevent, 
                                                   void                   *msg, 
                                                   unsigned char           opt);

unsigned char       os_QueueQuery               (OS_EVENT             *pevent, 
                                                   OS_QUEUE_DATA        *p_q_data);

unsigned short      os_SemaphoreAccept          (OS_EVENT             *pevent);

OS_EVENT         *os_SemaphoreCreate          (unsigned short          cnt);

OS_EVENT         *os_SemaphoreDelete          (OS_EVENT             *pevent, 
                                                   unsigned char           opt, 
                                                   unsigned char          *err);

void                os_SemaphorePend            (OS_EVENT             *pevent, 
                                                   unsigned short          timeout, 
                                                   unsigned char          *err);

unsigned char       os_SemaphorePost            (OS_EVENT             *pevent);

unsigned char       os_SemaphoreQuery           (OS_EVENT             *pevent, 
                                                   OS_SEMAPHORE_DATA    *p_sem_data);

void                os_SemaphoreSet             (OS_EVENT             *pevent, 
                                                   unsigned short          cnt, 
                                                   unsigned char          *err);

unsigned char       os_TaskChangePriority       (unsigned char           oldprio, 
                                                   unsigned char           newprio);

unsigned char       os_TaskCreate               (void                   (*task)(void *p_arg), 
                                                   void                   *p_arg, 
                                                   OS_TASK_STACK        *ptos, 
                                                   unsigned char           prio);

int                 os_TaskInfo                 (void                   (*add_info)(int prio, int stat));

unsigned char       os_TaskCreateEx             (void                   (*task)(void *p_arg),
                                                   void                   *p_arg,
                                                   OS_TASK_STACK        *ptos,
                                                   unsigned char           prio,
                                                   unsigned short          id,
                                                   OS_TASK_STACK        *pbos,
                                                   unsigned int            stk_size,
                                                   void                   *pext,
                                                   unsigned short          opt);

unsigned char       os_TaskDelete               (unsigned char           prio);

unsigned char       os_TaskRequestDeleteSelf    (unsigned char           prio);

unsigned char       os_TaskGetName              (unsigned char           prio, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

void                os_TaskSetName              (unsigned char           prio, 
                                                   unsigned char          *pname, 
                                                   unsigned char          *err);

unsigned char       os_TaskResume               (unsigned char           prio);

unsigned char       os_TaskSuspend              (unsigned char           prio);

unsigned char       os_TaskCheckStack           (unsigned char           prio, 
                                                   OS_STACK_DATA        *p_stk_data);

unsigned char       os_TaskQuery                (unsigned char           prio, 
                                                   OS_TCB               *p_task_data);

void                os_TimeDelay                (unsigned short          ticks);

unsigned char       os_TimeDelayHMSM            (unsigned char           hours, 
                                                   unsigned char           minutes, 
                                                   unsigned char           seconds, 
                                                   unsigned short          milli);

unsigned char       os_TimeDelayResume          (unsigned char           prio);

unsigned int        os_TimeGet                  (void);

void                os_TimeSet                  (unsigned int            ticks);

void                os_SystemTick               (void);

void                os_Init                     (void);

void                os_EnterISR                 (void);

void                os_ExitISR                  (void);

void                os_ScheduleLock             (void);

void                os_ScheduleUnlock           (void);

void                os_SystemStart              (void);

void                os_StatisticsInit           (void);

unsigned short      os_Verson                   (void);

void                os_Dummy                    (void);

unsigned char       os_EventTaskReady           (OS_EVENT             *pevent, 
                                                   void                   *msg, 
                                                   unsigned char           msk);

void                os_EventTaskWait            (OS_EVENT             *pevent);

void                os_EventTimeout             (OS_EVENT             *pevent);

void                os_EventWaitListInit        (OS_EVENT             *pevent);

void                os_FlagInit                 (void);

void                os_FlagUnlink               (OS_FLAG_NODE         *pnode);

void                os_ZeroMemory               (unsigned char          *pdest, 
                                                   unsigned short          size);

void                os_memcpy                   (unsigned char          *pdest, 
                                                   unsigned char          *psrc, 
                                                   unsigned short          size);

void                os_MemoryInit               (void);

void                os_QueueInit                (void);

void                os_Schedule                 (void);

unsigned char       os_strcpy                   (unsigned char          *pdest, 
                                                   unsigned char          *psrc);

unsigned char       os_strlen                   (unsigned char          *psrc);

void                os_TaskIdle                 (void                   *p_arg);

void                os_TaskClearStack           (OS_TASK_STACK        *pbos, 
                                                   unsigned int            size, 
                                                   unsigned short          opt);

unsigned char       os_InitTCB                  (unsigned char           prio, 
                                                   OS_TASK_STACK        *ptos, 
                                                   OS_TASK_STACK        *pbos, 
                                                   unsigned short          id, 
                                                   unsigned int            stk_size, 
                                                   void                   *pext, 
                                                   unsigned short          opt);


typedef struct
{
	void (*Init)();
	void (*Deinit)();
	void (*Create)(OS_TCB *ptcb);
	void (*Close)(OS_TCB *ptcb);
	int (*TaskIdle)();
	int (*TaskStat)();	
	void (*TaskSw)();
	void (*TCBInit)(OS_TCB *ptcb);
	void (*TimeTick)();
	int (*Delay)(unsigned short ticks);
}OSBTaskBSPHook,*POSTaskBSPHook;

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__OS_API_H__
