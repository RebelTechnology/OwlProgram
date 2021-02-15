#include "malloc.h"
#include "heap.h"
void *pvPortMalloc( size_t xWantedSize ){
  return malloc(xWantedSize);
}
void vPortFree( void *pv ){
  free(pv);
}
#include "basicmaths.h"
#include "message.h"
#include "TestPatch.hpp"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "MemoryBuffer.hpp"
#include <stdio.h>

#include "registerpatch.h"

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define BLOCKSIZE 64

#include "DummyPatch.hpp"

PatchProcessor processor;
ProgramVector programVector;
static int errorcode = 0;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

extern "C"{
  void error(int8_t code, const char* reason){
    printf("%s\n", reason);
    errorcode = -1;
  }
}

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  getInitialisingPatchProcessor()->setPatch(patch, name);
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

int main(int argc, char** argv){
  errorcode = 0;
#include "registerpatch.cpp"
  ASSERT(getInitialisingPatchProcessor()->patch != NULL, "Missing test patch");    
  AudioBuffer* samples = AudioBuffer::create(CHANNELS, BLOCKSIZE);
  getInitialisingPatchProcessor()->patch->processAudio(*samples);
  delete samples;
  delete getInitialisingPatchProcessor()->patch;  
  return errorcode;
}
