#include <string.h>
#include <inttypes.h>
#include "ProgramVector.h"
#include "StompBox.h"
#include "device.h"
#include "main.h"
#include "PatchProcessor.h"

/*
 * http://thealphanerd.io/blog/from-faust-to-webaudio/
 */

static ProgramVector programVector;
ProgramVector* getProgramVector() { return &programVector; }
extern PatchProcessor* getInitialisingPatchProcessor();

extern "C"{
  /* ASM exported functions */
  void WEB_setup(long fs, int bs);
  void WEB_processBlock(float** inputs, float** outputs);
  void WEB_setParameter(int pid, float value);
  char* WEB_getMessage();
  char* WEB_getPatchName();
  char* WEB_getParameterName(int pid);
}

extern "C"{
  void registerPatch(const char* name, uint8_t inputChannels, uint8_t outputChannels);
  void registerPatchParameter(uint8_t id, const char* name);
  void programReady();
  void programStatus(ProgramVectorAudioStatus status);
  int serviceCall(int service, void** params, int len);
}

#define NOF_PARAMETERS 16
static int blocksize;
static char* patchName = NULL;
static uint16_t parameters[NOF_PARAMETERS];
static char* parameterNames[NOF_PARAMETERS];

void WEB_setParameter(int pid, float value){
  if(pid < NOF_PARAMETERS)
    parameters[pid] = value*4096;
}

void WEB_setup(long fs, int bs){
  for(int i=0; i<NOF_PARAMETERS; ++i){
    parameters[i] = 0;
    parameterNames[i] = NULL;
  }
  blocksize = bs;
  // set up programvector with sample rate, blocksize, callbacks et c
  ProgramVector* pv = getProgramVector();
  pv->checksum = sizeof(ProgramVector);
  pv->hardware_version = OWL_PEDAL_HARDWARE;
  pv->audio_input = NULL;
  pv->audio_output = NULL;
  pv->audio_bitdepth = 32;
  pv->audio_blocksize = bs;
  pv->audio_samplingrate = fs;
  pv->parameters = parameters;
  pv->parameters_size = NOF_PARAMETERS;
  pv->buttons = 0;
  pv->registerPatch = registerPatch;
  pv->registerPatchParameter = registerPatchParameter;
  pv->cycles_per_block = 0;
  pv->heap_bytes_used = 0;
  pv->programReady = programReady;
  pv->programStatus = programStatus;
  pv->serviceCall = serviceCall;
  pv->message = NULL;
  setup();
}

class MemoryBuffer : public AudioBuffer {
protected:
  float** buffer;
  int channels;
  int size;
public:
  MemoryBuffer(float** buf, int ch, int sz): buffer(buf), channels(ch), size(sz) {}
  virtual ~MemoryBuffer(){}
  void clear(){
    // memset(buffer, 0, size*channels*sizeof(float));
  }
  FloatArray getSamples(int channel){
    return FloatArray(buffer[channel], size);
  }
  int getChannels(){
    return channels;
  }
  int getSize(){
    return size;
  }
};

void WEB_processBlock(float** inputs, float** outputs){
  MemoryBuffer buffer(inputs, 2, blocksize);
  PatchProcessor* processor = getInitialisingPatchProcessor();
  processor->setParameterValues(getProgramVector()->parameters);
  processor->patch->processAudio(buffer);
  memcpy(outputs[0], inputs[0], blocksize*sizeof(float));
  memcpy(outputs[1], inputs[1], blocksize*sizeof(float));
}

char* WEB_getMessage(){
  return getProgramVector()->message;
}

char* WEB_getPatchName(){
  return patchName;
}

char* WEB_getParameterName(int pid){
  if(pid<NOF_PARAMETERS)
    return parameterNames[pid];
  else
    return NULL;
}

void registerPatch(const char* name, uint8_t inputChannels, uint8_t outputChannels){
  patchName = (char*)name;
}

void registerPatchParameter(uint8_t pid, const char* name){
  if(pid<NOF_PARAMETERS)
    parameterNames[pid] = (char*)name;
}

void programReady(){}

void programStatus(ProgramVectorAudioStatus status){}

int serviceCall(int service, void** params, int len){
  return -1;
}
