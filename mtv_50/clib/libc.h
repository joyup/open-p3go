
#include "os_api.h"

#undef printf
#define fread  jzfs_Read

#define malloc drv_malloc
#define _malloc_r(ptr, size) drv_malloc(size)

#define _realloc_r(reent, ptr, size)  Drv_realloc(ptr, size)

#define free(ptr) deAlloc(ptr)
#define _free_r(ptr0, ptr) deAlloc(ptr)

#define calloc Drv_calloc

#define _fstat_r  libc_fstat_r
#define _errno libc_errno
#define isatty libc_isatty

# define HUGE_VAL \
  (__extension__                                                              \
   ((union { unsigned __l __attribute__((__mode__(__DI__))); double __d; })   \
    { __l: 0x7ff0000000000000ULL }).__d)


/* Daniel */
#define USE_STDIO_FILE 0
#define UC_FIXED

extern unsigned char os_malloc_completed;
extern unsigned char os_init_completed;


/* ------- sys/types.h ------------ */
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;

/* ------- sys/config.h ------------ */
#define _POINTER_INT long
typedef int __int32_t;
typedef unsigned int __uint32_t;

/* ------- gcc/include/stddef.h  ------------ */
//typedef long unsigned int size_t;
typedef unsigned int size_t;
typedef int wchar_t;

/* ------- _ansi.h  ------------ */

#define _HAVE_STDC

//#define NULL            0L
#define _PTR            void *
#define _AND            ,
#define _CONST          const
#define _DOTS           , ...
#define _EXFUN(name, proto)             name proto
#define _DEFUN(name, arglist, args)     name(args)
#define _CAST_VOID (void)
#ifndef _PARAMS
#define _PARAMS(paramlist)              paramlist
#endif

#ifndef _LONG_DOUBLE
#define _LONG_DOUBLE long double
#endif

/* ------- time.h  ------------ */

typedef unsigned long  clock_t;
typedef long  time_t;
struct tm
{
  int   tm_sec;
  int   tm_min;
  int   tm_hour;
  int   tm_mday;
  int   tm_mon;
  int   tm_year;
  int   tm_wday;
  int   tm_yday;
  int   tm_isdst;
};

/* ------- stdlib.h  ------------ */

typedef struct
{
  int quot; /* quotient */
  int rem; /* remainder */
} div_t;

typedef struct
{
  long quot; /* quotient */
  long rem; /* remainder */
} ldiv_t;

/* ------- sys/reent.h  ------------ */

struct _glue
{
  struct _glue *_next;
  int _niobs;
  struct __sFILE *_iobs;
};

struct _Bigint
{
  struct _Bigint *_next;
  int _k, _maxwds, _sign, _wds;
  unsigned long _x[1];
};

/*
 * atexit() support
 */

#define _ATEXIT_SIZE 32 /* must be at least 32 to guarantee ANSI conformance */

struct _atexit {
        struct  _atexit *_next;                 /* next in list */
        int     _ind;                           /* next index in this table */
        void    (*_fns[_ATEXIT_SIZE])(void);    /* the table itself */
};

/*
 * Stdio buffers.
 *
 * This and __sFILE are defined here because we need them for struct _reent,
 * but we don't want stdio.h included when stdlib.h is.
 */

struct __sbuf {
        unsigned char *_base;
        int     _size;
};

/*
 * We need fpos_t for the following, but it doesn't have a leading "_",
 * so we use _fpos_t instead.
 */

typedef long _fpos_t;           /* XXX must match off_t in <sys/types.h> */
                                /* (and must be `long' for now) */


/*
 * Stdio state variables.
 *
 * The following always hold:
 *
 *	if (_flags&(__SLBF|__SWR)) == (__SLBF|__SWR),
 *		_lbfsize is -_bf._size, else _lbfsize is 0
 *	if _flags&__SRD, _w is 0
 *	if _flags&__SWR, _r is 0
 *
 * This ensures that the getc and putc macros (or inline functions) never
 * try to write or read from a file that is in `read' or `write' mode.
 * (Moreover, they can, and do, automatically switch from read mode to
 * write mode, and back, on "r+" and "w+" files.)
 *
 * _lbfsize is used only to make the inline line-buffered output stream
 * code as compact as possible.
 *
 * _ub, _up, and _ur are used when ungetc() pushes back more characters
 * than fit in the current _bf, or when ungetc() pushes back a character
 * that does not match the previous one in _bf.  When this happens,
 * _ub._base becomes non-nil (i.e., a stream has ungetc() data iff
 * _ub._base!=NULL) and _up and _ur save the current values of _p and _r.
 */

