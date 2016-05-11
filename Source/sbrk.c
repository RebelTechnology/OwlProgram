#include <errno.h>
#include <inttypes.h>
#include <sys/stat.h>
extern int errno;
#include "message.h"

// #define DOUBLE_HEAP
#define FAST_HEAP_SIZE (32*1024)
uint32_t total_heap_used = 0;
caddr_t _sbrk(int incr)
{
  /* while(incr & 0x03) */
  /*   incr++; // word align */
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
  /* if(incr < 1*1024 && fast_heap_end+incr <= fast_eheap){ */
  if(heap_end == &_heap && fast_heap_end+incr <= fast_eheap){
  /* if(fast_heap_end+incr <= fast_eheap){ */
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
