#ifndef SINE_OSCILLATOR_H
#define SINE_OSCILLATOR_H

#include "Oscillator.h"

class SineOscillator : public Oscillator {
private:
  float mul;
  float phase;
  float incr;
public:
  SineOscillator() : phase(0.0f), incr(0.0f) {    
    setSampleRate(48000);
  }
  SineOscillator(float sr) : phase(0.0f), incr(0.0f){
    setSampleRate(sr);
  }    
  void setSampleRate(float sr){
    mul = 2*M_PI/sr;
  }
  void setFrequency(float freq){
    incr = freq*mul;
  }
  void setPhase(float ph){
    phase = ph;
    while(phase >= 2*M_PI)
      phase -= 2*M_PI;
  }
  void reset(){
    phase = 0.0f;
  }
  float getPhase(){
    return phase;
  }
  float getNextSample(){
    float sample = sinf(phase);
    phase += incr;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }
  float getNextSample(float fm){
    float sample = sinf(phase);
    phase += incr + fm;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }  
  static SineOscillator* create(float sr){
    return new SineOscillator(sr);
  }
  static void destroy(SineOscillator* osc){
    delete osc;
  }
};

#endif /* SINE_OSCILLATOR_H */
