#ifndef SINE_OSCILLATOR_H
#define SINE_OSCILLATOR_H

#include "Oscillator.h"

class SineOscillator : public Oscillator {
private:
  float phase;
  float incr;
public:
  SineOscillator() : phase(0.0f), incr(0.0f) {}
  SineOscillator(float freq, float sr) : phase(0.0f){
    setFrequency(freq, sr);
  }
  using Oscillator::setFrequency;
  void setFrequency(float nfreq){
    incr = nfreq*2*M_PI;
  }
  void reset(){
    phase = 0.0f;
  }
  void setPhase(float ph){
    phase = ph;
    while(phase >= 2*M_PI)
      phase -= 2*M_PI;
  }
  float getPhase(){
    return phase;
  }
  float generate(){
    float sample = sinf(phase);
    phase += incr;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }
  float generate(float fm){
    float sample = sinf(phase);
    phase += incr + fm;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }
  static SineOscillator* create(){
    return new SineOscillator();
  }
  static SineOscillator* create(float freq, float sr){
    return new SineOscillator(freq, sr);
  }
  static void destroy(SineOscillator* osc){
    delete osc;
  }
};

#endif /* SINE_OSCILLATOR_H */
