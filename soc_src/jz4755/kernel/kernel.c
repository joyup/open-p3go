#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>
#include <kernel.h>
#include <lcd_app.h>
#undef printf
//----------------------------------------------------------------------------
int osIdleDelay(unsigned short ticks); //from timer.c

int kernel_ioctl(void *buff, unsigned int cmd)
{
	extern OSBTaskBSPHook jz4755Bsp;
	int result = 0;
	switch(cmd)
	{
		case KERNEL_STOP_SCHEDULE:
			JZ_StopTicker();
			InitAssistTimer();
			jz4755Bsp.Delay = osIdleDelay;
			result = 1;
			break;
		case KERNEL_RESTART_SCHEDULE:
			StopAssistTimer();
			JZ_StartTicker(OS_TICKS_PER_SEC);
			jz4755Bsp.Delay = NULL;
			result = 1;
			break;
		case KERNEL_PM_CONTROL:
			return jz_pm_control(*(unsigned int *)buff);
			break;
	}
	return result;
}

static OS_EVENT *sft_sem = NULL;

void sft_interrupt_set()
{
	*(volatile unsigned int *)0xb3016000=0;
}
static void sft_interrupt_handler(unsigned int arg)
{
	*(volatile unsigned int *)0xb3016000=1 ;
	os_SemaphorePost(sft_sem);
  //printf("soft interrupt_handler!\n");
}

void wait_for_sft()
{
  unsigned char err;  
	os_SemaphorePend(sft_sem, 0, &err);
}

void init_sft_interrupt()
{  
	unsigned char err;
	if(sft_sem == NULL)
	{	
		sft_sem = os_SemaphoreCreate(0);
		request_irq(IRQ_SFT, sft_interrupt_handler, 0);
  }
  os_SemaphoreSet(sft_sem,0,&err);
}

