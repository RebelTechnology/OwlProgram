#include <cstddef>
#include <string.h>
#include "StompBox.h"
#include "device.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "basicmaths.h"

AudioBuffer::~AudioBuffer(){}

PatchProcessor* getInitialisingPatchProcessor();

Patch::Patch(){}

Patch::~Patch(){}

void Patch::registerParameter(PatchParameterId pid, const char* name){
  if(getProgramVector()->registerPatchParameter != NULL)
    getProgramVector()->registerPatchParameter(pid, name);
}

double Patch::getSampleRate(){
  return getProgramVector()->audio_samplingrate;
}

int Patch::getBlockSize(){
  return getProgramVector()->audio_blocksize;
}

float Patch::getParameterValue(PatchParameterId pid){
  //  return getInitialisingPatchProcessor()->getParameterValue(pid);
  // if(pid < getProgramVector()->parameters_size)
  return getProgramVector()->parameters[pid]/4096.0f;
}

AudioBuffer* Patch::createMemoryBuffer(int channels, int samples){
  return AudioBuffer::create(channels, samples);
}

void Patch::setButton(PatchButtonId bid, bool pressed){
  if(pressed)
    getProgramVector()->buttons |= 1<<bid;
  else
    getProgramVector()->buttons &= ~(1<<bid);
}

bool Patch::isButtonPressed(PatchButtonId bid){
  return getProgramVector()->buttons & (1<<bid);
}

int Patch::getSamplesSinceButtonPressed(PatchButtonId bid){
  int index = bid+PARAMETER_F;
  return index <= getProgramVector()->parameters_size ? 
    getProgramVector()->parameters[index] : 0;
}

#define DWT_CYCCNT ((volatile unsigned int *)0xE0001004)

float Patch::getElapsedBlockTime(){
  return (*DWT_CYCCNT)/getBlockSize()/3500.0;
}

int Patch::getElapsedCycles(){
  return *DWT_CYCCNT;
}

#include "MemoryBuffer.hpp"
AudioBuffer* AudioBuffer::create(int channels, int samples){
  return new ManagedMemoryBuffer(channels, samples);
}

FloatParameter Patch::getParameter(const char* name, float min, float max, float defaultValue, PatchParameterScale scale, float lambda, float delta){
  return getInitialisingPatchProcessor()->getParameter(name, min, max, defaultValue, scale, lambda, delta);
}

IntParameter Patch::getParameter(const char* name, int min, int max, int defaultValue, PatchParameterScale scale, float lambda, float delta){
  return getInitialisingPatchProcessor()->getParameter(name, min, max, defaultValue, scale, lambda, delta);
}

