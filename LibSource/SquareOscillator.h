#ifndef __SquareOscillator_h
#define __SquareOscillator_h

#include "Oscillator.h"

class SquareOscillator : public OscillatorTemplate<SquareOscillator> {
protected:
  float pw = 0.5;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  /**
   * Set pulse width to a value between 0 and 1
   */
  void setPulseWidth(float value){
    pw = value;
  }
  float getSample(){
    return phase < pw ? 1 : -1;
  }
};

class InvertedSquareOscillator : public OscillatorTemplate<InvertedSquareOscillator> {
protected:
  float pw = 0.5;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  /**
   * Set pulse width to a value between 0 and 1
   */
  void setPulseWidth(float value){
    pw = value;
  }
  float getSample(){
    return phase < pw ? -1 : 1;
  }
};

class AntialiasedSquareOscillator : public OscillatorTemplate<AntialiasedSquareOscillator> {
protected:
  float pw = 0.5f;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  /**
   * Set pulse width to a value between 0 and 1
   */
  void setPulseWidth(float value){
    pw = value;
  }
  float getSample(){
    float sample = phase < pw ? 1 : -1;
    sample += polyblep(incr, phase);
    sample -= polyblep(incr, fmod(phase + 1 + pw, 2));
    return sample;
  }
};
  
#endif /* __SquareOscillator_h */
