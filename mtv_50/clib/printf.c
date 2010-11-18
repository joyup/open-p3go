
#include "libc.h"

int stdout_write(_PTR _cookie, const char *pData, int Size)
{
  int n;
  n = jz_std_puts(pData, Size);
  return n;
}

int
printf (const char *fmt, ...)
{
  
  int ret;
  va_list ap;
  FILE *stdout_ptr;
  unsigned int irqsave;
  
  irqsave = spin_lock_irqsave();
  stdout_ptr = _stdout_r (_REENT);
  spin_unlock_irqrestore(irqsave);

  va_start (ap, fmt);
  ret = vfprintf (stdout_ptr, fmt, ap);
  va_end (ap);
  return ret;
}

