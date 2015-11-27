#include "VoltsPerOctave.h"
#include "message.h"
#include "ProgramVector.h"
#include "ServiceCall.h"
#include <stdint.h>

VoltsPerOctave::VoltsPerOctave() : tune(0.0) {
  int32_t volts_offset, volts_scalar;
  void* args[] = {
    (void*)"VO", (void*)&volts_offset, 
    (void*)"VS", (void*)&volts_scalar
  };
  int ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_PARAMETERS, args, 2);
  if(ret == OWL_SERVICE_OK){
    multiplier = (float)volts_scalar/INT32_MAX;
    offset = (float)volts_offset/INT32_MAX;
  }else{
    if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE){
      multiplier = -4.40f;
      offset = -0.0585f;
    }else{
      multiplier = 2.0f;
      offset = 0.0f;
    }
  }
}

VoltsPerOctave::VoltsPerOctave(float o, float m)
  : offset(o), multiplier(m), tune(0.0) {
}

void VoltsPerOctave::getFrequency(FloatArray samples, FloatArray output){
  ASSERT(output.getSize() >= samples.getSize(), "Output buffer too short");
  // todo: block based implementation
  for(int i=0; i<samples.getSize(); ++i)
    output[i] = getFrequency(samples[i]);
}
