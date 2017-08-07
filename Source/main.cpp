#include <string.h>
#include <stdint.h>
#include "ProgramVector.h"
#include "ServiceCall.h"
#include "device.h"
#include "main.h"
#include "heap.h"
#include "message.h"

#ifdef STARTUP_CODE
extern char _sbss[];
extern char _ebss[];
extern char _sidata[];
extern char _sdata[];
extern char _edata[];
extern "C" void __libc_init_array();
#endif /* STARTUP_CODE */

ProgramVector programVector __attribute__ ((section (".pv")));
// ProgramVector* getProgramVector() { return &programVector; }

extern "C" {
  void vApplicationMallocFailedHook( void ){
    error(0x60, "Memory overflow");
  }
}

int main(void){
 #ifdef STARTUP_CODE
  memcpy(_sidata, _sdata, _sdata-_edata); // Copy the data segment initializers
  memset(_sbss, 0, _ebss-_sbss); // zero fill the BSS segment
#endif /* STARTUP_CODE */

// #ifdef STARTUP_CODE
//   __libc_init_array(); // Call static constructors
// #endif /* STARTUP_CODE */

  ProgramVector* pv = getProgramVector();
  HeapRegion_t regions[5];
  if(pv->checksum >= PROGRAM_VECTOR_CHECKSUM_V13 && pv->heapLocations != NULL){
    int cnt = 0;
    extern char _eprogram, _eram; // remaining program space
    MemorySegment* seg = pv->heapLocations;
    while(seg != NULL && seg->location != NULL && cnt < 5){
      regions[cnt++] = { seg->location, seg->size };
      seg++;
    }
    regions[cnt] = {NULL, 0}; // terminate the array
  }else{
    /* Defined by the linker */
    extern char _fastheap, _fasteheap; // internal RAM dedicated to heap
    extern char _eprogram, _eram; // remaining program space
    extern char _heap, _eheap; // external memory
    int cnt = 0;
    regions[cnt++] = { (uint8_t*)&_fastheap, (size_t)(&_fasteheap - &_fastheap) };
    regions[cnt++] = { (uint8_t*)&_eprogram, (size_t)(&_eram - &_eprogram) };
    regions[cnt++] = { (uint8_t*)&_heap, (size_t)(&_eheap - &_heap) };
    regions[cnt] = {NULL, 0}; // terminate the array
  }
  vPortDefineHeapRegions(regions); // call before static initialisers to allow heap use

#ifdef STARTUP_CODE
  __libc_init_array(); // Call static constructors
#endif /* STARTUP_CODE */

  if(pv->checksum >= PROGRAM_VECTOR_CHECKSUM_V12){
    // set event callbacks
    pv->buttonChangedCallback = onButtonChanged;
  }else if(pv->checksum >= PROGRAM_VECTOR_CHECKSUM_V11){
    // no event callbacks
  }else{
    error(CHECKSUM_ERROR_STATUS, "ProgramVector checksum error");
    return -1;
  }
  if(pv->audio_blocksize <= 0 || pv->audio_blocksize > AUDIO_MAX_BLOCK_SIZE){     
    error(CONFIGURATION_ERROR_STATUS, "Invalid blocksize");
    return -1;
  }

  setup(pv);

  for(;;){
    pv->programReady();
    processBlock(pv);
  }
}
