#include <cstddef>
#include <string.h>
#include "StompBox.h"
#include "owlcontrol.h"
#include "device.h"
#include "SharedMemory.h"

AudioBuffer::~AudioBuffer(){}

Patch::Patch() { // : processor(patches.getInitialisingPatchProcessor()){
}

Patch::~Patch(){}

void Patch::registerParameter(PatchParameterId pid, const char* name, const char* description){
  if(smem.registerPatchParameter != NULL)
    smem.registerPatchParameter(pid, name);
}

double Patch::getSampleRate(){
  return smem.audio_samplingrate;
}

int Patch::getBlockSize(){
  return smem.audio_blocksize;
}

float Patch::getParameterValue(PatchParameterId pid){
  if(pid < smem.parameters_size)
    return smem.parameters[pid];
  return 0.0;
}

AudioBuffer* Patch::createMemoryBuffer(int channels, int samples){
  return NULL; // todo
  // return processor->createMemoryBuffer(channels, samples);
}

void Patch::setButton(PatchButtonId bid, bool pressed){
  setButton(bid, pressed);
  switch(bid){
  case PUSHBUTTON:
    toggleLed();
    break;
  case GREEN_BUTTON:
    setLed(GREEN);
    break;
  case RED_BUTTON:
    setLed(RED);
    break;
  }
}

bool Patch::isButtonPressed(PatchButtonId bid){
  return smem.buttons & (1<<bid);
}
