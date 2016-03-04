#include "ProgramVector.h"
#include "ServiceCall.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "message.h"
#include "StompBox.h"
#include "patch.h"
#include "main.h"
#include "heap.h"

#if !defined PATCH_ALLOCATE_STACK && !defined PATCH_ALLOCATE_HEAP
#define PATCH_ALLOCATE_DYNAMIC
#define MAX_STACK_PATCH_SIZE (1024*8)
#endif

PatchProcessor processor;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  ASSERT(patch != NULL, "Memory allocation failed");
  if(getProgramVector()->registerPatch != NULL)
    getProgramVector()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch);
}

#include <alloca.h>
#include <new>

#ifdef PATCH_ALLOCATE_HEAP
#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)
#elif defined PATCH_ALLOCATE_STACK
#define REGISTER_PATCH(T, STR, IN, OUT) do{static T t; registerPatch(STR, IN, OUT, &t); }while(0)
#elif defined PATCH_ALLOCATE_DYNAMIC
#define REGISTER_PATCH(T, STR, IN, OUT) do{T* t; if(sizeof(T) > MAX_STACK_PATCH_SIZE){ t = new T(); }else{ t = new((void*)alloca(sizeof(T)))T(); } registerPatch(STR, IN, OUT, t); }while(0)
#endif

void run(){
  ProgramVector* pv = getProgramVector();
#ifdef DEBUG_DWT
  volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004; //address of the
#endif

#ifdef DEBUG_MEM
  size_t before = xPortGetFreeHeapSize();
#endif

#include "patch.cpp"

#ifdef DEBUG_MEM
  // todo xPortGetFreeHeapSize() before and after
  // extern uint32_t total_heap_used;
  // pv->heap_bytes_used = total_heap_used;
  pv->heap_bytes_used = before - xPortGetFreeHeapSize();
#endif

  SampleBuffer buffer;
  for(;;){
    pv->programReady();
#ifdef DEBUG_DWT
    *DWT_CYCCNT = 0; // reset the counter
#endif

    buffer.split(pv->audio_input, pv->audio_blocksize);
    processor.setParameterValues(pv->parameters);
    processor.patch->processAudio(buffer);
    buffer.comb(pv->audio_output);

#ifdef DEBUG_DWT
    pv->cycles_per_block = *DWT_CYCCNT;
#endif
  }
}
