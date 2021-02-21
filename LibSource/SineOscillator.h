#ifndef SINE_OSCILLATOR_H
#define SINE_OSCILLATOR_H

#include "Oscillator.h"

class SineOscillator : public Oscillator {
private:
  float mul;
  float phase;
  float incr;
public:
  SineOscillator(float sr=48000) : mul(2*M_PI/sr), phase(0), incr(0) {}
  SineOscillator(float freq, float sr) : mul(2*M_PI/sr), phase(0.0f) {
    setFrequency(freq);
  }
  void reset(){
    phase = 0.0f;
  }
  void setSampleRate(float sr){
    float freq = getFrequency();
    mul = 2*M_PI/sr;
    setFrequency(freq);
  }
  float getSampleRate(){
    return (2*M_PI)/mul;
  }
  void setFrequency(float freq){
    incr = freq*mul;
  }
  float getFrequency(){
    return incr/mul;
  }
  void setPhase(float ph){
    phase = ph;
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
  void generate(FloatArray output){
    size_t len = output.getSize();
    for(size_t i=0; i<len; ++i){
      output[i] = sinf(phase);
      phase += incr; // allow phase to overrun
    }
    while(phase >= 2*M_PI)
      phase -= 2*M_PI;
  }
  float generate(float fm){
    float sample = sinf(phase);
    phase += incr + fm;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }
  static SineOscillator* create(float sr){
    return new SineOscillator(sr);
  }
  static SineOscillator* create(float freq, float sr){
    return new SineOscillator(freq, sr);
  }
  static void destroy(SineOscillator* osc){
    delete osc;
  }
};

#endif /* SINE_OSCILLATOR_H */
