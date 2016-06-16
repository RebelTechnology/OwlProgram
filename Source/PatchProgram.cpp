#include "ProgramVector.h"
#include "ServiceCall.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "message.h"
#include "StompBox.h"
#include "patch.h"
#include "main.h"
#include "heap.h"

PatchProcessor processor;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(patch == NULL)
    error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  if(getProgramVector()->registerPatch != NULL)
    getProgramVector()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch);
}

SampleBuffer* samples;
void setup(ProgramVector* pv){
#ifdef DEBUG_MEM
#ifdef ARM_CORTEX
  size_t before = xPortGetFreeHeapSize();
#endif
#endif
#include "patch.cpp"
#ifdef DEBUG_MEM
  // todo xPortGetFreeHeapSize() before and after
  // extern uint32_t total_heap_used;
  // pv->heap_bytes_used = total_heap_used;
#ifdef ARM_CORTEX
  getProgramVector()->heap_bytes_used = before - xPortGetFreeHeapSize();
#endif
#endif
  // samples = new SampleBuffer(getBlockSize());
  samples = new SampleBuffer();
}

void processBlock(ProgramVector* pv){
  samples->split(pv->audio_input, pv->audio_blocksize);
  processor.setParameterValues(pv->parameters);
  processor.patch->processAudio(*samples);
  samples->comb(pv->audio_output);
}
