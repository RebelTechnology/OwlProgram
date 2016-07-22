#ifndef TESTCLASS 
#define TESTCLASS ShortArrayTestPatch
#endif
// #ifndef TESTFILE 
// #define TESTFILE "ShortArrayTestPatch.hpp"
// #endif

// #include TESTFILE
#include "message.h"
#include "TestPatch.hpp"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "ShortArrayTestPatch.hpp"
#include <stdio.h>

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
