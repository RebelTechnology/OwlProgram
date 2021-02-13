#include <stdio.h>
#include <malloc.h>

#include "heap.h"
// define these before defining the malloc/free macros
void *pvPortMalloc( size_t xWantedSize ){
  return malloc(xWantedSize);
}
void vPortFree( void *pv ){
  free(pv);
}
#include "basicmaths.h"
#include "message.h"
#include "Patch.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "MemoryBuffer.hpp"
#include "registerpatch.h"
#include "wav.h"

#define SAMPLE_RATE 48000
#define CHANNELS    2
#define BLOCKSIZE   128

static float parameter_values[40] = {};
static uint32_t button_values = 0;

class SampleBuffer : public AudioBuffer {
protected:
  FloatArray left;
  FloatArray right;
  uint16_t size;
  const float mul = 1/2147483648.0f;
public:
  SampleBuffer(int blocksize){
    left = FloatArray::create(blocksize);
    right = FloatArray::create(blocksize);
  }
  ~SampleBuffer(){
    FloatArray::destroy(left);
    FloatArray::destroy(right);
  }
  void split16(int16_t* input, uint16_t blocksize){
    size = blocksize;
    for(int i=0; i<size; ++i){
      left[i] = float(*input++)/INT16_MAX;
      right[i] = float(*input++)/INT16_MAX;
    }
  }
  void comb16(int16_t* output){
    int16_t* dest = output;
    for(int i=0; i<size; ++i){
      *dest++ = ((int16_t)(left[i] * INT16_MAX));
      *dest++ = ((int16_t)(right[i] * INT16_MAX));
    }
  }
  void clear(){
    left.clear();
    right.clear();
  }
  inline FloatArray getSamples(int channel){
    return channel == LEFT_CHANNEL ? left : right;
    // return channel == 0 ? FloatArray(left, size) : FloatArray(right, size);
  }
  inline int getChannels(){
    return 2;
  }
  inline int getSize(){
    return size;
  }
};

extern "C" {
  // http://www.keil.com/forum/60479/
  void arm_bitreversal_32(uint32_t *pSrc, const uint16_t bitRevLen, const uint16_t *pBitRevTab)
{
  uint32_t r3 = (bitRevLen + 1) / 2;
  uint32_t *r2, *r6;
  uint32_t r4, r5;
  while(r3--)
  {
    r2 = (uint32_t *)((uint32_t)pSrc + pBitRevTab[0]);
    r6 = (uint32_t *)((uint32_t)pSrc + pBitRevTab[1]);
    r5 = r2[0];
    r4 = r6[0];
    r6[0] = r5;
    r2[0] = r4;
    r5 = r2[1];
    r4 = r6[1];
    r6[1] = r5;
    r2[1] = r4;
    pBitRevTab += 2;
  }
}

void arm_bitreversal_16(uint32_t *pSrc, const uint16_t bitRevLen, const uint16_t *pBitRevTab)
{
  #warning TODO!
  // ASSERT(false, "arm_bitreversal_16");
}
}

void assert_failed(const char* msg, const char* location, int line){
  printf("Assertion failed: %s, in %s line %d\n", msg, location, line);
  exit(-1);
}

void debugMessage(char const* msg, int a){
  printf("%s %d\n", msg, a);
}

void debugMessage(char const* msg, float a){
  printf("%s %f\n", msg, a);
}

void debugMessage(char const* msg, int a, int b){
  printf("%s %d %d\n", msg, a, b);
}

void debugMessage(char const* msg, float a, float b){
  printf("%s %f %f\n", msg, a, b);
}

void debugMessage(char const* msg, int a, int b, int c){
  printf("%s %d %d %d\n", msg, a, b, c);
}

void debugMessage(char const* msg, float a, float b, float c){
  printf("%s %f %f %f\n", msg, a, b, c);
}

void debugMessage(char const* msg){
  printf("%s\n", msg);
}

AudioBuffer* AudioBuffer::create(int channels, int samples){
  return new ManagedMemoryBuffer(channels, samples);
}
AudioBuffer::~AudioBuffer(){}

Patch::Patch(){}
Patch::~Patch(){}
PatchProcessor::PatchProcessor(){}
PatchProcessor::~PatchProcessor(){}
void Patch::registerParameter(PatchParameterId pid, const char* name){
  printf("Register parameter %c: %s\n", 'A'+pid, name);
}  
void Patch::processMidi(MidiMessage msg){}
void Patch::sendMidi(MidiMessage msg){
  printf("Sending MIDI [%x:%x:%x:%x]\n", msg.data[0], msg.data[1], msg.data[2], msg.data[3]);
}  

float Patch::getSampleRate(){
  return SAMPLE_RATE;
}

int Patch::getBlockSize(){
  return BLOCKSIZE;
}

int Patch::getNumberOfChannels(){
  return CHANNELS;
}

float Patch::getParameterValue(PatchParameterId pid){
  if(pid < 40)
    return parameter_values[pid];
  return 0.0f;
}

void Patch::setParameterValue(PatchParameterId pid, float value){
  printf("Set parameter %c: %f\n", 'A'+pid, value);
  if(pid < 40)
    parameter_values[pid] = value;
}

void Patch::setButton(PatchButtonId bid, uint16_t value, uint16_t samples){
  printf("Set button %c: %d\n", 'A'+bid, value);
  if(value)
    button_values |= (1<<bid);
  else
    button_values &= ~(1<<bid);
}

bool Patch::isButtonPressed(PatchButtonId bid){
  return button_values & (1<<bid);
}

static PatchProcessor processor;
ProgramVector programVector;
static Patch* testpatch = NULL;

int serviceCall(int service, void** params, int len){
  printf("Service call (todo) : %d\n", service);
}

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  //if(patch == NULL)
  //  error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  printf("Register patch %s (%d ins, %d outs)\n", name, inputs, outputs);
  testpatch = patch;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

int main(int argc, char** argv){
  programVector.serviceCall = serviceCall;
#include "registerpatch.cpp"
  ASSERT(testpatch != NULL, "Missing patch");    
  int ret = 0;
  SampleBuffer* samples = new SampleBuffer(BLOCKSIZE);
  if(argc > 1){
    const char* input_filename = argv[1];
    WavHeader *wav_header = new WavHeader();
    int16_t* data = NULL;
    wavread(wav_header, input_filename, &data);
    ASSERT(wav_header->num_channels == CHANNELS, "Incorrect number of channels in input file");
    // ASSERT(wav_header->sample_rate == SAMPLE_RATE, "Incorrect sample rate in input file");
    ASSERT(wav_header->bps == 16, "Incorrect number of bits per sample in input file");
    const int channels = wav_header->num_channels;
    const int len = wav_header->datachunk_size/(wav_header->fmtchunk_size/8);
    int16_t* src = data;
    int16_t* end = data+len;
    while(src+BLOCKSIZE <= end){
      samples->split16(src, BLOCKSIZE);
      testpatch->processAudio(*samples);
      samples->comb16(src);
      src += BLOCKSIZE*channels;
    }
    if(argc > 2){
      const char* output_filename = argv[2];
      wavwrite(wav_header, output_filename, data);
    }
    free(data);
    delete wav_header;
  }else{
    testpatch->processAudio(*samples);
  }
  delete samples;
  delete testpatch;  
  return ret;
}
