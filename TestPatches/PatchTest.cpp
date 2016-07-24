#ifndef TESTCLASS 
#define TESTCLASS ShortFastFourierTestPatch
#endif

#include "message.h"
#include "TestPatch.hpp"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "ShortArrayTestPatch.hpp"
#include "ShortFastFourierTestPatch.hpp"
#include <stdio.h>

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
}
}
PatchProcessor processor;
ProgramVector programVector;

void assert_failed(const char* msg, const char* location, int line){
  printf("Assertion failed: %s, in %s line %d\n", msg, location, line);
}
void debugMessage(char const* msg, int i){
  printf("%s %d\n", msg, i);
}

Patch::Patch(){}
Patch::~Patch(){}
PatchProcessor::PatchProcessor(){}
PatchProcessor::~PatchProcessor(){}
int Patch::getBlockSize(){return 128;}

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

int main(int argc, char** argv){
  TestPatch* patch = new TESTCLASS();
  int ret = 0;
  printf("Passed %d Failed %d\n", patch->passed, patch->failed);
  if(patch->success){
    printf("Success\n");
  }else{
    printf("Fail\n");
    ret = -1;
  }
  return ret;
}
