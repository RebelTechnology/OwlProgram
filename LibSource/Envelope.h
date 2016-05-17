#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include "FloatArray.h"

class Envelope {
public:
  virtual void trigger(){
    trigger(true, 0);
  }
  virtual void trigger(bool state){
    trigger(state, 0);
  }
  virtual void trigger(bool state, int triggerDelay){}
  virtual void gate(bool state){
    gate(state, 0);
  }
  virtual void gate(bool state, int gateDelay){}
};

/**
 * Linear ADSR Envelope
 */
class AdsrEnvelope : public Envelope {
private:
  enum EnvelopeStage { kAttack, kDecay, kSustain, kRelease, kIdle };
  enum EnvelopeTrigger { kGate, kTrigger };

public:
  AdsrEnvelope(float newSampleRate);
  void setAttack(float newAttack);
  void setDecay(float newDecay);
  void setRelease(float newRelase);
  void setSustain(float newSustain);
  void trigger();
  void trigger(bool state);
  void trigger(bool state, int triggerDelay);
  void setRetrigger(bool on);
  void gate(bool state);
  void gate(bool state, int gateDelay);
  void setLevel(float newLevel);
  float getNextSample(); // increments envelope one step
  void getEnvelope(FloatArray output); // increments envelope by output buffer length
  void attenuate(FloatArray buf); // increments envelope by buffer length
private:
  static const float minTime;
  const float samplePeriod;
  EnvelopeStage stage;
  EnvelopeTrigger trig;
  bool retrigger;
  float level;
  float attackIncrement;
  float decayIncrement;
  float releaseIncrement;
  float sustain;
  bool gateState;
  int gateTime;
};

#endif /* ENVELOPE_HPP */
