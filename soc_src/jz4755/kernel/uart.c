/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4755  Driver  V1.0.0
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
 *  Create:   2008-09-24, by George Zhang
 *            
 *  Maintain: 2008-11-08, by  George Zhang
 *            
 *
 *******************************************************************************
 */
 
#include <bsp.h>
#include <jz4755.h>
#include <os_api.h>


void serial_setgpio(void)
{
#if (DEBUG_UART_BASE == UART0_BASE_U_VIRTUAL)
	UART0_GPIO_INIT();
	CLRREG32(A_CPM_CLKGR,CLKGR_STOP_UART0);
#elif (DEBUG_UART_BASE == UART1_BASE_U_VIRTUAL)
	UART1_GPIO_INIT();
	CLRREG32(A_CPM_CLKGR,CLKGR_STOP_UART1);
#else
	#error There are only two serial port!!!
#endif
}

void serial_setbrg (void)
{	
	unsigned short UART_DIVISOR_LATCH = DEVICE_CLOCK / 16 / DEBUG_UART_BAUDRATE;
	SETREG8( (DEBUG_UART_BASE + UART_ULCR_OFFSET), ULCR_DLAB );	
	OUTREG8( (DEBUG_UART_BASE + UART_UDLLR_OFFSET), UART_DIVISOR_LATCH & 0xFF );
	OUTREG8( (DEBUG_UART_BASE + UART_UDLHR_OFFSET), (UART_DIVISOR_LATCH >> 8) & 0xFF );
	CLRREG8( (DEBUG_UART_BASE + UART_ULCR_OFFSET), ULCR_DLAB );
}

int serial_init (void)
{
	serial_setgpio();

	OUTREG8( (DEBUG_UART_BASE + UART_UIER_OFFSET), 0 );
	CLRREG8( (DEBUG_UART_BASE + UART_UFCR_OFFSET), UFCR_UME );	
	CLRREG8( (DEBUG_UART_BASE + UART_UISR_OFFSET), (UISR_RCVEIR|UISR_XMITIR) );
	SETREG8( (DEBUG_UART_BASE + UART_ULCR_OFFSET), (ULCR_WLS_8BITS|ULCR_SBLS_1BIT) );
	serial_setbrg();
	SETREG8( (DEBUG_UART_BASE + UART_UFCR_OFFSET), (UFCR_FME | UFCR_RFRT | UFCR_TFRT | UFCR_UME | UFCR_RDTR_15) );

	return 0;
}

void serial_putc (const char c)
{
#if 1
	if (c == '\n') serial_putc ('\r');
	/* Wait for fifo to shift out some bytes */
	while ( !(INREG8((DEBUG_UART_BASE + UART_ULSR_OFFSET))&ULSR_TDRQ ) );
	OUTREG8((DEBUG_UART_BASE + UART_UTHR_OFFSET), c); 	
#endif
}
void serial_waitfinish()
{
	while(!(INREG8((DEBUG_UART_BASE + UART_ULSR_OFFSET)) & ULSR_TEMP ) );
}


void serial_puts (const char *s)
{
	while (*s)
		serial_putc (*s++);
}

int serial_tstc (void)
{
	/* Data in rfifo */
	return  (INREG8((DEBUG_UART_BASE + UART_ULSR_OFFSET)) & ULSR_DRY );
}

int serial_getc (void)
{
	while (!serial_tstc());
	return INREG8((DEBUG_UART_BASE + UART_URBR_OFFSET));
}
static  OS_EVENT  *jz_std_sem = NULL;
#define OP_STD_LOCK() do{ \
		unsigned char  err; \
		os_SemaphorePend(jz_std_sem, 0, &err); \
		}while(0)
#define OP_STD_UNLOCK() do{ \
		unsigned char  err; \
		os_SemaphorePost(jz_std_sem); \
		}while(0)

#if DRIVER_ONLY
unsigned char os_init_completed=1;
#else
extern unsigned char os_init_completed;
#endif
int jz_std_puts(const unsigned char *s,unsigned int count)
{
  unsigned int i = 0;
  if (os_init_completed)
  {
    if(jz_std_sem == NULL) jz_std_sem = os_SemaphoreCreate(1);
    OP_STD_LOCK();	
  }
  while(i++ < count)
   	serial_putc(*s++);
  if (os_init_completed)
    OP_STD_UNLOCK();
  return (count);
}

int jz_std_gets(unsigned char *s,unsigned int count)
{
  unsigned int i = 0;
  if(jz_std_sem == NULL) jz_std_sem = os_SemaphoreCreate(1);
  OP_STD_LOCK();	
  while(i++ < count)
   	*s++ = serial_getc();
  OP_STD_UNLOCK();
  return (count);
}

