#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include "FloatArray.h"

class Oscillator {
public:
  Oscillator(){}
  virtual ~Oscillator(){}
  virtual float getNextSample(){ return 0.0f; }
  virtual float getNextSample(float fm){ return 0.0f; }
  /* Fills @param output with samples */
  virtual void getSamples(FloatArray output){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = getNextSample();
  }
  virtual void getSamples(FloatArray output, FloatArray fm){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = getNextSample(fm[i]);
  }
  virtual void setSampleRate(float value){}
  virtual void setFrequency(float value){}
  virtual void reset(){}
};

#endif /* OSCILLATOR_HPP */
