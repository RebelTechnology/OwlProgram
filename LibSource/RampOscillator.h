#ifndef RAMPOSCILLATOR_H
#define RAMPOSCILLATOR_H

#include "Oscillator.h"

class RampOscillator : public Oscillator {
private:
  float mul;
  float phase;
  float incr;
public:
  RampOscillator() : phase(0.0f), incr(0.0f) {    
    setSampleRate(48000);
  }
  RampOscillator(float sr) : phase(0.0f), incr(0.0f){
    setSampleRate(sr);
  }    
  void setSampleRate(float sr){
    mul = 1.0f/sr;
  }
  void setFrequency(float freq){
    incr = freq*mul;
  }
  void setPhase(float ph){
    phase = ph;
    while(phase >= 1)
      phase -= 1;
  }
  void reset(){
    phase = 0.0f;
  }
  float getPhase(){
    return phase;
  }
  float getNextSample(){
    float sample = phase;
    phase += incr;
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }
  float getNextSample(float fm){
    float sample = phase;
    phase += incr + fm;
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }  
  static RampOscillator* create(float sr){
    return new RampOscillator(sr);
  }
  static void destroy(RampOscillator* osc){
    delete osc;
  }
};

#endif /* RAMPOSCILLATOR_H */
