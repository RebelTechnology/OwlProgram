#include "VoltsPerOctave.h"
#include "message.h"
#include "ProgramVector.h"
#include "ServiceCall.h"
#include <stdint.h>

VoltsPerOctave::VoltsPerOctave(bool input) : tune(0.0) {
  int32_t volts_offset = 0, volts_scalar = 0;
  void* args[] = {
    (void*)(input ? "IO" : "OO"), (void*)&volts_offset, 
    (void*)(input ? "IS" : "OS"), (void*)&volts_scalar
  };
  int ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_PARAMETERS, args, 4);
  if(ret == OWL_SERVICE_OK){
    multiplier = (float)volts_scalar/UINT16_MAX;
    offset = (float)volts_offset/UINT16_MAX;
  }else{
    if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE){
      if(input){
	multiplier = -4.29;
	offset = -0.06382;
      }else{
	multiplier = -4.642;
	offset = 0.1208;
      }
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
  // samples.subtract(offset, output);
  // samples.multiply(multiplier, output);
  // for(int i=0; i<samples.getSize(); ++i)
  //   output[i] = voltsToHertz(output[i]);
  for(int i=0; i<samples.getSize(); ++i)
    output[i] = getFrequency(samples[i]);
}
