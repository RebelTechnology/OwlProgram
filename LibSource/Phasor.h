#ifndef PHASOR_H
#define PHASOR_H

#include "Oscillator.h"

class Phasor : public Oscillator {
private:
  float mul;
  float phase;
  float incr;
public:
  Phasor() : phase(0.0f), incr(0.0f) {    
    setSampleRate(48000);
  }
  Phasor(float sr) : phase(0.0f), incr(0.0f){
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
      phase -= 1.0f;
    return sample;
  }
  float getNextSample(float fm){
    float sample = phase;
    phase += incr + fm;
    if(phase >= 1.0f)
      phase -= 1.0f;
    return sample;
  }  
  static Phasor* create(float sr){
    return new Phasor(sr);
  }
  static void destroy(Phasor* osc){
    delete osc;
  }
};

#endif /* PHASOR_H */
