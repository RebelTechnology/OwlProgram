#ifndef __RampOscillator_h
#define __RampOscillator_h

#include "Oscillator.h"

/**
 * Ramp oscillator generates rising output values from -1 to 1.
 */
class RampOscillator : public OscillatorTemplate<RampOscillator> {
public:
  static constexpr float begin_phase = -1;
  static constexpr float end_phase = 1;
  RampOscillator(){}
  RampOscillator(float sr){
    setSampleRate(sr);
  }  
  float getSample(){
    return phase;
  }
};

/**
 * Inverted ramp oscillator generates falling output values from 1 to -1.
 */
class InvertedRampOscillator : public OscillatorTemplate<InvertedRampOscillator> {
public:
  static constexpr float begin_phase = -1;
  static constexpr float end_phase = 1;
  InvertedRampOscillator(){}
  InvertedRampOscillator(float sr){
    setSampleRate(sr);
  }  
  float getSample(){
    return -phase;
  }
};

class AntialiasedRampOscillator : public OscillatorTemplate<AntialiasedRampOscillator> {
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  float getSample(){
    float sample = 2*phase-1; // naive ramp
    sample -= polyblep(phase, incr);
    return sample;
  }
};
  
#endif /* __RampOscillator_h */
