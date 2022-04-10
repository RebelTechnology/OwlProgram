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

template<InterpolationMethod im = LINEAR_INTERPOLATION>
class InterpolatingWavetableOscillator : public OscillatorTemplate<InterpolatingWavetableOscillator<im>> {
private:
  InterpolatingCircularFloatBuffer<im> wave = NULL;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  InterpolatingWavetableOscillator(){}
  InterpolatingWavetableOscillator(float sr){
    OscillatorTemplate<InterpolatingWavetableOscillator<im>>::setSampleRate(sr);
  }
  InterpolatingWavetableOscillator(float sr, FloatArray data)
    : wave(data.getData(), data.getSize()) {
    OscillatorTemplate<InterpolatingWavetableOscillator<im>>::setSampleRate(sr);
  }
  FloatArray getWavetable(){
    return FloatArray(wave.getData(), wave.getSize());
  }
  void setWavetable(FloatArray wavetable){
    wave.setData(wavetable.getData(), wavetable.getSize());
  }
  float getSample(){
    float index = OscillatorTemplate<InterpolatingWavetableOscillator<im>>::phase * wave.getSize();
    return wave.readAt(index);
  }
  static InterpolatingWavetableOscillator<im>* create(float sr, size_t length){
    return new InterpolatingWavetableOscillator<im>(sr, FloatArray::create(length));
  }
  static void destroy(InterpolatingWavetableOscillator<im>* obj){
    FloatArray::destroy(obj->getWavetable());
    delete obj;
  }
};

#endif /* __WavetableOscillator_h__ */