struct __sFILE {
  unsigned char *_p;	/* current position in (some) buffer */
  int	_r;		/* read space left for getc() */
  int	_w;		/* write space left for putc() */
  short	_flags;		/* flags, below; this FILE is free if 0 */
  short	_file;		/* fileno, if Unix descriptor, else -1 */
  struct __sbuf _bf;	/* the buffer (at least 1 byte, if !NULL) */
  int	_lbfsize;	/* 0 or -_bf._size, for inline putc */

  /* operations */
  _PTR	_cookie;	/* cookie passed to io functions */

  int	_EXFUN((*_read),(_PTR _cookie, char *_buf, int _n));
  int	_EXFUN((*_write),(_PTR _cookie, const char *_buf, int _n));
  _fpos_t _EXFUN((*_seek),(_PTR _cookie, _fpos_t _offset, int _whence));
  int	_EXFUN((*_close),(_PTR _cookie));

  /* separate buffer for long sequences of ungetc() */
  struct __sbuf _ub;	/* ungetc buffer */
  unsigned char *_up;	/* saved _p when _p is doing ungetc data */
  int	_ur;		/* saved _r when _r is counting ungetc data */

  /* tricks to meet minimum requirements even when malloc() fails */
  unsigned char _ubuf[3];	/* guarantee an ungetc() buffer */
  unsigned char _nbuf[1];	/* guarantee a getc() buffer */

  /* separate buffer for fgetline() when line crosses buffer boundary */
  struct __sbuf _lb;	/* buffer for fgetline() */

  /* Unix stdio files get aligned to block boundaries on fseek() */
  int	_blksize;	/* stat.st_blksize (may be != _bf._size) */
  int	_offset;	/* current lseek offset */

  struct _reent *_data;
};

/*
 * struct _reent
 *
 * This structure contains *all* globals needed by the library.
 * It's raison d'etre is to facilitate threads by making all library routines
 * reentrant.  IE: All state information is contained here.
 */

struct _reent
{
  /* local copy of errno */
  int _errno;

  /* FILE is a big struct and may change over time.  To try to achieve binary
     compatibility with future versions, put stdin,stdout,stderr here.
     These are pointers into member __sf defined below.  */
  struct __sFILE *_stdin, *_stdout, *_stderr;

  int  _inc;			/* used by tmpnam */
  char _emergency[25];
 
  int _current_category;	/* used by setlocale */
  _CONST char *_current_locale;

  int __sdidinit;		/* 1 means stdio has been init'd */

  void _EXFUN((*__cleanup),(struct _reent *));

  /* used by mprec routines */
  struct _Bigint *_result;
  int _result_k;
  struct _Bigint *_p5s;
  struct _Bigint **_freelist;

  /* used by some fp conversion routines */
  int _cvtlen;			/* should be size_t */
  char *_cvtbuf;

  union
    {
      struct
        {
          unsigned int _rand_next;
          char * _strtok_last;
          char _asctime_buf[26];
          struct tm _localtime_buf;
          int _gamma_signgam;
        } _reent;
  /* Two next two fields were once used by malloc.  They are no longer
     used. They are used to preserve the space used before so as to
     allow addition of new reent fields and keep binary compatibility.   */ 
      struct
        {
#define _N_LISTS 30
          unsigned char * _nextf[_N_LISTS];
          unsigned int _nmalloc[_N_LISTS];
        } _unused;
    } _new;

  /* atexit stuff */
  struct _atexit *_atexit;	/* points to head of LIFO stack */
  struct _atexit _atexit0;	/* one guaranteed table, required by ANSI */

  /* signal info */
  void (**(_sig_func))(int);

  /* These are here last so that __sFILE can grow without changing the offsets
     of the above members (on the off chance that future binary compatibility
     would be broken otherwise).  */
  struct _glue __sglue;			/* root of glue chain */
  struct __sFILE __sf[3];		/* first three file descriptors */
};

#define _REENT_INIT(var) \
  { 0, &var.__sf[0], &var.__sf[1], &var.__sf[2], 0, "", 0, "C", \
    0, NULL, NULL, 0, NULL, NULL, 0, NULL, { {1, NULL, "", \
    { 0,0,0,0,0,0,0,0}, 0 } } }

/*
 * All references to struct _reent are via this pointer.
 * Internally, newlib routines that need to reference it should use _REENT.
 */

