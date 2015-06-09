#include <string.h>
#include <inttypes.h>
#include "ProgramVector.h"
#include "stm32f4xx.h"
#ifdef DEBUG_MEM
#include <malloc.h>
#endif /* DEBUG_MEM */

extern "C" {
  void program_setup();
  void program_processBlock();
  void program_run(void);
}

void program_run(void){
  if(getProgramVector()->checksum != sizeof(ProgramVector)){
    // problem!
    // getProgramVector()->status = AUDIO_ERROR_STATUS;
    getProgramVector()->error = CHECKSUM_ERROR_STATUS;
    getProgramVector()->programStatus(AUDIO_ERROR_STATUS);
    // getProgramVector()->exitProgram();
    // return -1;
  }

  program_setup();

#ifdef DEBUG_MEM
  struct mallinfo minfo = mallinfo(); // never returns when -O1 or -O2 ?
  /* ^ may cause OwlWare.sysex to trip to:
   USART6_IRQHandler () at ./Source/startup.s:142
   142	  b  Infinite_Loop */
  // getProgramVector()->heap_bytes_used = minfo.uordblks;
  getProgramVector()->heap_bytes_used = minfo.arena;
#endif /* DEBUG_MEM */

  for(;;){
    getProgramVector()->programReady();
    // if(getProgramVector()->status == AUDIO_READY_STATUS){
      program_processBlock();
  }
}

#include "ProgramVector.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "basicmaths.h"
#include "solopatch.h"
#include "owlcontrol.h"

PatchProcessor processor;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(getProgramVector()->registerPatch != NULL)
    getProgramVector()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch);
}

void program_setup(){
#include "solopatch.cpp"
}

SampleBuffer buffer;

void program_processBlock(){
  buffer.split(getProgramVector()->audio_input, getProgramVector()->audio_blocksize);
  processor.setParameterValues(getProgramVector()->parameters);
  processor.patch->processAudio(buffer);
  buffer.comb(getProgramVector()->audio_output);
}
