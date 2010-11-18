/*

#define fflush fflushx
#define vfprintf vfprintfx 
#define vsnprintf vsnprintfx

#include <stdio.h>
#include <stdarg.h>
#include "jzfs_api.h"


#undef vsnprintf
#undef fflush
#undef vfprintf

#undef FILE
#define FILE FS_FILE 

int printf(const char *fmt, ...) 
{
	return 0;
}        
int sprintf (char *__restrict str, __const char *__restrict fmt, ...)
{
	return 0;
}
int vsnprintf (char *__restrict str, size_t size,
                      __const char *__restrict fmt, _G_va_list ap)
{
	return 0;
}
FS_size_t stdout_write(const void *pData, FS_size_t Size, FS_size_t N, FILE *pFile)
{
	return 0;
}
int vfprintf_ucos (FILE *__restrict fp, __const char *__restrict fmt0,
                     _G_va_list ap)
{
	return 0;
}
*/
void memcpy()
{
}
void memset()
{
}
int
isdigit(int c)
{
}

int
isalpha(int c)
{
}

int
isspace(int c)
{
}

int
isprint(int c)
{
}

int
islower(int c)
{
}

int
isupper(int c)
{
}

int
toupper(int c)
{
}

int
tolower(int c)
{
}

int strncasecmp (char *s1, char *s2, int n)
{
}

void *memchr(void *src_void, int c, int length)
{
}

char *strncat(char *s1, char *s2, int n)
{
}

int strcspn(char *s1, char *s2)
{
}

int strspn (char *s1, char *s2)
{
}
  
char *strcat(char *s1, char *s2)
{
}

int memcmp(void *m1, void *m2, int n)
{
}

char *strncpy (char *dst0, char *src0, int count)
{
}

int strncmp(char *s1, char *s2, int n)
{
}

int strcasecmp(char *s1, char *s2)
{
}

char *strstr(char *searchee, char *lookfor)
{
}

char *strchr(char *s1, int i)
{
}

char* strcpy(char *dst0, char *src0)
{
}

char *strrchr(char *s, int i)
{
}

void *memmove(void* dst_void, void* src_void, int length)
{
}



int strlen(char *str)
{
}

int strcmp(char *s1, char *s2)
{
}

void srand(unsigned int seed)
{
}

int rand (void)
{
}



/* ============= printf serial functios ===================== */
#define fflush fflushx
#define vfprintf vfprintfx 
#define fprintf fprintfx
#define ferror ferrorx
#define fputs fputsx
//#include <stdio.h>
#include <stdarg.h>
#include "jzfs_api.h"
#undef fflush
#undef vfprintf
#undef fprintf
#undef ferror
#undef fputs
#undef FILE
#define FILE JZFS_FILE 

unsigned int __errno_location;
unsigned int stderr,stdout;

int cantwrite (FILE * fp)
{
  return 0;
}

/* Flush a single file, or (if fp is NULL) all files.  */

int fflush (FILE * fp)
{
}


/*
 * Write some memory regions.  Return zero on success, EOF on error.
 *
 * This routine is large and unsightly, but most of the ugliness due
 * to the three different kinds of output buffering is handled here.
 */

int __sfvwrite ()
{
}

/*
 * Flush out all the vectors defined by the given uio,
 * then reset it so that it can be reused.
 */
static int __sprint()
{
}
void __cxa_pure_virtual()
{
}
#if 0
void __libc_write()
{
}
void __libc_read()
{
}
#endif

int _mbtowc_r ()
{
}

int 
_vfprintf_r()
{
}
#if 0
int vfprintf (FILE *__restrict fp, __const char *__restrict fmt0,
                     _G_va_list ap)
{
}
#endif
int fprintf() 
{
	
}
int sprintf()
{
}

int vsnprintf ()
{
}

int snprintf ()
{
}

int stdout_write()
{
}
int printf ()
{

}
void exit()
{}
void ferror()
{
}
void fputs()
{
}
void __assert_fail()
{
}