#ifndef __ATTRIBUTE_IMPURE_PTR__
#define __ATTRIBUTE_IMPURE_PTR__
#endif

extern struct _reent *_impure_ptr __ATTRIBUTE_IMPURE_PTR__;

/* #define _REENT_ONLY define this to get only reentrant routines */
extern int stdout_write(void*, const char*, int);
#ifndef _REENT_ONLY


//#define _REENT _impure_ptr

#define _REENT                                                               \
({                                                                           \
  struct _reent *_impure_ptr_tmp;                                            \
  if (os_malloc_completed)                                                   \
  {                                                                          \
    if (g_OS_TCB_Current->pTCBExtPtr == NULL)                                       \
    {                                                                        \
      _impure_ptr_tmp = (void *)drv_malloc (sizeof(struct _reent));          \
      memset ((void *)_impure_ptr_tmp, 0, sizeof(struct _reent));            \
      _impure_ptr_tmp->_stdin  = &(_impure_ptr_tmp->__sf[0]);                \
      _impure_ptr_tmp->_stdout = &(_impure_ptr_tmp->__sf[1]);                \
      _impure_ptr_tmp->_stderr = &(_impure_ptr_tmp->__sf[2]);                \
      _impure_ptr_tmp->_stdout->_write = (void *)stdout_write;               \
      _impure_ptr_tmp->_stdout->_data = _impure_ptr_tmp;                     \
      _impure_ptr_tmp->_current_locale = "C";                                \
      _impure_ptr_tmp->_new._reent._rand_next = 1;                           \
      g_OS_TCB_Current->pTCBExtPtr = (void *)_impure_ptr_tmp;                       \
    }                                                                        \
    _impure_ptr_tmp = g_OS_TCB_Current->pTCBExtPtr;                                 \
  }                                                                          \
  else                                                                       \
    _impure_ptr_tmp = _impure_ptr;                                           \
  _impure_ptr_tmp;                                                           \
})

#endif

/* --------- gcc/include/stdarg.h ----------- */
#if 1
#include <stdarg.h>
#else
typedef void *__gnuc_va_list;
typedef __gnuc_va_list va_list;

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG)                                           \
 (AP = ((__gnuc_va_list) __builtin_next_arg (LASTARG)))

/* This is for little-endian machines; small args are padded upward.  */
#define va_arg(AP, TYPE)                                                \
 (AP = (__gnuc_va_list) ((char *) (AP) + __va_rounded_size (TYPE)),     \
  *((TYPE *) (void *) ((char *) (AP) - __va_rounded_size (TYPE))))

#undef va_end
void va_end (__gnuc_va_list);           /* Defined in libgcc.a */
#define va_end(AP)      ((void)0)
#endif


/* ----------- stdio.h -----------*/

typedef _fpos_t fpos_t;
typedef struct __sFILE FILE;

#define __SLBF  0x0001          /* line buffered */
#define __SNBF  0x0002          /* unbuffered */
#define __SRD   0x0004          /* OK to read */
#define __SWR   0x0008          /* OK to write */
        /* RD and WR are never simultaneously asserted */
#define __SRW   0x0010          /* open for reading & writing */
#define __SEOF  0x0020          /* found EOF */
#define __SERR  0x0040          /* found error */
#define __SMBF  0x0080          /* _buf is from malloc */
#define __SAPP  0x0100          /* fdopen()ed in append mode - so must  write to end */
#define __SSTR  0x0200          /* this is an sprintf/snprintf string */
#define __SOPT  0x0400          /* do fseek() optimisation */
#define __SNPT  0x0800          /* do not do fseek() optimisation */
#define __SOFF  0x1000          /* set iff _offset is in fact correct */
#define __SMOD  0x2000          /* true => fgetline modified _p text */


#define BUFSIZ  1024
#define EOF     (-1)

extern int __mb_cur_max;
#define MB_CUR_MAX __mb_cur_max
#if 0
#define stdin   (_impure_ptr->_stdin)
#define stdout  (_impure_ptr->_stdout)
#define stderr  (_impure_ptr->_stderr)
#endif

#define _stdin_r(x)     ((x)->_stdin)
#define _stdout_r(x)    ((x)->_stdout)
#define _stderr_r(x)    ((x)->_stderr)

/*
 * The __sfoo macros are here so that we can
 * define function versions in the C library.
 */
#define __sgetc(p) (--(p)->_r < 0 ? __srget(p) : (int)(*(p)->_p++))

