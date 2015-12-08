#include "PatchParameter.h"
#include "PatchProcessor.h"

PatchProcessor* getInitialisingPatchProcessor();

template<typename T>
PatchParameter<T>::PatchParameter(const PatchParameter<T>& other){
  // copy ctor
  // register for update callback in copy constructor
  pid = other.pid;
  value = other.value;
  if(pid != NO_PID)
    getInitialisingPatchProcessor()->setPatchParameter(pid, this);
}

template class PatchParameter<int>;
template class PatchParameter<float>;
