#include "ProgramVector.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "basicmaths.h"
#include "owlcontrol.h"
#include "solopatch.h"

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

void setup(){
#include "solopatch.cpp"
}

SampleBuffer buffer;

void processBlock(){
  buffer.split(getProgramVector()->audio_input, getProgramVector()->audio_blocksize);
  processor.setParameterValues(getProgramVector()->parameters);
  processor.patch->processAudio(buffer);
  buffer.comb(getProgramVector()->audio_output);
}
