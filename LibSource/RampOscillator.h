#ifndef RAMPOSCILLATOR_H
#define RAMPOSCILLATOR_H

#include "Oscillator.h"

class RampOscillator : public Oscillator {
private:
  float mul;
  float phase;
  float incr;
public:
  RampOscillator(float sr=48000) : phase(0.0f), incr(0.0f) {
  }
  RampOscillator(float freq, float sr) : phase(0.0f){
    setSampleRate(sr);
    setFrequency(freq);
  }
  void reset(){
    phase = 0.0f;
  }
  void setSampleRate(float sr){
    mul = 2.0f/sr;
  }
  float getS2ampleRate(){
    return 2.0f/mul;
  }
  void setFrequency(float freq){
    incr = freq*mul;
  }
  float getFrequency(){
    return incr/mul;
  }
  void setPhase(float ph){
    phase = ph/M_PI - 1.0f; // internal phase is -1 to 1
  }
  float getPhase(){
    // return phase 0 to 2*pi
    return phase*M_PI+M_PI;
  }
  float generate(){
    float sample = phase;
    phase += incr;
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }
  void generate(FloatArray output){
    size_t len = output.getSize();
    float* dest = output;
    while(phase + incr*len >= 1.0f){
      float remain = 1.0f - phase;
      size_t steps = (size_t)(remain/incr);
      for(size_t i=0; i<steps; ++i){
	*dest++ = phase;
	phase += incr;
      }
      phase -= 2.0f;
      len -= steps;
    }
    for(size_t i=0; i<len; ++i){
      *dest++ = phase;
      phase += incr;
    }
  }
  float generate(float fm){
    float sample = phase;
    phase += incr + fm;
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }  
  static RampOscillator* create(float sr){
    return new RampOscillator(sr);
  }
  static RampOscillator* create(float freq, float sr){
    return new RampOscillator(freq, sr);
  }
  static void destroy(RampOscillator* osc){
    delete osc;
  }
};

#endif /* RAMPOSCILLATOR_H */
