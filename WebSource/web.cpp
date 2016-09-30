#include <string.h>
#include <inttypes.h>
#include "ProgramVector.h"
#include "Patch.h"
#include "device.h"
#include "main.h"
#include "message.h"
#include "PatchProcessor.h"
#include "malloc.h"
#include <math.h>
#include <time.h>

#define NO_ERROR         0x00
#define HARDFAULT_ERROR  0x10
#define BUS_ERROR        0x20
#define MEM_ERROR        0x30
#define NMI_ERROR        0x40
#define USAGE_ERROR      0x50
#define PROGRAM_ERROR    0x60

ProgramVector programVector;
extern PatchProcessor* getInitialisingPatchProcessor();
extern char* itoa(int val, int base);

extern "C"{
  /* ASM exported functions */
  int WEB_setup(long fs, int bs);
  void WEB_processBlock(float** inputs, float** outputs);
  void WEB_setParameter(int pid, float value);
  void WEB_setButtons(int values);
  int WEB_getButtons();
  char* WEB_getMessage();
  char* WEB_getStatus();
  char* WEB_getPatchName();
  char* WEB_getParameterName(int pid);

  void *pvPortMalloc( size_t xWantedSize );
  void vPortFree( void *pv );
}

extern "C"{
  void registerPatch(const char* name, uint8_t inputChannels, uint8_t outputChannels);
  void registerPatchParameter(uint8_t id, const char* name);
  void programReady();
  void programStatus(ProgramVectorAudioStatus status);
  int serviceCall(int service, void** params, int len);
}

unsigned long systicks(){
  // return clock() / (CLOCKS_PER_SEC / 1000);
  return clock();
}

#define NOF_PARAMETERS 16
static int blocksize;
static char* patchName = NULL;
static int16_t parameters[NOF_PARAMETERS];
static char* parameterNames[NOF_PARAMETERS];
static volatile uint16_t buttons = 1<<2; // GREEN

void WEB_setParameter(int pid, float value){
  if(pid < NOF_PARAMETERS)
    parameters[pid] = value*4096;
}

void WEB_setButtons(int values){
  if(values != buttons){
    if((buttons&(1<<PUSHBUTTON)) != (values&(1<<PUSHBUTTON)))
      getInitialisingPatchProcessor()->patch->buttonChanged(PUSHBUTTON, values&(1<<PUSHBUTTON)?4095:0, 0);
    buttons = values;
  }
}

int WEB_getButtons(){
  return buttons;
}

int WEB_setup(long fs, int bs){
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
  pv->buttons = buttons;
  pv->registerPatch = registerPatch;
  pv->registerPatchParameter = registerPatchParameter;
  pv->cycles_per_block = 0;
  pv->heap_bytes_used = 0;
  pv->programReady = programReady;
  pv->programStatus = programStatus;
  pv->serviceCall = serviceCall;
  pv->message = NULL;
  setup(pv);

  struct mallinfo minfo = mallinfo();
  // pv->heap_bytes_used = minfo.uordblks;
  pv->heap_bytes_used = minfo.arena;

  return 0;
}

class MemBuffer : public AudioBuffer {
protected:
  float** buffer;
  int channels;
  int size;
public:
  MemBuffer(float** buf, int ch, int sz): buffer(buf), channels(ch), size(sz) {}
  ~MemBuffer(){}
  FloatArray getSamples(int channel){
    return FloatArray(buffer[channel], size);
  }
  int getChannels(){
    return channels;
  }
  int getSize(){
    return size;
  }
  void clear(){
    // memset(buffer, 0, size*channels*sizeof(float));
  }
};

void WEB_processBlock(float** inputs, float** outputs){
  unsigned long now = systicks();
  ProgramVector* pv = getProgramVector();
  MemBuffer buffer(inputs, 2, blocksize);
  PatchProcessor* processor = getInitialisingPatchProcessor();
  pv->buttons = buttons;
  processor->setParameterValues(pv->parameters);
  processor->patch->processAudio(buffer);
  memcpy(outputs[0], inputs[0], blocksize*sizeof(float));
  memcpy(outputs[1], inputs[1], blocksize*sizeof(float));
  pv->cycles_per_block = systicks()-now;
  buttons = pv->buttons;
}

char* WEB_getMessage(){
  char* msg = getProgramVector()->message;
  // getProgramVector()->message = NULL;
  return msg;
}

char* WEB_getStatus(){
  static char buffer[64];
  char* p = buffer;
  ProgramVector* pv = getProgramVector();
  if(pv == NULL)
    return (char*)"Missing program vector";
  uint8_t err = pv->error;
  switch(err & 0xf0){
  case NO_ERROR: {
    // p = stpcpy(p, (const char*)"No error");
    p = stpcpy(p, (const char*)"CPU ");
    float percent = (pv->cycles_per_block/pv->audio_blocksize) / (float)3500;
    p = stpcpy(p, itoa(ceilf(percent*100), 10));
    p = stpcpy(p, (const char*)"% Heap ");
    int mem = pv->heap_bytes_used;
    p = stpcpy(p, itoa(mem, 10));
    break;
  }
  case MEM_ERROR:
    p = stpcpy(p, (const char*)"Memory Error 0x");
    p = stpcpy(p, itoa(err, 16));
    break;
  case BUS_ERROR:
    p = stpcpy(p, (const char*)"Bus Error 0x");
    p = stpcpy(p, itoa(err, 16));
    break;
  case USAGE_ERROR:
    p = stpcpy(p, (const char*)"Usage Error 0x");
    p = stpcpy(p, itoa(err, 16));
    break;
  case NMI_ERROR:
    p = stpcpy(p, (const char*)"Non-maskable Interrupt 0x");
    p = stpcpy(p, itoa(err, 16));
    break;
  case HARDFAULT_ERROR:
    p = stpcpy(p, (const char*)"HardFault Error 0x");
    p = stpcpy(p, itoa(err, 16));
    break;
  case PROGRAM_ERROR:
    p = stpcpy(p, (const char*)"Missing or Invalid Program 0x");
    p = stpcpy(p, itoa(err, 16));
    break;
  default:
    p = stpcpy(p, (const char*)"Unknown Error 0x");
    p = stpcpy(p, itoa(err, 16));
    break;
  }
  return buffer;
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

void *pvPortMalloc( size_t xWantedSize ){
#ifdef malloc
#undef malloc
#endif
  return malloc(xWantedSize);
}
void vPortFree( void *pv ){
#ifdef free
#undef free
#endif
  free(pv);
}
