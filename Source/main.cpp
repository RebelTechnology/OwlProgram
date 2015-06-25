#include <string.h>
#include <inttypes.h>
#include "ProgramVector.h"
#include "device.h"

#ifdef DEBUG_MEM
#include <malloc.h>
#endif /* DEBUG_MEM */

#ifdef STARTUP_CODE
extern char _sbss[];
extern char _ebss[];
extern char _sidata[];
extern char _sdata[];
extern char _edata[];
extern "C" void __libc_init_array();
#endif /* STARTUP_CODE */

extern void setup();
extern void processBlock();
ProgramVector* getProgramVector() { return ((ProgramVector*)((uint32_t)0x40024000)); }

// extern "C" {
//   ProgramVector pv;
//   const void* ProgramVectorPointer = &pv;
// }

#define BANK1_SRAM3 0x68000000
int main(void){
#ifdef STARTUP_CODE
  memcpy(_sidata, _sdata, _sdata-_edata); // Copy the data segment initializers
  memset(_sbss, 0, _ebss-_sbss); // zero fill the BSS segment
  __libc_init_array(); // Call static constructors
#endif /* STARTUP_CODE */

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

  if(getProgramVector()->checksum != sizeof(ProgramVector)){
    getProgramVector()->error = CHECKSUM_ERROR_STATUS;
    getProgramVector()->message = (char*)"ProgramVector checksum error";
    getProgramVector()->programStatus(AUDIO_ERROR_STATUS);
    return -1;
  }

  setup();

#ifdef DEBUG_MEM
  struct mallinfo minfo = mallinfo();
  // getProgramVector()->heap_bytes_used = minfo.uordblks;
  getProgramVector()->heap_bytes_used = minfo.arena;
#endif /* DEBUG_MEM */

  for(;;){
    getProgramVector()->programReady();
#ifdef DEBUG_DWT
      *DWT_CYCCNT = 0; // reset the counter
#endif /* DEBUG_DWT */
      processBlock();
#ifdef DEBUG_DWT
      getProgramVector()->cycles_per_block = *DWT_CYCCNT;
#endif /* DEBUG_DWT */
  }
}
