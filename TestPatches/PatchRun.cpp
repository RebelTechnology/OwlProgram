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
#include "WavFile.h"

#define SAMPLE_RATE 48000
#define CHANNELS    2
#define BLOCKSIZE   128

#include "DummyPatch.hpp"

static PatchProcessor processor;
ProgramVector programVector;
extern int errorcode;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  printf("Register patch %s (%d ins, %d outs)\n", name, inputs, outputs);
  getInitialisingPatchProcessor()->setPatch(patch, name);
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

int main(int argc, char** argv){
  errorcode = 0;
  programVector.serviceCall = serviceCall;
#include "registerpatch.cpp"
  ASSERT(processor.patch != NULL, "Missing test patch");    
  StereoSampleBuffer* samples = new StereoSampleBuffer(BLOCKSIZE);
  if(argc > 1){
    const char* input_filename = argv[1];
    uint8_t* data = NULL;
    size_t len = fileread(input_filename, &data, 0);
    data = (uint8_t*)malloc(len);
    fileread(input_filename, &data, len);
    WavFile wav(data);
    const int channels = wav.getNumberOfChannels();
    // ASSERT(wav.getSampleRate() == SAMPLE_RATE, "Incorrect sample rate in input file");
    ASSERT(channels == CHANNELS, "Incorrect number of channels in input file");
    ASSERT(wav.getBitsPerSample() == 16, "Incorrect number of bits per sample in input file");
    ASSERT(wav.isValid(), "Invalid wav");
    int16_t* src = (int16_t*)wav.getData();
    int16_t* end = src+wav.getNumberOfSamples();
    while(src+BLOCKSIZE <= end){
      samples->split16(src, BLOCKSIZE);
      processor.patch->processAudio(*samples);
      samples->comb16(src);
      src += BLOCKSIZE*channels;
    }
    if(argc > 2){
      const char* output_filename = argv[2];
      filewrite(output_filename, data, len);
    }
    free(data);
  }else{
    processor.patch->processAudio(*samples);
  }
  delete samples;
  delete processor.patch;  
  return errorcode;
}
