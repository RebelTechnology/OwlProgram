#include <cstddef>
#include <string.h>
#include "Patch.h"
#include "device.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "basicmaths.h"
#include "main.h"

AudioBuffer::~AudioBuffer(){}

PatchProcessor* getInitialisingPatchProcessor();

Patch::Patch(){}

Patch::~Patch(){}

void Patch::registerParameter(PatchParameterId pid, const char* name){
  if(getProgramVector()->registerPatchParameter != NULL)
    getProgramVector()->registerPatchParameter(pid, name);
}

float Patch::getSampleRate(){
  return getProgramVector()->audio_samplingrate;
}

int Patch::getBlockSize(){
  return getProgramVector()->audio_blocksize;
}

int Patch::getNumberOfChannels(){
  uint8_t format = getProgramVector()->audio_format;
  if((format & 0xf0) == AUDIO_FORMAT_24B32)
    return format & 0x0f;
  return 2;
}

float Patch::getParameterValue(PatchParameterId pid){
  //  return getInitialisingPatchProcessor()->getParameterValue(pid);
  // if(pid < getProgramVector()->parameters_size)
  if(pid < getProgramVector()->parameters_size){
    if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE && pid < 4){
      return (4095 - getProgramVector()->parameters[pid])/4096.0f;
    }else{
      return getProgramVector()->parameters[pid]/4096.0f;
    }
  }
  return 0.0f;
}

void Patch::setParameterValue(PatchParameterId pid, float value){
  if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE && pid < 4)
    doSetPatchParameter(pid, 4095 - (int16_t)(value*4096.0f));
  else
    doSetPatchParameter(pid, (int16_t)(value*4096));
}

void Patch::setButton(PatchButtonId bid, uint16_t value, uint16_t samples){
  doSetButton(bid, value, samples);
}

bool Patch::isButtonPressed(PatchButtonId bid){
  return getProgramVector()->buttons & (1<<bid);
}

int Patch::getSamplesSinceButtonPressed(PatchButtonId bid){
  // deprecated
  return 0;
}

AudioBuffer* Patch::createMemoryBuffer(int channels, int samples){
  return AudioBuffer::create(channels, samples);
}

#define DWT_CYCCNT ((volatile unsigned int *)0xE0001004)

float Patch::getElapsedBlockTime(){
  return (*DWT_CYCCNT)/getBlockSize()/3500.0;
}

int Patch::getElapsedCycles(){
  return *DWT_CYCCNT;
}

#ifdef USE_SCREEN
  void drawMessage(ScreenBuffer& screen){
    ProgramVector* pv = getProgramVector();
    if(pv->message != NULL){
      screen.setTextSize(1);
      screen.setTextWrap(true);
      screen.print(0, 26, pv->message);
    }    
  }
  void drawTitle(const char* title, ScreenBuffer& screen){
    // draw title
    screen.setTextSize(2);
    screen.print(0, 16, title);
  }
void Patch::processScreen(ScreenBuffer& screen){
  // screen.clear();
  const char* title = getInitialisingPatchProcessor()->getPatchName();
  drawTitle(title, screen);
  drawMessage(screen);
  // const char title[] = "KickBox";
  // screen.setTextSize(2);
  // screen.print(0, 16, title);
}
#endif /* USE_SCREEN */

#ifdef USE_MIDI_CALLBACK
void Patch::processMidi(MidiMessage msg){}

void Patch::sendMidi(MidiMessage msg){
  doMidiSend(msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
}

#endif /* USE_MIDI_CALLBACK */

#include "MemoryBuffer.hpp"
AudioBuffer* AudioBuffer::create(int channels, int samples){
  return new ManagedMemoryBuffer(channels, samples);
}

FloatParameter Patch::getParameter(const char* name, float defaultValue){
  return getFloatParameter(name, 0.0f, 1.0f, defaultValue, 0.0f, 0.0f, LIN);
}

FloatParameter Patch::getFloatParameter(const char* name, float min, float max, float defaultValue, float lambda, float delta, float skew){
  return getInitialisingPatchProcessor()->getParameter(name, min, max, defaultValue, lambda, delta, skew);
}

IntParameter Patch::getIntParameter(const char* name, int min, int max, int defaultValue, float lambda, float delta, float skew){
  return getInitialisingPatchProcessor()->getParameter(name, min, max, defaultValue, lambda, delta, skew);
}

const float Patch::EXP = 0.5;
const float Patch::LIN = 1.0;
const float Patch::LOG = 2.0;
const uint16_t Patch::ON = 4095;
const uint16_t Patch::OFF = 0;
