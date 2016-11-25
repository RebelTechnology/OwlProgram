#ifndef EXPONENTIAL_DECAY_ENVELOPE_H
#define EXPONENTIAL_DECAY_ENVELOPE_H

#include "Envelope.h"

class ExponentialDecayEnvelope : public Envelope {
private:
  const float fs;
  float value;
  float incr;
public:
  ExponentialDecayEnvelope(float sr)
    : fs(sr){}
  void setRate(float r){
    if(r < 0)
      incr = 1.0f - 100*(1/(1-r))/fs;
    else
      incr = 1.0f + 100*r/fs;
  }
  void setDecay(float d){
    setRate(-(d+1/fs));
  }
  void trigger(){
    value = 1.0;
  }
  float getNextSample(){
    float sample = value;
    value *= incr;
    return sample;
  }
  static ExponentialDecayEnvelope* create(float sr){
    return new ExponentialDecayEnvelope(sr);
  }
  static void destroy(ExponentialDecayEnvelope* env){
    delete env;
  }
};

#endif /* EXPONENTIAL_DECAY_ENVELOPE_H */
