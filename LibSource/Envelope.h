#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include "FloatArray.h"
#include "SignalGenerator.h"
#include "SignalProcessor.h"

class Envelope : public SignalGenerator, SignalProcessor {
public:
  using SignalGenerator::generate;
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
  virtual float process(float input){
    return input*generate();
  }
  /**
   * Attenuate samples in @param input by envelope and place results in @param output
   */
  virtual void process(FloatArray input, FloatArray output){
    for(size_t n = 0; n < output.getSize(); n++)
      output[n] = input[n]*generate();
  }
};

/**
 * Linear ADSR Envelope
 */
class AdsrEnvelope : public Envelope {
private:
  enum EnvelopeStage { kAttack, kDecay, kSustain, kRelease, kIdle };
  enum EnvelopeTrigger { kGate, kTrigger };

public:
  AdsrEnvelope(float sampleRate=48000);
  virtual ~AdsrEnvelope();
  using Envelope::process;
  using SignalGenerator::generate;
  void setSampleRate(float sampleRate);
  void setAttack(float newAttack);
  void setDecay(float newDecay);
  void setRelease(float newRelease);
  void setSustain(float newSustain);
  void trigger();
  void trigger(bool state);
  void trigger(bool state, int triggerDelay);
  void setRetrigger(bool on);
  void gate(bool state);
  void gate(bool state, int gateDelay);
  float getLevel();
  void setLevel(float newLevel);
  /**
   * Produce the next envelope sample.
   */
  float generate();
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
  static AdsrEnvelope* create(float sampleRate){
    return new AdsrEnvelope(sampleRate);
  }
  static AdsrEnvelope* create(float a, float d, float s, float r, float sampleRate){
     AdsrEnvelope* env = new AdsrEnvelope(sampleRate);
     env->setAttack(a);
     env->setDecay(d);
     env->setSustain(s);
     env->setRelease(r);
     return env;
  }
  static void destroy(AdsrEnvelope* env){
    delete env;
  }
private:
  float samplePeriod;
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
