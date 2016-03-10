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
  ASSERT(patch != NULL, "Memory allocation failed");
  if(getProgramVector()->registerPatch != NULL)
    getProgramVector()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch);
}

SampleBuffer* buffer;
ProgramVector* pv;
void setup(){
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
  // buffer = new SampleBuffer(getBlockSize());
  buffer = new SampleBuffer();
  pv = getProgramVector();
}

void processBlock(){
  buffer->split(pv->audio_input, pv->audio_blocksize);
  processor.setParameterValues(pv->parameters);
  processor.patch->processAudio(*buffer);
  buffer->comb(pv->audio_output);
}
