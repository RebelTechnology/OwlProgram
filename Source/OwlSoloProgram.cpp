#include "SharedMemory.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "basicmaths.h"
#include "solopatch.h"

PatchProcessor processor;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(smem.registerPatch != NULL)
    smem.registerPatch(name, inputs, outputs);
  processor.setPatch(patch);
}

void setup(){
#include "solopatch.cpp"
}

SampleBuffer buffer;

void processBlock(){
  buffer.split(smem.audio_input, smem.audio_blocksize);
  processor.setParameterValues(smem.parameters);
  processor.patch->processAudio(buffer);
  buffer.comb(smem.audio_output);
}
