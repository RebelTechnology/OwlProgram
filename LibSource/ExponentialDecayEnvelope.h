#ifndef EXPONENTIAL_DECAY_ENVELOPE_H
#define EXPONENTIAL_DECAY_ENVELOPE_H

#include "Envelope.h"

class ExponentialDecayEnvelope : public Envelope {
private:
  float sr;
  float value;
  float incr;
public:
  ExponentialDecayEnvelope(float sr=48000): sr(sr), value(0), incr(0){}
  using Envelope::process;
  using SignalGenerator::generate;
  void setSampleRate(float sampleRate){
    sr = sampleRate;
  }
  void setRate(float r){
    if(r < 0)
      incr = 1.0f - 100*(1/(1-r))/sr;
    else
      incr = 1.0f + 100*r/sr;
  }
  void setDecay(float d){
    setRate(-(d+1/sr));
  }
  void trigger(){
    value = 1.0;
  }
  /**
   * Produce the next envelope sample.
   */
  float generate(){
    float sample = value;
    value *= incr;
    return sample;
  }
  [[deprecated("use generate() instead.")]]
  float getNextSample(){
    return generate(); // increments envelope one step
  }  
  static ExponentialDecayEnvelope* create(float sr){
    return new ExponentialDecayEnvelope(sr);
  }
  static ExponentialDecayEnvelope* create(float rate, float sr){
    ExponentialDecayEnvelope* env = new ExponentialDecayEnvelope(sr);
    env->setDecay(rate);
    return env;
  }
  static void destroy(ExponentialDecayEnvelope* env){
    delete env;
  }
};

#endif /* EXPONENTIAL_DECAY_ENVELOPE_H */
