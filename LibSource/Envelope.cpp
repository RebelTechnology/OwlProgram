#include "Envelope.h"
#include "message.h"

/*
void EnvelopeGenerator::calculateMultiplier(double startLevel,
                                            double endLevel,
                                            unsigned long long lengthInSamples) {
    multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}
*/

AdsrEnvelope::AdsrEnvelope(float sampleRate) :
  samplePeriod(1.0/sampleRate),
  stage(kIdle),
  trig(kGate),
  level(0.0),
  gateState(false),
  gateTime(-1) {
  setAttack(0.0);
  setDecay(0.0);
  setSustain(1.0);
  setRelease(0.0);
  setRetrigger(false);
}

AdsrEnvelope::~AdsrEnvelope(){}

void AdsrEnvelope::setSampleRate(float sampleRate){
  samplePeriod = 1.0/sampleRate;
}

void AdsrEnvelope::setAttack(float newAttack){
  if(newAttack > 0)
    attackIncrement = samplePeriod / newAttack;
  else
    attackIncrement = 1;
}

void AdsrEnvelope::setDecay(float newDecay){
  if(newDecay > 0)
    decayIncrement = - samplePeriod / newDecay;
  else
    decayIncrement = - 1;
}

void AdsrEnvelope::setRelease(float newRelease){
  if(newRelease > 0)
    releaseIncrement = - samplePeriod / newRelease;
  else
    releaseIncrement = -1;
}

void AdsrEnvelope::setSustain(float newSustain){
  sustain = newSustain;
}

void AdsrEnvelope::setRetrigger(bool state){
  retrigger = state;
}

void AdsrEnvelope::trigger(){
  trigger(true, 0);
}

void AdsrEnvelope::trigger(bool state){
  trigger(state, 0);
}

void AdsrEnvelope::trigger(bool state, int delay){
  gate(state, delay);
  trig = kTrigger;
}

void AdsrEnvelope::gate(bool state){
  gate(state, 0);
}

void AdsrEnvelope::gate(bool state, int delay){
  if(gateState != state){
    gateTime = delay;
    gateState = state;
  }
  trig = kGate;
}

float AdsrEnvelope::getLevel(){
  return level;
}

void AdsrEnvelope::setLevel(float newLevel){
  level = newLevel;
}

float AdsrEnvelope::generate(){
  if(gateTime == 0){
    stage = kAttack;
    if(trig == kTrigger){
      gateState = false;
    }
  }
  if(gateTime >= 0){
    gateTime--; // this will stop at -1
  }
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
      if(trig == kGate){
        stage = kSustain;
      } else { // (trig == kTrigger)
        stage = kRelease;
      }
    } else if(gateState == false && trig == kGate){
      stage = kRelease;
    }
    break;
  case kSustain:
    level = sustain;
    if(gateState == false){
      stage = kRelease;
    }
    break;
  case kRelease:
    // release ramp
    level += releaseIncrement;
    if(level <= 0.0){
      level = 0.0;
      if (retrigger == true)
        trigger();
      else // (retrigger == false)
        stage = kIdle;
    }else if(gateState == true ){ // if during release the gate is on again, start over from the current level
      stage = kAttack;
    }
    break;
  case kIdle:
    level = 0.0;
    if(gateState == true)
      stage = kAttack;
    break;
  }
  return level;
}
