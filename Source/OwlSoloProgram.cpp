#include "SharedMemory.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"

#include "../Libraries/OwlPatches/SimpleDelayPatch.hpp"
#include "../Libraries/OwlPatches/GainPatch.hpp"

PatchProcessor processor;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

void setup(){
  // processor.setPatch(new GainPatch());
  processor.setPatch(new SimpleDelayPatch());
}

SampleBuffer buffer;

void processBlock(){
  buffer.split(smem.audio_input, smem.audio_blocksize);
  processor.setParameterValues(smem.parameters);
  processor.patch->processAudio(buffer);
  buffer.comb(smem.audio_output);
}
