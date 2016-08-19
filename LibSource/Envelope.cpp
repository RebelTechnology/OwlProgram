#include "Envelope.h"
#include "message.h"

/*
void EnvelopeGenerator::calculateMultiplier(double startLevel,
                                            double endLevel,
                                            unsigned long long lengthInSamples) {
    multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}
*/

const float AdsrEnvelope::minTime = 0.001;

AdsrEnvelope::AdsrEnvelope() : AdsrEnvelope(1) {}

AdsrEnvelope::AdsrEnvelope(unsigned int timeBase) : 
  stage(kIdle),
  trig(kGate),
  level(0.0),
  gateState(false),
  gateTime(-1)
{
  setTimeBase(timeBase);
  setAttack(0.0);
  setDecay(0.0);
  setSustain(1.0);
  setRelease(0.0);
  setRetrigger(false);
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

void AdsrEnvelope::setTimeBase(unsigned int newTimeBase){
  attackIncrement = attackIncrement * newTimeBase / timeBase;
  decayIncrement = decayIncrement * newTimeBase / timeBase;
  releaseIncrement = releaseIncrement * newTimeBase / timeBase;
  timeBase = newTimeBase;
  samplePeriod = timeBase / Patch::getSampleRate();
}

void AdsrEnvelope::setLevel(float newLevel){
  level = newLevel;
}

void AdsrEnvelope::attenuate(FloatArray output){
  for(int n = 0; n < output.getSize(); n++)
    output[n] *= getNextSample();
}

void AdsrEnvelope::getEnvelope(FloatArray output){
  for(int n = 0; n < output.getSize(); n++)
    output[n] = getNextSample();
}

float AdsrEnvelope::getNextSample(){
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
