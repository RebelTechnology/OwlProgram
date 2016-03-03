/***********************************************************************
 * $Id:: libnosys_gnu.c 3304 2010-04-20 21:04:01Z usb10131             $
 *
 * Project: Libnosys function for GNU c compiler
 *
 * Description:
 *     Definitions for OS interface, stub function required by newlibc
 * used by Codesourcery GNU compiler.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
#if defined(__GNUC__)

#include <errno.h>
#include <sys/times.h>
#include <sys/stat.h>

/* errno definition */
#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

int _close(int file)
{
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}
int _exit()
{
  return 0;
}

int _fork()
{
  errno = EAGAIN;
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _getpid()
{
  return 1;
}

int _kill(int pid, int sig)
{
  errno = EINVAL;
  return(-1);
}

int isatty(int fildes)
{
  return 1;
}
int _isatty(int fildes)
{
  return 1;
}

int _link(char *old, char *new)
{
  errno = EMLINK;
  return -1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _open(const char *name, int flags, int mode)
{
  return -1;
}

int _read(int file, char *ptr, int len)
{
  return 0;
}

#define DOUBLE_HEAP
#define FAST_HEAP_SIZE (32*1024)
uint32_t total_heap_used = 0;
void error(int8_t code, const char* reason);
caddr_t _sbrk(int incr)
{
  char *prev_heap_end;
  extern char _heap, _eheap; /* Defined by the linker */
  static char *heap_end = &_heap;
#ifdef DOUBLE_HEAP
  if(incr < 0)
    // newlib nano malloc doesn't deallocate
    error(0x60, "Deallocating heap not allowed");
  static char fast_heap[FAST_HEAP_SIZE] __attribute__ ((section (".ccmdata")));
  static char *fast_heap_end = fast_heap;
  static char* fast_eheap = fast_heap+FAST_HEAP_SIZE;
  if(incr < 4*1024 && fast_heap_end+incr < fast_eheap){
    prev_heap_end = fast_heap_end;
    fast_heap_end += incr;
    total_heap_used += incr;
    return (caddr_t) prev_heap_end;
  }
#endif
  prev_heap_end = heap_end;
  if (heap_end+incr > &_eheap)
  {
    errno = ENOMEM;
    error(0x60, "Heap overflow");
    return (caddr_t)0;
  }
  heap_end += incr;
  total_heap_used += incr;
  return (caddr_t) prev_heap_end;
}

int _stat(char *file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}
int _times(struct tms *buf)
{
  return -1;
}

int _unlink(char *name)
{
  errno = ENOENT;
  return -1;
}

int _wait(int *status)
{
  errno = ECHILD;
  return -1;
}

int _write(int file, char *ptr, int len)
{
  return 0;
}

#endif /*__GNUC__*/
