
#define fflush fflush_tmp

int feof (void)
{
  return 0;
}

#define feof feof_fake

#include <os_api.h>
#include <mobile_tv.h>

char *_user_strerror(int errnum)
{
  return 0;
}

int
libc_isatty (int fd)
{
  return 0;
}

void *drv_malloc(int size)
{
  return (void *)alloc (size);
}

int libc_fstat_r()
{
  return -1;
}

void __assert_fail ()
{
  printf ("+++++++++ calling ___assert_fail +++++++++\n");
  return;
}

int mkstemp64 ()
{
  printf ("+++++++++ calling mkstemp64 +++++++++\n");
  return -1;
}

int ioctl ()
{
  printf ("+++++++++ calling ioctl +++++++++\n");
  return 0;
}

void abort ()
{
  printf ("+++++++++ calling abort +++++++++\n");
  abort();
}

int __xstat64 ()
{
  printf ("+++++++++ calling xstat64 +++++++++\n");
  return 0;
}

int __fxstat64 ()
{
  printf ("+++++++++ calling fxstat64 +++++++++\n");
  return 0;
}

int write ()
{
  printf ("+++++++++ calling write +++++++++\n");
  return 0;
}


struct timeval_uc
{
  long int tv_sec;
  long int tv_usec;
};

struct timezone_uc
{
  int tz_minuteswest;
  int tz_dsttime;
};

int
gettimeofday (struct timeval_uc *tp, struct timezone_uc *tzp)
{
  if (tp)
    {
      unsigned int dm[2];
//      JZ_GetTimer_20ms(dm);
      tp->tv_sec = dm[1]/50;
      tp->tv_usec = (dm[1] % 50) * 20 * 1000 + dm[0];
    }

  /* Return fixed data for the timezone */
  if (tzp)
    {
      tzp->tz_minuteswest = 0;
      tzp->tz_dsttime = 0;
    }

  return 0;
}