/*
 * This has been tuned to generate reasonable code on the vax using pcc
 */
#define __sputc(c, p) \
        (--(p)->_w < 0 ? \
                (p)->_w >= (p)->_lbfsize ? \
                        (*(p)->_p = (c)), *(p)->_p != '\n' ? \
                                (int)*(p)->_p++ : \
                                __swbuf('\n', p) : \
                        __swbuf((int)(c), p) : \
                (*(p)->_p = (c), (int)*(p)->_p++))

/* --------- gcc/include/limits.h ----------- */
/* Minimum and maximum values a `signed int' can hold.  */
#ifndef __INT_MAX__
#define __INT_MAX__ 2147483647
#endif
#undef INT_MIN
#define INT_MIN (-INT_MAX-1)
#undef INT_MAX
#define INT_MAX __INT_MAX__

#define __LONG_MAX__ 2147483647L
#undef LONG_MIN
#define LONG_MIN (-LONG_MAX-1)
#undef LONG_MAX
#define LONG_MAX __LONG_MAX__

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
#undef ULONG_MAX
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#define __sferror(p)    (((p)->_flags & __SERR) != 0)

int     _EXFUN(fflush, (FILE *));


/* ------- stdio/fvwrite.h  ------------ */
struct __siov {
        const  void *      iov_base;
        size_t  iov_len;
};
struct __suio {
        struct  __siov *uio_iov;
        int     uio_iovcnt;
        int     uio_resid;
};


extern int  __sfvwrite   (FILE *, struct __suio *)  ;
extern int  __swsetup   (FILE *)  ;

/* ------- stdio/local.h  ------------ */

extern int    _EXFUN(__svfscanf,(FILE *, _CONST char *,va_list));
extern FILE  *_EXFUN(__sfp,(struct _reent *));
extern int    _EXFUN(__sflags,(struct _reent *,_CONST char*, int*));
extern int    _EXFUN(__srefill,(FILE *));
extern int    _EXFUN(__sread,(void *, char *, int));
extern int    _EXFUN(__swrite,(void *, char const *, int));
extern fpos_t _EXFUN(__sseek,(void *, fpos_t, int));
extern int    _EXFUN(__sclose,(void *));
extern void   _EXFUN(__sinit,(struct _reent *));
extern void   _EXFUN(_cleanup_r,(struct _reent *));
extern void   _EXFUN(__smakebuf,(FILE *));
extern int    _EXFUN(_fwalk,(struct _reent *, int (*)(FILE *)));
struct _glue * _EXFUN(__sfmoreglue,(struct _reent *,int n));
extern int   _EXFUN(__srefill,(FILE *fp));

/* Called by the main entry point fns to ensure stdio has been initialized.  */

#define CHECK_INIT(fp) \
  do                                    \
    {                                   \
      if ((fp)->_data == 0)             \
        (fp)->_data = _REENT;           \
      if (!(fp)->_data->__sdidinit)     \
        __sinit ((fp)->_data);          \
    }                                   \
  while (0)

/* Return true iff the given FILE cannot be written now.  */

#define cantwrite(fp) \
  ((((fp)->_flags & __SWR) == 0 || (fp)->_bf._base == NULL) && \
   __swsetup(fp))

/* Test whether the given stdio file has an active ungetc buffer;
   release such a buffer, without restoring ordinary unread data.  */

#define HASUB(fp) ((fp)->_ub._base != NULL)
#define FREEUB(fp) { \
        if ((fp)->_ub._base != (fp)->_ubuf) \
                _free_r(fp->_data, (char *)(fp)->_ub._base); \
        (fp)->_ub._base = NULL; \
}

#define NDYNAMIC 4      /* add four more whenever necessary */

/* ------- locale.h  ------------ */
#define LC_ALL	    0
#define LC_COLLATE  1
#define LC_CTYPE    2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME     5

struct lconv
{
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;
  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;
};

#ifndef _REENT_ONLY
char *_EXFUN(setlocale,(int category, const char *locale));
struct lconv *_EXFUN(localeconv,(void));
#endif

struct _reent;
char *_EXFUN(_setlocale_r,(struct _reent *, int category, const char *locale));
struct lconv *_EXFUN(_localeconv_r,(struct _reent *));

/* ------- stdio/floatio.h  ------------ */
/*
 * Floating point scanf/printf (input/output) definitions.
 */

