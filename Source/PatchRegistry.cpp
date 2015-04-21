#include "PatchRegistry.h"
#include "SharedMemory.h"
#include "basicmaths.h"
#include "OwlPatches/includes.h"
#include "PatchDefinition.hpp"

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, Register<T>::construct)

PatchRegistry::PatchRegistry() : nofPatches(0) {
}

void PatchRegistry::init(){
  nofPatches = 0;
#include "OwlPatches/patches.cpp"
}

const char* PatchRegistry::getName(unsigned int index){
  if(index < getNumberOfPatches())
    return names[index];
  return "";  
}

unsigned int PatchRegistry::getNumberOfPatches(){
  return nofPatches;
}

Patch* PatchRegistry::create(unsigned int index) {
  if(index < getNumberOfPatches())
    return (*creators[index])();
  return NULL;
}

void PatchRegistry::registerPatch(const char* name, uint8_t inputs, uint8_t outputs,
				  PatchCreator creator){
  if(nofPatches < MAX_NUMBER_OF_PATCHES){
    names[nofPatches] = name;
    creators[nofPatches] = creator;
    nofPatches++;
    if(getSharedMemory()->registerPatch != NULL)
      getSharedMemory()->registerPatch(name, inputs, outputs);
  }
}
