#include <inttypes.h>
#include "SharedMemory.h"

__attribute__ ((section (".sharedram")))
volatile SharedMemory smem;

extern void setup();
extern void processBlock();

int main(void){
  if(smem.checksum != sizeof(smem)){
    // problem!
    smem.status = AUDIO_ERROR_STATUS;
    smem.error = CHECKSUM_ERROR_STATUS;
    return -1;
  }
  setup();
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
