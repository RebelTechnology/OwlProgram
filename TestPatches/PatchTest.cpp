#include "message.h"
#include "TestPatch.hpp"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include <stdio.h>

#include "registerpatch.h"

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
PatchProcessor processor;
ProgramVector programVector;

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

Patch::Patch(){}
Patch::~Patch(){}
PatchProcessor::PatchProcessor(){}
PatchProcessor::~PatchProcessor(){}
int Patch::getBlockSize(){return 128;}

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

static TestPatch* testpatch = NULL;
void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, TestPatch* patch){
  if(patch == NULL)
    error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  testpatch = patch;
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

int main(int argc, char** argv){
#include "registerpatch.cpp"
  ASSERT(testpatch != NULL, "Missing test patch");    
  int ret = 0;
  printf("Passed %d Failed %d\n", testpatch->passed, testpatch->failed);
  if(testpatch->success){
    printf("Success\n");
  }else{
    printf("Fail\n");
    ret = -1;
  }
  return ret;
}