/* 11-bit exponent (VAX G floating point) is 308 decimal digits */
#define MAXEXP          308
/* 128 bit fraction takes up 39 decimal digits; max reasonable precision */
#define MAXFRACT        39


/* ------- gcc/include/float.h  ------------ */
   /* Number of decimal digits of precision in a double */
#undef DBL_DIG
#define DBL_DIG 15
   /* Addition rounds to 0: zero, 1: nearest, 2: +inf, 3: -inf, -1: unknown */
#undef FLT_ROUNDS
#define FLT_ROUNDS 1
   /* Maximum int x such that 10**x is a representable double */
#undef DBL_MAX_10_EXP
#define DBL_MAX_10_EXP 308
   /* Maximum int x such that FLT_RADIX**(x-1) is a representable double */
#undef DBL_MAX_EXP
#define DBL_MAX_EXP 1024
   /* Radix of exponent representation */
#undef FLT_RADIX
#define FLT_RADIX 2

/* ------- ieeefp.h ------------ */

#define __IEEE_LITTLE_ENDIAN

#ifdef __IEEE_LITTLE_ENDIAN

typedef union 
{
  double value;
  struct 
  {
#ifdef __SMALL_BITFIELDS
    unsigned int fraction3:16;
    unsigned int fraction2:16;
    unsigned int fraction1:16;
    unsigned int fraction0: 4;
#else
    unsigned int fraction1:32;
    unsigned int fraction0:20;
#endif
    unsigned int exponent :11;
    unsigned int sign     : 1;
  } number;
  struct 
  {
#ifdef __SMALL_BITFIELDS
    unsigned int function3:16;
    unsigned int function2:16;
    unsigned int function1:16;
    unsigned int function0:3;
#else
    unsigned int function1:32;
    unsigned int function0:19;
#endif
    unsigned int quiet:1;
    unsigned int exponent: 11;
    unsigned int sign : 1;
  } nan;
  struct 
  {
    unsigned long lsw;
    unsigned long msw;
  } parts;

  long aslong[2];

} __ieee_double_shape_type;

#endif
/* ------- sys/stat.h ------------ */

struct  stat {
        short st_dev;
        short st_ino;
        unsigned short st_mode;
        short st_nlink;
        short st_uid;
        short st_gid;
        short st_rdev;
        short st_align_for_word32;
        long  st_size;
        long  st_atime;
        long  st_mtime;
        long  st_ctime;
        long  st_blksize;
};

#define S_IFMT  0xF000  /* file type mask */
#define S_IFDIR 0x4000  /* directory */
#define S_IFIFO 0x1000  /* FIFO special */
#define S_IFCHR 0x2000  /* character special */
#define S_IFBLK 0x3000  /* block special */
#define S_IFREG 0x8000  /* or just 0x0000, regular */
#define S_IREAD 0x0100  /* owner may read */
#define S_IWRITE 0x0080 /* owner may write */
#define S_IEXEC 0x0040  /* owner may execute <directory search> */

#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)

/* ------- sys/errno.h  ------------ */

