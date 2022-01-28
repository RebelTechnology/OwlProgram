#ifndef __heap_h
#define __heap_h

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define configUSE_MALLOC_FAILED_HOOK    1
#define portBYTE_ALIGNMENT		8
#if portBYTE_ALIGNMENT == 8
#define portBYTE_ALIGNMENT_MASK ( 0x0007 )
#endif
#define configASSERT( x )

   typedef struct HeapRegion
   {
     uint8_t *pucStartAddress;
     size_t xSizeInBytes;
   } HeapRegion_t;

   typedef long BaseType_t;

   void *pvPortMalloc( size_t xWantedSize );
   void vPortFree( void *pv );
   size_t xPortGetFreeHeapSize( void );
   size_t xPortGetMinimumEverFreeHeapSize( void );
   void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions );
   int vPortGetSizeBlock (void *pv);

#ifdef __cplusplus
}
#endif

#endif /* __heap_h */
