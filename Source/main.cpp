#include <inttypes.h>
#include "SharedMemory.h"
#include "owlcontrol.h"
#include "stm32f4xx.h"
// #include <malloc.h>
#include <string.h>
// #include "myalloc.h"

// __attribute__ ((section (".sharedram")))
// volatile SharedMemory smem;

extern void setup();
extern void processBlock();

extern char _sbss[];
extern char _ebss[];
extern char _sidata[];
extern char _sdata[];
extern char _edata[];

extern "C" void __libc_init_array();
// extern void __init_registers();
// extern void __copy_rom_sections_to_ram(void);
// extern "C" void __init_cpp(void);
// extern void __init_user();

// /* Copy the data segment initializers from flash to SRAM */  
//   movs  r1, #0
//   b  LoopCopyDataInit
// CopyDataInit:
//   ldr  r3, =_sidata
//   ldr  r3, [r3, r1]
//   str  r3, [r0, r1]
//   adds  r1, r1, #4    
// LoopCopyDataInit:
//   ldr  r0, =_sdata
//   ldr  r3, =_edata
//   adds  r2, r0, r1
//   cmp  r2, r3
//   bcc  CopyDataInit

#define DEBUG_DWT
/*
  extern "C"{
  int _exit() {
  return 0;
  }
  }
*/
extern char *heap_end;
// extern int allocated_mem;

#define BANK1_SRAM3 0x68000000
int main(void){
  /* from startup.s Reset_Handler:
     zero-fill _ebss
     Call static constructors:   bl __libc_init_array
     call C++ static initializers?
  */
  // memset(_sbss, 0, _ebss-_sbss); // zero fill the BSS segment
  // // memcpy(_sidata, _sdata, _sdata-_edata); // Copy the data segment initializers
  // __libc_init_array(); // caused reset when .data had been strippped

  // InitMem((char*)BANK1_SRAM3, 1024*1024);

  if(getSharedMemory()->checksum != sizeof(SharedMemory)){
    // problem!
    getSharedMemory()->status = AUDIO_ERROR_STATUS;
    getSharedMemory()->error = CHECKSUM_ERROR_STATUS;
    // getSharedMemory()->exitProgram();
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

  // struct mallinfo minfo = mallinfo();
  // getSharedMemory()->heap_bytes_used = minfo.uordblks;
  /* ^ causes OwlWare.sysex to trip to:
   USART6_IRQHandler () at ./Source/startup.s:142
   142	  b  Infinite_Loop */

  // getSharedMemory()->heap_bytes_used = allocated_mem;
  for(;;){
    if(getSharedMemory()->status == AUDIO_READY_STATUS){
#ifdef DEBUG_DWT
      *DWT_CYCCNT = 0; // reset the counter
      // DWT->CYCCNT = 0; // reset the counter
#endif

      processBlock();
#ifdef DEBUG_DWT
      getSharedMemory()->cycles_per_block = *DWT_CYCCNT;
      // dwt_count = DWT->CYCCNT;
#endif /* DEBUG_DWT */

      if(getSharedMemory()->status == AUDIO_EXIT_STATUS)
    	break;
      getSharedMemory()->status = AUDIO_PROCESSED_STATUS; // always check status before changing it
    }
    if(getSharedMemory()->status == AUDIO_EXIT_STATUS)
      break;
  }
  for(;;);
}
