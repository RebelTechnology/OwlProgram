#include "PatchParameter.h"
#include "PatchProcessor.h"
#include "SmoothValue.h"
#include "message.h"

PatchProcessor* getInitialisingPatchProcessor();

// copy ctors: superceded by assignment operators
/* PatchParameter(PatchParameter<T>& other); */
/* PatchParameter(const PatchParameter<T>& other); */

// template<typename T>
// PatchParameter<T>::PatchParameter(PatchParameter<T>& other) :
//   pid(other.pid), value(other.value) {
//   // copy ctor
//   // register for update callback in copy constructor
//   debugMessage("const copy ctor", pid, (int)this);
//   if(pid != NO_PID)
//     getInitialisingPatchProcessor()->setPatchParameter(pid, this);
// }

// template<typename T>
// PatchParameter<T>::PatchParameter(const PatchParameter<T>& other) :
//   pid(other.pid), value(other.value) {
//   // copy ctor
//   // register for update callback in copy constructor
//   debugMessage("const copy ctor", pid, (int)this);
//   if(pid != NO_PID)
//     getInitialisingPatchProcessor()->setPatchParameter(pid, this);
// }

template<typename T>
PatchParameter<T>& PatchParameter<T>::operator=( const PatchParameter<T>& other ){
  pid = other.pid;
  value = other.value;
  debugMessage("assignment operator", pid, (int)this);
  if(pid != NO_PID)
    getInitialisingPatchProcessor()->setPatchParameter(pid, this);
  return *this;
}

template class PatchParameter<int>;
template class PatchParameter<float>;
