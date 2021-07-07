#ifndef __ADSR_ENVELOPE_H
#define __ADSR_ENVELOPE_H

#include "Envelope.h"

/**
 * ADSR Envelope, either linear or exponential
 */
template<bool linear>
class AdsrEnvelope : public Envelope {
protected:
  static const float MINLEVEL;
  enum EnvelopeStage { kAttack, kDecay, kSustain, kRelease, kIdle };
  enum EnvelopeTrigger { kGate, kTrigger };
  float calculateIncrement(float startValue, float endValue, float time);
  float increment(float level, float amount);
  float decrement(float level, float amount);
public:
  AdsrEnvelope(float sampleRate) :
    sampleRate(sampleRate),
    stage(kIdle),
    trig(kGate),
    level(MINLEVEL),
    gateState(false),
    gateTime(-1) {
    setAttack(1/sampleRate);
    setDecay(1/sampleRate);
    setSustain(1.0);
    setRelease(1/sampleRate);
    setRetrigger(false);
  }

  using Envelope::process;
  using Envelope::trigger;
  using SignalGenerator::generate;
  void setSampleRate(float value){
    sampleRate = value;
  }
  void setAttack(float value){
    attackIncrement = calculateIncrement(MINLEVEL, 1, value);
  }
  void setDecay(float value){
    decayIncrement = calculateIncrement(1, MINLEVEL, value);
  }
  void setRelease(float value){
    releaseIncrement = calculateIncrement(1, MINLEVEL, value);
  }
  void setSustain(float newSustain){
    sustain = newSustain;
  }
  void trigger(bool state, int delay){
    gate(state, delay);
    trig = kTrigger;
  }
  void setRetrigger(bool state){
    retrigger = state;
  }
  void gate(bool state){
    gate(state, 0);
  }
  void gate(bool state, int delay){
    if(gateState != state){
      gateTime = delay;
      gateState = state;
    }
    trig = kGate;
  }
  float getLevel(){
    return level;
  }
  void setLevel(float newLevel){
    level = newLevel;
  }
  /**
   * Produce the next envelope sample.
   */
  float generate(){
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
      level = increment(level, attackIncrement);
      if(level >= 1.0){
	level = 1.0;
	stage = kDecay;
      }else if(gateState == false && trig == kGate){
	stage = kRelease;
      }
      break;
    case kDecay:
      // decay ramp
      level = decrement(level, decayIncrement);
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
      level = decrement(level, releaseIncrement);
      if(level <= MINLEVEL){
	level = MINLEVEL;
	if (retrigger == true)
	  trigger();
	else // (retrigger == false)
	  stage = kIdle;
      }else if(gateState == true ){ // if during release the gate is on again, start over from the current level
	stage = kAttack;
      }
      break;
    case kIdle:
      level = MINLEVEL;
      if(gateState == true)
	stage = kAttack;
      break;
    }
    return level;
  }
  [[deprecated("use generate() instead.")]]
  float getNextSample(){
    return generate(); // increments envelope one step
  }  
  [[deprecated("use generate() instead.")]]
  void getEnvelope(FloatArray output){
    generate(output); // increments envelope by output buffer length
  }
  [[deprecated("use process() instead.")]]
  void attenuate(FloatArray buf){
    process(buf, buf); // increments envelope by buffer length
  }
  static AdsrEnvelope<linear>* create(float sampleRate){
    return new AdsrEnvelope<linear>(sampleRate);
  }
  static void destroy(AdsrEnvelope<linear>* env){
    delete env;
  }
protected:
  float sampleRate;
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

template<>
float AdsrEnvelope<true>::increment(float level, float amount){
  return level + amount;
}

template<>
float AdsrEnvelope<true>::decrement(float level, float amount){
  return level + amount;
}

template<>
float AdsrEnvelope<false>::increment(float level, float amount){
  return level + (1.01-level)*amount; // aim slightly higher to ensure we reach 1.0
}

template<>
float AdsrEnvelope<false>::decrement(float level, float amount){
  return level + level * amount;
}

template<>
float AdsrEnvelope<true>::calculateIncrement(float startValue, float endValue, float time){
  return (endValue-startValue)/(sampleRate*time+1);
}

template<>
float AdsrEnvelope<false>::calculateIncrement(float startValue, float endValue, float time) {
  // Ref: Christian Schoenebeck http://www.musicdsp.org/showone.php?id=189
  return (logf(endValue) - logf(startValue)) / (time*sampleRate+10);
}

template<>
const float AdsrEnvelope<true>::MINLEVEL = 0;

template<>
const float AdsrEnvelope<false>::MINLEVEL = 0.00001; // -100dB

typedef AdsrEnvelope<true> LinearAdsrEnvelope;
typedef AdsrEnvelope<false> ExponentialAdsrEnvelope;

#endif /* __ADSR_ENVELOPE_H */
