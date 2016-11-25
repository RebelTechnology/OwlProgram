#ifndef CHIRP_OSCILLATOR_H
#define CHIRP_OSCILLATOR_H

#include "Oscillator.h"

class ChirpOscillator : public Oscillator {
private:
  const float fs;
  float phase;
  float incr;
public:
  float rate;
  ChirpOscillator(float sr)
    : fs(sr), phase(0.0f), incr(1.0f){}
  void setFrequency(float freq){
    incr = freq*2*M_PI/fs;
  }
  void setRate(float r){
    // should be: rate 0: 1 (constant), rate -1: 1-0.5/sr (halved in a second), rate 1: 1+1/sr (doubled in a second)
    if(r < 0)
      rate = 1.0f - 10*(1/(1-r))/fs;
    else
      rate = 1.0f + 10*(1/(1+r))/fs;
  }
  /**
   * Set expontential decay rate.
   */
  void setDecay(float d){
    setRate(-(d+1/fs));
  }
  void trigger(){
    phase = 0.0f;
  }
  float getNextSample(){
    float sample = sinf(phase);
    phase += incr;
    incr *= rate;
    // phase %= 2*M_PI;
    // if(phase >= 2*M_PI)
    //   phase -= 2*M_PI;
    return sample;
  }
  static ChirpOscillator* create(float sr){
    return new ChirpOscillator(sr);
  }
  static void destroy(ChirpOscillator* osc){
    delete osc;
  }
};

#endif /* CHIRP_OSCILLATOR_H */
