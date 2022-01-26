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
    sample += polyblep(phase, incr);
    sample -= polyblep(fmod(phase + 1 - pw, 1), incr);
    return sample;
  }
  void generate(FloatArray output){
    float sample;
    if(phase < pw){
      sample = 1;
      if(phase < incr){
	float t = phase / incr;
	sample += t+t - t*t - 1;
      }
    }else{
      sample = -1;
      if(phase-pw < incr){
	float t = fmod(phase + 1 - pw, 1) / incr;
	sample -= t+t - t*t - 1;
      }
    }
    size_t len = output.getSize();
    float* dest = output.getData();
    while(len--){
      phase += incr;
      if(phase >= pw){
	if(phase >= 1){
	  // wrap phase
	  phase -= 1;
	  // correct current sample
	  float t = (phase - incr) / incr;
	  sample += t*t + t+t + 1;
	  *dest++ = sample;
	  sample = 1;
	  // correct next sample
	  t = phase / incr;
	  sample += t+t - t*t - 1;
	}else if(sample == 1){
	  // correct current sample
	  float t = (fmod(phase + 1 - pw, 1) - incr) / incr;
	  sample -= t*t + t+t + 1;
	  *dest++ = sample;
	  sample = -1;
	  // correct next sample
	  t = fmod(phase + 1 - pw, 1) / incr;
	  sample -= t+t - t*t - 1;
	}else{
	  *dest++ = sample;
	  sample = -1;
	}
      }else{
	*dest++ = sample;
	sample = 1;
      }
    }
  }
  using OscillatorTemplate<AntialiasedSquareWaveOscillator>::generate;  
};
  
#endif /* __SquareWaveOscillator_h */
