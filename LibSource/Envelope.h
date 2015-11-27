#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include "FloatArray.h"

/**
 * Linear ADSR Envelope
 */
class AdsrEnvelope {
private:
  enum EnvelopeStage { kAttack, kDecay, kSustain, kRelease, kIdle };
  enum EnvelopeTrigger { kGate, kTrigger, kRetrigger };

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
  void retrigger(bool on, int triggerDelay);
  void gate(bool state);
  void gate(bool state, int gateDelay);
  void setLevel(float newLevel);
  float getNextSample(); // increments envelope one step
  void getEnvelope(FloatArray &output); // increments envelope by output buffer length
  void attenuate(FloatArray &buf); // increments envelope by buffer length
private:
  void updateStage();
  static const float minTime;
  const float samplePeriod;
  EnvelopeStage stage;
  EnvelopeTrigger trig;
  float level;
  float attackIncrement;
  float decayIncrement;
  float releaseIncrement;
  float sustain;
  bool gateState;
  int gateTime;
};

#endif /* ENVELOPE_HPP */
