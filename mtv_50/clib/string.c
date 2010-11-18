
#define NULL ((void *)0)

#define _U      01
#define _L      02
#define _N      04
#define _S      010
#define _P      020
#define _C      040
#define _X      0100
#define _B      0200

char _ctype_[1 + 256] = {
        0,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _C,     _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C,     _C,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _S|_B,  _P,     _P,     _P,     _P,     _P,     _P,     _P,
        _P,     _P,     _P,     _P,     _P,     _P,     _P,     _P,
        _N,     _N,     _N,     _N,     _N,     _N,     _N,     _N,
        _N,     _N,     _P,     _P,     _P,     _P,     _P,     _P,
        _P,     _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U,
        _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
        _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
        _U,     _U,     _U,     _P,     _P,     _P,     _P,     _P,
        _P,     _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L,
        _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
        _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
        _L,     _L,     _L,     _P,     _P,     _P,     _P,     _C
};

int
isdigit(int c)
{
        return((_ctype_ + 1)[c] & _N);
}

int
isalpha(int c)
{
        return((_ctype_ + 1)[c] & (_U|_L));
}

int
isspace(int c)
{
        return((_ctype_ + 1)[c] & _S);
}

int
isprint(int c)
{
        return((_ctype_ + 1)[c] & (_P|_U|_L|_N|_B));
}

int
islower(int c)
{
        return((_ctype_ + 1)[c] & _L);
}

int
isupper(int c)
{
        return((_ctype_ + 1)[c] & _U);
}

int
isxdigit(int c)
{
        return((_ctype_ + 1)[c] & ((_X)|(_N)));
}

int
toupper(int c)
{
  return islower(c) ? c - 'a' + 'A' : c;
}

int
tolower(int c)
{
        return isupper(c) ? (c) - 'A' + 'a' : c;
}

int strncasecmp (char *s1, char *s2, int n)
{
  if (n == 0)
    return 0;

  while (n-- != 0 && tolower(*s1) == tolower(*s2))
    {
      if (n == 0 || *s1 == '\0' || *s2 == '\0')
        break;
      s1++;
      s2++;
    }

  return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

void *memchr(void *src_void, int c, int length)
{
  unsigned char *src = (unsigned char *) src_void;

  c &= 0xff;

  while (length--)
    {
      if (*src == c)
        return (char *) src;
      src++;
    }
  return NULL;
}

char *strncat(char *s1, char *s2, int n)
{
  char *s = s1;

  while (*s1)
    s1++;
  while (n-- != 0 && (*s1++ = *s2++))
    {
      if (n == 0)
        *s1 = '\0';
    }

  return s;
}

int strcspn(char *s1, char *s2)
{
  char *s = s1;
  char *c;

  while (*s1)
    {
      for (c = s2; *c; c++)
        {
          if (*s1 == *c)
            break;
        }
      if (*c)
        break;
      s1++;
    }

  return s1 - s;
}

int strspn (char *s1, char *s2)
{
  char *s = s1;
  char *c;

  while (*s1)
    {
      for (c = s2; *c; c++)
        {
          if (*s1 == *c)
            break;
        }
      if (*c == '\0')
        break;
      s1++;
    }

  return s1 - s;
}
  
char *strcat(char *s1, char *s2)
{
  char *s = s1;

  while (*s1)
    s1++;

  while (*s1++ = *s2++)
    ;
  return s;
}

int memcmp(void *m1, void *m2, int n)
{
  unsigned char *s1 = (unsigned char *) m1;
  unsigned char *s2 = (unsigned char *) m2;

  while (n--)
    {
      if (*s1 != *s2)
        {
          return *s1 - *s2;
        }
      s1++;
      s2++;
    }
  return 0;
}

char *strncpy (char *dst0, char *src0, int count)
{
  char *dscan;
  char *sscan;

  dscan = dst0;
  sscan = src0;
  while (count > 0)
    {
      --count;
      if ((*dscan++ = *sscan++) == '\0')
        break;
    }
  while (count-- > 0)
    *dscan++ = '\0';

  return dst0;
}

int strncmp(char *s1, char *s2, int n)
{
  if (n == 0)
    return 0;

  while (n-- != 0 && *s1 == *s2)
    {
      if (n == 0 || *s1 == '\0')
        break;
      s1++;
      s2++;
    }

  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

int strcasecmp(char *s1, char *s2)
{
  while (*s1 != '\0' && tolower(*s1) == tolower(*s2))
    {
      s1++;
      s2++;
    }

  return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

char *strstr(char *searchee, char *lookfor)
{
  if (*searchee == 0)
    {
      if (*lookfor)
        return (char *) NULL;
      return (char *) searchee;
    }

  while (*searchee)
    {
      int i;
      i = 0;

      while (1)
        {
          if (lookfor[i] == 0)
            {
              return (char *) searchee;
            }

          if (lookfor[i] != searchee[i])
            {
              break;
            }
          i++;
        }
      searchee++;
    }

  return (char *) NULL;
}

char *strchr(char *s1, int i)
{
  unsigned char *s = (unsigned char *)s1;
  unsigned char c = (unsigned int)i;

  while (*s && *s != c)
    {
      s++;
    }

  if (*s != c)
    {
      s = NULL;
    }

  return (char *) s;
}

char* strcpy(char *dst0, char *src0)
{
  char *s = dst0;

  while (*dst0++ = *src0++)
    ;

  return s;
}

char *strrchr(char *s, int i)
{
  char *last = NULL;
  char c = i;

  while (*s)
    {
      if (*s == c)
        {
          last = s;
        }
      s++;
    }

  if (*s == c)
    {
      last = s;
    }

  return (char *) last;
}

void *memmove(void* dst_void, void* src_void, int length)
{
  char *dst = dst_void;
  char *src = src_void;

  if (src < dst && dst < src + length)
    {
      /* Have to copy backwards */
      src += length;
      dst += length;
      while (length--)
        {
          *--dst = *--src;
        }
    }
  else
    {
      while (length--)
        {
          *dst++ = *src++;
        }
    }

  return dst_void;
}

#if 0
void *memcpy(void* dst0, void* src0, int len0)
{
  char *dst = (char *) dst0;
  char *src = (char *) src0;

  void* save = dst0;

  while (len0--)
    {
      *dst++ = *src++;
    }

  return save;
}

void *memset(void* m, int c, int n)
{
  char *s = (char *) m;

  while (n-- != 0)
    {
      *s++ = (char) c;
    }

  return m;
}
#endif

int strlen(char *str)
{
  char *start = str;

  while (*str)
    str++;

  return str - start;
}

int strcmp(char *s1, char *s2)
{
  while (*s1 != '\0' && *s1 == *s2)
    {
      s1++;
      s2++;
    }

  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

#define RAND_MAX 0x7fffffff
unsigned long long _rand_next;

void srand(unsigned int seed)
{
        _rand_next = seed;
}

int rand (void)
{
  /* This multiplier was obtained from Knuth, D.E., "The Art of
     Computer Programming," Vol 2, Seminumerical Algorithms, Third
     Edition, Addison-Wesley, 1998, p. 106 (line 26) & p. 108 */
  _rand_next = _rand_next * (long long)6364136223846793005LL + 1;
  return (int)((_rand_next >> 32) & RAND_MAX);
}



