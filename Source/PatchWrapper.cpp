#include "ProgramVector.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "basicmaths.h"
#include "message.h"
#include "FloatArray.h"
#include "ComplexFloatArray.h"
#include "FastFourierTransform.h"
#include "Window.hpp"
#include "BiquadFilter.hpp"
#include "patch.h"

#include <emscripten.h>
#include <map>
#include <string>

extern "C" {
  void setup();
  void processBlock();
  // void OWL_create(int samplingFreq);
  // void OWL_process();
  // void OWL_destroy();
};

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
#include "patch.cpp"
}

SampleBuffer buffer;

void processBlock(){
  buffer.split(getProgramVector()->audio_input, getProgramVector()->audio_blocksize);
  processor.setParameterValues(getProgramVector()->parameters);
  processor.patch->processAudio(buffer);
  buffer.comb(getProgramVector()->audio_output);
}
