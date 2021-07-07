#ifndef EXPONENTIAL_DECAY_ENVELOPE_H
#define EXPONENTIAL_DECAY_ENVELOPE_H

#include "Envelope.h"

class ExponentialDecayEnvelope : public Envelope {
protected:
  float sr;
  float value;
  float incr;
  float calculateMultiplier(float startLevel, float endLevel, size_t lengthInSamples) {
    // Ref: Christian Schoenebeck http://www.musicdsp.org/showone.php?id=189
    return 1.0 + (logf(endLevel) - logf(startLevel)) / (lengthInSamples+20);
  }
public:
  ExponentialDecayEnvelope(float sr=48000): sr(sr), value(0), incr(0){}
  using Envelope::gate;
  using Envelope::trigger;
  using Envelope::process;
  using SignalGenerator::generate;
  void setSampleRate(float sampleRate){
    sr = sampleRate;
  }
  void setDecaySamples(float samples){ // set decay time in samples
    incr = calculateMultiplier(1, 0.0001, samples);
  }
  void setDecay(float sec){ // set decay time in seconds to reach -80dB
    incr = calculateMultiplier(1, 0.0001, sec*sr);
  }
  void trigger(bool state, int triggerDelay){
    if(state)
      value = 1;
  }
  void gate(bool state, int triggerDelay){
    value = state;
  }
  /**
   * Produce the next envelope sample.
   */
  float generate(){
    float sample = value;
    value *= incr;
    return sample;
  }
  float getLevel(){
    return value;
  }
  void setLevel(float level){
    value = level;
  }
  [[deprecated("use generate() instead.")]]
  float getNextSample(){
    return generate(); // increments envelope one step
  }  
  static ExponentialDecayEnvelope* create(float sr){
    return new ExponentialDecayEnvelope(sr);
  }
  static void destroy(ExponentialDecayEnvelope* env){
    delete env;
  }
};

#endif /* EXPONENTIAL_DECAY_ENVELOPE_H */
