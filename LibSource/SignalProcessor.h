#ifndef __SignalProcessor_h__
#define __SignalProcessor_h__

#include "FloatArray.h"
#include "ComplexFloatArray.h"

/**
 * Base class for signal processors such as Filters
 */
class SignalProcessor {
public:
  virtual ~SignalProcessor(){}
  virtual float process(float input){
    return input;
  }
  virtual void process(FloatArray input, FloatArray output){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = process(input[i]);
  }
};

class AudioBuffer;
class MultiSignalProcessor {
public:
  virtual ~MultiSignalProcessor(){}
  virtual void process(AudioBuffer& input, AudioBuffer& output) = 0;
};

class ComplexSignalProcessor {
public:
  virtual ~ComplexSignalProcessor(){}
  virtual ComplexFloat process(ComplexFloat input) = 0;
  virtual void process(ComplexFloatArray input, ComplexFloatArray output) {
    for (size_t i = 0; i < output.getSize(); i++)
      output[i] = process(ComplexFloat(input[i]));
  }
};
#endif // __SignalProcessor_h__
