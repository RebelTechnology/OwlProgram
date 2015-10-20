#include "ProgramVector.h"
#include "ServiceCall.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "message.h"
#include "FloatArray.h"
#include "ComplexFloatArray.h"
#include "FastFourierTransform.h"
#include "Window.hpp"
#include "BiquadFilter.hpp"
#include "Resample.h"
#include "PitchDetector.hpp"
#include "FirFilter.hpp"
#include "patch.h"
#include "main.h"

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
