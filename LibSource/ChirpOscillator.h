#ifndef CHIRP_OSCILLATOR_H
#define CHIRP_OSCILLATOR_H

#include "Oscillator.h"

class ChirpOscillator : public Oscillator {
private:
  float sr;
  float phase;
  float incr;
  float basefreq;
  float rate;
public:
  ChirpOscillator(float sr=48000)
    : sr(sr), phase(0), incr(0), basefreq(0), rate(0) {}
  void setFrequency(float freq){
    incr = freq*2*M_PI/sr;
    basefreq = freq;
  }
  float getFrequency(){
    return basefreq;
  }
  void setSampleRate(float sample_rate){
    sr = sample_rate;
  }
  float getSampleRate(){
    return sr;
  }
  /**
   * Set rate of change: positive for ascending chirp, negative for descending.
   * +1 doubles frequency in one second
   * -1 halves frequency in one second
   */
  void setRate(float r){
    rate = 1 + r/sr;
  }
  void trigger(){
    incr = basefreq*2*M_PI/sr;
    reset();
  }
  void reset(){
    phase = 0.0f;
  }
  float getPhase(){
    return phase;
  }
  void setPhase(float phase){
    this->phase = phase;
  }
  float generate(){
    float sample = sinf(phase);
    phase += incr;
    incr *= rate;
    if(phase >= 2*M_PI)
      phase -= 2*M_PI;
    return sample;
  }
  float generate(float fm){
    float sample = sinf(phase);
    phase += incr + fm;
    incr *= rate;
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
