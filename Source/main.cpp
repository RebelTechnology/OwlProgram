#include <inttypes.h>
#include "SharedMemory.h"
#include "owlcontrol.h"

#include <string.h>

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

int main(void){
  /* todo from startup.s Reset_Handler:
     zero-fill _ebss
     Call static constructors:   bl __libc_init_array
     call C++ static initializers?
  */
    memset(_sbss, 0, (_ebss-_sbss));
    // __libc_init_array(); // causes reset
    
  if(smem.checksum != sizeof(smem)){
    // problem!
    smem.status = AUDIO_ERROR_STATUS;
    smem.error = CHECKSUM_ERROR_STATUS;
    // smem.exitProgram();
    // return -1;
  }
  setup();
  for(;;){
    if(smem.status == AUDIO_READY_STATUS){
  // static uint8_t counter = 0;
  // switch(counter++ & 0xff){
  // case 0x00:
  //   setLed(RED);
  //   break;
  // case 0x40:
  //   setLed(GREEN);
  //   break;
  // case 0x80:
  //   setLed(RED);
  //   break;
  // case 0xc0:
  //   setLed(GREEN);
  //   break;
  // }
      processBlock();
      if(smem.status == AUDIO_EXIT_STATUS)
	// smem.exitProgram();
    	return 0;
      smem.status = AUDIO_PROCESSED_STATUS; // always check status before changing it
    }
    if(smem.status == AUDIO_EXIT_STATUS)
	// smem.exitProgram();
      return 0;
  }
	// smem.exitProgram();
  // return -1;
}
