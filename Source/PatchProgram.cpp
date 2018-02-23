#include <string> /* include this here to avoid errors with std::min/std::max later */
#include "ProgramVector.h"
#include "ServiceCall.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "message.h"
#include "Patch.h"
#include "registerpatch.h"
#include "main.h"
#include "heap.h"
#include "system_tables.h"

static PatchProcessor processor;
PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

void doSetPatchParameter(uint8_t id, int16_t value){
  ProgramVector* vec = getProgramVector();
  if(vec->checksum >= PROGRAM_VECTOR_CHECKSUM_V12 &&
     vec->setPatchParameter != NULL && vec->parameters[id] != value)
    vec->setPatchParameter(id, value);
}

void doSetButton(uint8_t id, uint16_t value, uint16_t samples){
  ProgramVector* vec = getProgramVector();
  if(vec->checksum >= PROGRAM_VECTOR_CHECKSUM_V12 &&
     vec->setButton != NULL &&
     // if it is not a MIDI note, check that value has changed
     (id > 31 || (bool)(vec->buttons&(1<<id)) != (bool)value))
    vec->setButton((PatchButtonId)id, value, samples);
  if(id < 32){
    if(value)
      vec->buttons |= (1<<id);
    else
      vec->buttons &= ~(1<<id);
  }
}

void onButtonChanged(uint8_t id, uint16_t value, uint16_t samples){
  if(processor.patch != NULL)
    processor.patch->buttonChanged((PatchButtonId)id, value, samples);
}

#ifdef USE_SCREEN
void onDrawCallback(uint8_t* pixels, uint16_t width, uint16_t height){
  if(processor.patch != NULL){
    ScreenBuffer screen(width, height);
    screen.setBuffer(pixels);
    processor.patch->processScreen(screen);
  }
}
#endif /* USE_SCREEN */

#ifdef USE_MIDI_CALLBACK
void onMidiCallback(uint8_t port, uint8_t status, uint8_t d1, uint8_t d2){
  static MidiMessage msg;
  if(processor.patch != NULL){
    msg.data[0] = port;
    msg.data[1] = status;
    msg.data[2] = d1;
    msg.data[3] = d2;
    processor.patch->processMidi(msg);
  }
}
#endif /* USE_MIDI_CALLBACK */

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(patch == NULL)
    error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  if(getProgramVector()->registerPatch != NULL)
    getProgramVector()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch, name);
}

static SampleBuffer* samples;
void setup(ProgramVector* pv){
  setSystemTables(pv);
#ifdef USE_SCREEN
  void* drawArgs[] = {(void*)SYSTEM_FUNCTION_DRAW, (void*)&onDrawCallback};
  getProgramVector()->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, drawArgs, 2);
#endif /* USE_SCREEN */
#ifdef USE_MIDI_CALLBACK
  void* midiArgs[] = {(void*)SYSTEM_FUNCTION_MIDI, (void*)&onMidiCallback};
  getProgramVector()->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, midiArgs, 2);  
#endif /* USE_MIDI_CALLBACK */
  samples = new SampleBuffer(pv->audio_blocksize);
#include "registerpatch.cpp"
}

void run(ProgramVector* pv){
  if(pv->audio_format == AUDIO_FORMAT_24B32){
    for(;;){
      pv->programReady();
#if 0
  memcpy(pv->audio_output, pv->audio_input, pv->audio_blocksize*2*sizeof(uint32_t));
#else
      samples->split32(pv->audio_input, pv->audio_blocksize);
      processor.setParameterValues(pv->parameters);
      processor.patch->processAudio(*samples);
      samples->comb32(pv->audio_output);
#endif
    }
  }else{
    for(;;){
      pv->programReady();
      samples->split16(pv->audio_input, pv->audio_blocksize);
      processor.setParameterValues(pv->parameters);
      processor.patch->processAudio(*samples);
      samples->comb16(pv->audio_output);
    }
  }
}
