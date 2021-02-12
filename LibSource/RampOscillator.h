#ifndef RAMPOSCILLATOR_H
#define RAMPOSCILLATOR_H

#include "Oscillator.h"

class RampOscillator : public Oscillator {
private:
  float phase;
  float incr;
public:
  RampOscillator() : phase(0.0f), incr(0.0f) {    
  }
  RampOscillator(float freq, float sr) : phase(0.0f){
    setFrequency(freq, sr);
  }    
  void reset(){
    phase = 0.0f;
  }
  using Oscillator::setFrequency;
  void setFrequency(float freq){
    incr = freq*2;
  }
  void setPhase(float ph){
    phase = ph;
    while(phase >= 1)
      phase -= 1;
  }
  float getPhase(){
    return phase;
  }
  float generate(){
    float sample = phase;
    phase += incr;
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }
  float generate(float fm){
    float sample = phase;
    phase += incr + fm;
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }  
  static RampOscillator* create(){
    return new RampOscillator();
  }
  static RampOscillator* create(float freq, float sr){
    return new RampOscillator(freq, sr);
  }
  static void destroy(RampOscillator* osc){
    delete osc;
  }
};

#endif /* RAMPOSCILLATOR_H */
