#include "VoltsPerOctave.h"
#include "message.h"
#include "ProgramVector.h"
#include "ServiceCall.h"
#include <stdint.h>

VoltsPerOctave::VoltsPerOctave(float o, float m)
  : tune(0.0), offset(o), multiplier(m) {
}

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

void VoltsPerOctave::getFrequency(FloatArray samples, FloatArray output){
  ASSERT(output.getSize() >= samples.getSize(), "Output buffer too short");
  // Block based implementation - this is giving ~1% higher CPU usage, tested on Magus
  /*
  output.copyFrom(samples);
  output.subtract(offset);
  output.multiply(multiplier);
  output.add(tune);
  for(size_t i = 0; i < samples.getSize(); ++i)
    output[i] = exp2f(output[i]);
  output.multiply(440.f);
  */

  // Sample by sample processing
  for(size_t i=0; i<samples.getSize(); ++i)
     output[i] = getFrequency(samples[i]);
}

void VoltsPerOctave::getFrequency(FloatArray samples){
  getFrequency(samples, samples);
}

void VoltsPerOctave::getSample(FloatArray frequencies, FloatArray output){
  ASSERT(output.getSize() >= frequencies.getSize(), "Output buffer too short");
  // Block based implementation - this gives the same CPU load as sample
  // by sample processing, so probably no reason to use it
  /*
  for (size_t i = 0; i < output.getSize(); i++)
    //output[i] = tune - hertzToVolts(output[i]);
    output[i] = log2f(output[i]);
  output.add(tune - log2f(440.0f));
  output.multiply(1.0f / multiplier);
  output.add(offset);
  */
  
  // Sample by sample processing
  for(size_t i=0; i < frequencies.getSize(); ++i)
    output[i] = getSample(frequencies[i]);
}
void VoltsPerOctave::getSample(FloatArray frequencies){
  getSample(frequencies, frequencies);
}
