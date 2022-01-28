#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "Oscillator.h"

class WavetableOscillator : public OscillatorTemplate<WavetableOscillator> {
private:
  FloatArray wave;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  WavetableOscillator(){}
  WavetableOscillator(float sr){
    setSampleRate(sr);
  }
  WavetableOscillator(float sr, const FloatArray wavetable): wave(wavetable) {
    setSampleRate(sr);
  }
  FloatArray getWavetable(){
    return wave;
  }    
  float getSample(){
    size_t index = phase*wave.getSize();
    // index = min(index, size-1);
    return wave[index];
  }
};


class AntialiasedWavetableOscillator : public OscillatorTemplate<AntialiasedWavetableOscillator> {
private:
  FloatArray wave;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  AntialiasedWavetableOscillator(){}
  AntialiasedWavetableOscillator(float sr){
    setSampleRate(sr);
  }  
  AntialiasedWavetableOscillator(float sr, FloatArray wave):wave(wave){
    setSampleRate(sr);
  }  
  float getSample(){
    size_t index = phase*wave.getSize();
    float sample = wave[index];
    sample -= polyblep(phase, incr);
    return sample;
  }
  using OscillatorTemplate<AntialiasedWavetableOscillator>::generate;  
};

#endif /* __WavetableOscillator_h__ */
