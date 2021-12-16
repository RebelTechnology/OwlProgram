#ifndef __SquareWaveOscillator_h
#define __SquareWaveOscillator_h

#include "Oscillator.h"

class SquareWaveOscillator : public OscillatorTemplate<SquareWaveOscillator> {
protected:
  float pw = 0.5;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  SquareWaveOscillator(){}
  SquareWaveOscillator(float sr){
    setSampleRate(sr);
  }  
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

class InvertedSquareWaveOscillator : public OscillatorTemplate<InvertedSquareWaveOscillator> {
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

class AntialiasedSquareWaveOscillator : public OscillatorTemplate<AntialiasedSquareWaveOscillator> {
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
  
#endif /* __SquareWaveOscillator_h */
