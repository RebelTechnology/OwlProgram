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

/* Defined by the linker */
  extern char _fastheap, _fasteheap; // internal RAM dedicated to heap
  extern char _eprogram, _eram; // remaining program space
  extern char _heap, _eheap; // external memory
  const HeapRegion_t xHeapRegions[] = {
    { ( uint8_t * )&_fastheap, (size_t)(&_fasteheap - &_fastheap) },
    { ( uint8_t * )&_eprogram, (size_t)(&_eram - &_eprogram) },
    { ( uint8_t * )&_heap, (size_t)(&_eheap - &_heap) },
    { NULL, 0 } /* Terminates the array. */
  };
  vPortDefineHeapRegions( xHeapRegions ); // call before static initialisers to allow heap use

#ifdef STARTUP_CODE
  __libc_init_array(); // Call static constructors
#endif /* STARTUP_CODE */

  ProgramVector* pv = getProgramVector();
  if(pv->checksum >= PROGRAM_VECTOR_CHECKSUM_V12){
    // set event callbacks
    pv->buttonChangedCallback = onButtonChanged;
    pv->encoderChangedCallback = onEncoderChanged;
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
