#include "ProgramVector.h"
#include "ServiceCall.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "message.h"
#include "StompBox.h"
#include "patch.h"
#include "main.h"

#ifndef PATCH_ALLOCATE_STACK
#define PATCH_ALLOCATE_HEAP
#endif
//#define PATCH_ALLOCATE_STACK

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

#ifdef PATCH_ALLOCATE_HEAP
#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)
#elif defined PATCH_ALLOCATE_STACK
#define REGISTER_PATCH(T, STR, IN, OUT) do{static T t; registerPatch(STR, IN, OUT, &t); }while(0)
#endif

void setup(){
#include "patch.cpp"
}

void processBlock(){
  SampleBuffer buffer;
  buffer.split(getProgramVector()->audio_input, getProgramVector()->audio_blocksize);
  processor.setParameterValues(getProgramVector()->parameters);
  processor.patch->processAudio(buffer);
  buffer.comb(getProgramVector()->audio_output);
}
