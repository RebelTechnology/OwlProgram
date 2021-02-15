#ifndef __SignalProcessor_h__
#define __SignalProcessor_h__

#include "FloatArray.h"

/**
 * Base class for signal processors such as Filters
 */
class SignalProcessor {
public:
  virtual ~SignalProcessor(){}
  virtual float process(float input) = 0;
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

#endif // __SignalProcessor_h__
