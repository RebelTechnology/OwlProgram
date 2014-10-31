#include <inttypes.h>
#include "owlcontrol.h"
#include "SharedMemory.h"

__attribute__ ((section (".sharedram")))
volatile SharedMemory smem;

extern void processBlock();

int main(void){
  setLed(RED);

  if(smem.checksum != sizeof(smem)){
    // problem!
    smem.status = AUDIO_ERROR_STATUS;
    smem.error = CHECKSUM_ERROR_STATUS;
  }

  for(;;){
    if(smem.status == AUDIO_READY_STATUS){
      processBlock();
      if(smem.status == AUDIO_EXIT_STATUS)
	return 0;
      smem.status = AUDIO_PROCESSED_STATUS; // always check status before changing it
    }
    if(smem.status == AUDIO_EXIT_STATUS)
      return 0;
  }
  return -1;
}
