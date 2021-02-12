#ifndef CHIRP_OSCILLATOR_H
#define CHIRP_OSCILLATOR_H

#include "Oscillator.h"

class ChirpOscillator : public Oscillator {
private:
  float phase;
  float incr;
public:
  float rate;
  ChirpOscillator()
    : phase(0.0f), incr(1.0f), rate(0) {}
  using Oscillator::setFrequency;
  void setFrequency(float nfreq){
    incr = nfreq*M_PI;
  }
  void setRate(float r, float sr){
    // should be: rate 0: 1 (constant), rate -1: 1-0.5/sr (halved in a second), rate 1: 1+1/sr (doubled in a second)
    if(r < 0)
      rate = 1.0f - 10*(1/(1-r))/sr;
    else
      rate = 1.0f + 10*(1/(1+r))/sr;
  }
  /**
   * Set expontential decay rate.
   */
  void setDecay(float d, float sr){
    setRate(-(d+1/sr), sr);
  }
  void trigger(){
    reset();
  }
  void reset(){
    phase = 0.0f;
  }
  float generate(){
    float sample = sinf(phase);
    phase += incr;
    incr *= rate;
    // phase %= 2*M_PI;
    // if(phase >= 2*M_PI)
    //   phase -= 2*M_PI;
    return sample;
  }
  float generate(float fm){
    float sample = sinf(phase);
    phase += incr + fm;
    incr *= rate;
    return sample;
  }
  static ChirpOscillator* create(){
    return new ChirpOscillator();
  }
  static void destroy(ChirpOscillator* osc){
    delete osc;
  }
};

#endif /* CHIRP_OSCILLATOR_H */