#define	EPERM 1		/* Not super-user */
#define	ENOENT 2	/* No such file or directory */
#define	ESRCH 3		/* No such process */
#define	EINTR 4		/* Interrupted system call */
#define	EIO 5		/* I/O error */
#define	ENXIO 6		/* No such device or address */
#define	E2BIG 7		/* Arg list too long */
#define	ENOEXEC 8	/* Exec format error */
#define	EBADF 9		/* Bad file number */
#define	ECHILD 10	/* No children */
#define	EAGAIN 11	/* No more processes */
#define	ENOMEM 12	/* Not enough core */
#define	EACCES 13	/* Permission denied */
#define	EFAULT 14	/* Bad address */
#define	ENOTBLK 15	/* Block device required */
#define	EBUSY 16	/* Mount device busy */
#define	EEXIST 17	/* File exists */
#define	EXDEV 18	/* Cross-device link */
#define	ENODEV 19	/* No such device */
#define	ENOTDIR 20	/* Not a directory */
#define	EISDIR 21	/* Is a directory */
#define	EINVAL 22	/* Invalid argument */
#define	ENFILE 23	/* Too many open files in system */
#define	EMFILE 24	/* Too many open files */
#define	ENOTTY 25	/* Not a typewriter */
#define	ETXTBSY 26	/* Text file busy */
#define	EFBIG 27	/* File too large */
#define	ENOSPC 28	/* No space left on device */
#define	ESPIPE 29	/* Illegal seek */
#define	EROFS 30	/* Read only file system */
#define	EMLINK 31	/* Too many links */
#define	EPIPE 32	/* Broken pipe */
#define	EDOM 33		/* Math arg out of domain of func */
#define	ERANGE 34	/* Math result not representable */
#define	ENOMSG 35	/* No message of desired type */
#define	EIDRM 36	/* Identifier removed */
#define	ECHRNG 37	/* Channel number out of range */
#define	EL2NSYNC 38	/* Level 2 not synchronized */
#define	EL3HLT 39	/* Level 3 halted */
#define	EL3RST 40	/* Level 3 reset */
#define	ELNRNG 41	/* Link number out of range */
#define	EUNATCH 42	/* Protocol driver not attached */
#define	ENOCSI 43	/* No CSI structure available */
#define	EL2HLT 44	/* Level 2 halted */
#define	EDEADLK 45	/* Deadlock condition */
#define	ENOLCK 46	/* No record locks available */
#define EBADE 50	/* Invalid exchange */
#define EBADR 51	/* Invalid request descriptor */
#define EXFULL 52	/* Exchange full */
#define ENOANO 53	/* No anode */
#define EBADRQC 54	/* Invalid request code */
#define EBADSLT 55	/* Invalid slot */
#define EDEADLOCK 56	/* File locking deadlock error */
#define EBFONT 57	/* Bad font file fmt */
#define ENOSTR 60	/* Device not a stream */
#define ENODATA 61	/* No data (for no delay io) */
#define ETIME 62	/* Timer expired */
#define ENOSR 63	/* Out of streams resources */
#define ENONET 64	/* Machine is not on the network */
#define ENOPKG 65	/* Package not installed */
#define EREMOTE 66	/* The object is remote */
#define ENOLINK 67	/* The link has been severed */
#define EADV 68		/* Advertise error */
#define ESRMNT 69	/* Srmount error */
#define	ECOMM 70	/* Communication error on send */
#define EPROTO 71	/* Protocol error */
#define	EMULTIHOP 74	/* Multihop attempted */
#define	ELBIN 75	/* Inode is remote (not really error) */
#define	EDOTDOT 76	/* Cross mount point (not really error) */
#define EBADMSG 77	/* Trying to read unreadable message */
#define ENOTUNIQ 80	/* Given log. name not unique */
#define EBADFD 81	/* f.d. invalid for this operation */
#define EREMCHG 82	/* Remote address changed */
#define ELIBACC 83	/* Can't access a needed shared lib */
#define ELIBBAD 84	/* Accessing a corrupted shared lib */
#define ELIBSCN 85	/* .lib section in a.out corrupted */
#define ELIBMAX 86	/* Attempting to link in too many libs */
#define ELIBEXEC 87	/* Attempting to exec a shared library */
#define ENOSYS 88	/* Function not implemented */
#define ENMFILE 89      /* No more files */
#define ENOTEMPTY 90	/* Directory not empty */
#define ENAMETOOLONG 91	/* File or path name too long */
#define ELOOP 92	/* Too many symbolic links */
#define EOPNOTSUPP 95	/* Operation not supported on transport endpoint */
#define EPFNOSUPPORT 96 /* Protocol family not supported */
#define ECONNRESET 104  /* Connection reset by peer */
#define ENOBUFS 105	/* No buffer space available */
#define EAFNOSUPPORT 106
#define EPROTOTYPE 107
#define ENOTSOCK 108
#define ENOPROTOOPT 109
#define ESHUTDOWN 110
#define ECONNREFUSED 111	/* Connection refused */
#define EADDRINUSE 112		/* Address already in use */
#define ECONNABORTED 113	/* Connection aborted */
#define ENETUNREACH 114
#define ENETDOWN 115
#define ETIMEDOUT 116
#define EHOSTDOWN 117
#define EHOSTUNREACH 118
#define EINPROGRESS 119
#define EALREADY 120
#define EDESTADDRREQ 121
#define EMSGSIZE 122
#define EPROTONOSUPPORT 123
#define ESOCKTNOSUPPORT 124
#define EADDRNOTAVAIL 125
#define ENETRESET 126
#define EISCONN 127
#define ENOTCONN 128
#define ETOOMANYREFS 129
#define EPROCLIM 130
#define EUSERS 131
#define EDQUOT 132
#define ESTALE 133
#define ENOTSUP 134

/* -------------------------------------- */
#ifndef DEF_LIBC_VALUE
extern FILE *stderr;
#endif
