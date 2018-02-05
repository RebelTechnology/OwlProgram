#ifndef __Control_h__
#define __Control_h__

#include "Patch.h"
#include "message.h"
#include "main.h"

template<PatchParameterId PID>
class Control {
public:
  Control<PID>(const float value=0){
    ASSERT(PID < getProgramVector()->parameters_size, "Invalid parameter ID");
    set(value);
  }
  void set(const float value){
    if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE && PID < 4)
      doSetPatchParameter(PID, 4095 - (int16_t)(value*4096.0f));
    else
      doSetPatchParameter(PID, (int16_t)(value*4096));
  }
  float get(){
    if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE && PID < 4){
      return (4095 - getProgramVector()->parameters[PID])/4096.0f;
    }else{
      return getProgramVector()->parameters[PID]/4096.0f;
    }
  }
  Control<PID>& operator=(const float value){
    set(value);
    return *this;
  }
  operator float(){
    return get();
  }
};

#endif /* __Control_h__ */
