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

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

extern "C"{
void error(int8_t code, const char* reason){
  printf("%s\n", reason);
  exit(-1);
}
}
static Patch* testpatch = NULL;
void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(patch == NULL)
   error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  testpatch = patch;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

int main(int argc, char** argv){
#include "registerpatch.cpp"
  ASSERT(testpatch != NULL, "Missing test patch");    
  int ret = 0;
  AudioBuffer* samples = AudioBuffer::create(CHANNELS, BLOCKSIZE);
  testpatch->processAudio(*samples);
  // printf("Passed %d Failed %d\n", testpatch->passed, testpatch->failed);
  // if(testpatch->success){
  //   printf("Success\n");
  // }else{
  //   printf("Fail\n");
  //   ret = -1;
  // }
  return ret;
}
