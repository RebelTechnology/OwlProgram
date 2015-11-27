#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP
#include "message.h"

/*
void EnvelopeGenerator::calculateMultiplier(double startLevel,
                                            double endLevel,
                                            unsigned long long lengthInSamples) {
    multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}
*/

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
  void trigger(int triggerDelay);
  void retrigger();
  void retrigger(int triggerDelay);
  void gate(bool state);
  void gate(bool state, int gateDelay);
  void setLevel(float newLevel);
  float getNextSample(); // increments envelope one step
  void getEnvelope(FloatArray &output); // increments envelope by output buffer length
  void attenuate(FloatArray &buf); // increments envelope by buffer length
private:
  void updateStage(){
    if(gateState)
      stage = kAttack;
    else if(trig == kGate)
      stage = kRelease;
  }
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

const float AdsrEnvelope::minTime = 0.005;

AdsrEnvelope::AdsrEnvelope(float sampleRate) : 
  samplePeriod(1.0/sampleRate),
  stage(kIdle),
  trig(kGate),
  level(0.0),
  gateState(false),
  gateTime(0) {
  setAttack(0.0);
  setDecay(0.0);
  setSustain(1.0);
  setRelease(0.0);
}

void AdsrEnvelope::setAttack(float newAttack){
  newAttack = newAttack > minTime ? newAttack : minTime;
  attackIncrement = samplePeriod / newAttack;
}

void AdsrEnvelope::setDecay(float newDecay){
  newDecay = newDecay > minTime ? newDecay : minTime;
  decayIncrement = - samplePeriod / newDecay;
}

void AdsrEnvelope::setRelease(float newRelease){
  newRelease = newRelease > minTime ? newRelease : minTime;
  releaseIncrement = - samplePeriod / newRelease;
}

void AdsrEnvelope::setSustain(float newSustain){
  sustain = newSustain;
 // TODO: in the real world, you would probably glide to the new sustain level at a rate determined by either decay or attack
}

void AdsrEnvelope::retrigger(){
  retrigger(0);
}

void AdsrEnvelope::retrigger(int delay){
  gate(true, delay);
  trig = kRetrigger;
}

void AdsrEnvelope::trigger(){
  trigger(0);
}

void AdsrEnvelope::trigger(int delay){
  gate(true, delay);
  trig = kTrigger;
}

void AdsrEnvelope::gate(bool state){
  gate(state, 0);
}


void AdsrEnvelope::gate(bool state, int delay){
  if(gateState != state){
    gateTime = delay;
    gateState = state;
    if(gateTime == 0)
      updateStage();
  }
  trig = kGate;
}

void AdsrEnvelope::setLevel(float newLevel){
  level = newLevel;
}

void AdsrEnvelope::attenuate(FloatArray &output){
  for(int n = 0; n < output.getSize(); n++)
    output[n] *= getNextSample();
}

void AdsrEnvelope::getEnvelope(FloatArray &output){
  for(int n = 0; n < output.getSize(); n++)
    output[n] = getNextSample();
}

float AdsrEnvelope::getNextSample(){
  if(gateTime && --gateTime == 0)
    updateStage();
  switch (stage) {
  case kAttack:
    // attack ramp
    level += attackIncrement;
    if(level >= 1.0){
      level = 1.0;
      stage = kDecay;
    }else if(gateState == false && trig == kGate){
      stage = kRelease;
    }
    break;
  case kDecay:
    // decay ramp
    level += decayIncrement;
    if(level <= sustain){
      level = sustain;
      stage = kSustain;
    }else if(gateState == false && trig == kGate){
      stage = kRelease;
    }
    break;
  case kSustain:
    level = sustain;
    if(gateState == false)
      stage = kRelease;
    break;
  case kRelease:
    // release ramp
    level += releaseIncrement;
    if(level <= 0.0){
      level = 0.0;
      stage = kIdle;
    }else if(gateState == true){
      stage = kAttack;
    }
    break;
  case kIdle:
    level = 0.0;
    if(gateState == true || trig == kRetrigger)
      stage = kAttack;
    break;
  }
  return level;
}

#endif /* ENVELOPE_HPP */
