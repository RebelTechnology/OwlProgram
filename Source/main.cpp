#include <inttypes.h>
#include "SharedMemory.h"
#include "owlcontrol.h"
#include "stm32f4xx.h"

#include <string.h>
#include "myalloc.h"

__attribute__ ((section (".sharedram")))
volatile SharedMemory smem;

extern void setup();
extern void processBlock();

extern char _sbss[];
extern char _ebss[];

extern "C" void __libc_init_array();
// extern void __init_registers();
// extern void __copy_rom_sections_to_ram(void);
// extern void __call_static_initializers(void);
// extern void __init_user();

#define DEBUG_DWT
extern "C"{
int _exit() {
  return 0;
}
}
#include <malloc.h>
extern char *heap_end;
extern int allocated_mem;

#define BANK1_SRAM3 0x68000000
int main(void){
  /* from startup.s Reset_Handler:
     zero-fill _ebss
     Call static constructors:   bl __libc_init_array
     call C++ static initializers?
  */
    memset(_sbss, 0, (_ebss-_sbss));
    __libc_init_array(); // caused reset when .data had been strippped

    InitMem((char*)BANK1_SRAM3, 1024*1024);

  if(smem.checksum != sizeof(smem)){
    // problem!
    smem.status = AUDIO_ERROR_STATUS;
    smem.error = CHECKSUM_ERROR_STATUS;
    // smem.exitProgram();
    // return -1;
  }

#ifdef DEBUG_DWT
  volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004; //address of the register
  volatile unsigned int *DWT_CONTROL = (volatile unsigned int *)0xE0001000; //address of the register
  volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC; //address of the register
  *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
  *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
  // if(!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
  //   CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  //   DWT->CYCCNT = 0;
  //   DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  // }
#endif /* DEBUG_DWT */

  setup();
  smem.heap_bytes_used = allocated_mem;
  for(;;){
    if(smem.status == AUDIO_READY_STATUS){
#ifdef DEBUG_DWT
      *DWT_CYCCNT = 0; // reset the counter
      // DWT->CYCCNT = 0; // reset the counter
#endif

      processBlock();
#ifdef DEBUG_DWT
      smem.cycles_per_block = *DWT_CYCCNT;
      // dwt_count = DWT->CYCCNT;
#endif /* DEBUG_DWT */

      if(smem.status == AUDIO_EXIT_STATUS)
    	return 0;
      smem.status = AUDIO_PROCESSED_STATUS; // always check status before changing it
    }
    if(smem.status == AUDIO_EXIT_STATUS)
      return 0;
  }
  return -1;
}
