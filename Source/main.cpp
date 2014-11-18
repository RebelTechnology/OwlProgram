#include <inttypes.h>
#include "SharedMemory.h"
#include "owlcontrol.h"

__attribute__ ((section (".sharedram")))
volatile SharedMemory smem;

extern void setup();
extern void processBlock();

int main(void){
  if(smem.checksum != sizeof(smem)){
    // problem!
    smem.status = AUDIO_ERROR_STATUS;
    smem.error = CHECKSUM_ERROR_STATUS;
    smem.exitProgram();
    // return -1;
  }
  setup();
  for(;;){
    if(smem.status == AUDIO_READY_STATUS){
  // static uint8_t counter = 0;
  // switch(counter++ & 0xff){
  // case 0x00:
  //   setLed(GREEN);
  //   break;
  // case 0x40:
  //   setLed(NONE);
  //   break;
  // case 0x80:
  //   setLed(GREEN);
  //   break;
  // case 0xc0:
  //   setLed(NONE);
  //   break;
  // }
      processBlock();
      if(smem.status == AUDIO_EXIT_STATUS)
	smem.exitProgram();
    	// return 0;
      smem.status = AUDIO_PROCESSED_STATUS; // always check status before changing it
    }
    if(smem.status == AUDIO_EXIT_STATUS)
	smem.exitProgram();
      // return 0;
  }
	smem.exitProgram();
  // smem.exitProgram();
  // return -1;
}
