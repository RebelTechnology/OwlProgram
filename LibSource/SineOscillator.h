#ifndef SINE_OSCILLATOR_H
#define SINE_OSCILLATOR_H

#include "Oscillator.h"

class SineOscillator : public OscillatorTemplate<SineOscillator> {
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 2*M_PI;
  SineOscillator(){}
  SineOscillator(float sr){
    setSampleRate(sr);
  }  
  float getSample(){
    return sinf(phase);
  }
  void generate(FloatArray output){
    size_t len = output.getSize();
    float* out = output.getData();
    const float incr = this->incr;
    float phase = this->phase;
    for(size_t i=0; i<len; ++i){
      out[i] = sinf(phase);
      phase += incr; // allow phase to overrun
    }
    this->phase = fmodf(phase, end_phase);
  }
  void generate(FloatArray output, FloatArray fm){
    size_t len = output.getSize();
    float* out = output.getData();
    const float* fmd = fm.getData();
    const float incr = this->incr;
    float phase = this->phase;
    for(size_t i=0; i<len; ++i){
      out[i] = sinf(phase);
      phase += incr * (1 + fmd[i]);
      // allow phase to overrun
    }
    this->phase = fmodf(phase, end_phase);
  }
  using OscillatorTemplate<SineOscillator>::generate;  
};

#endif /* SINE_OSCILLATOR_H */
