#ifndef SINE_OSCILLATOR_H
#define SINE_OSCILLATOR_H

#include "Oscillator.h"

class SineOscillator : public Oscillator {
private:
  float mul;
  float phase;
  float incr;
public:
  SineOscillator(float sr=48000) : phase(0), incr(0) {
    setSampleRate(sr);
  }
  SineOscillator(float freq, float sr) : phase(0.0f) {
    setSampleRate(sr);
    setFrequency(freq);
  }
  void reset(){
    phase = 0.0f;
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
