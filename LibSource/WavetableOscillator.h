#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"
#include "Oscillator.h"

class WavetableOscillator : public Oscillator {
private:
  float multiplier;
  FloatArray wave;
  float acc;
  float inc;
public:
  WavetableOscillator(float sr, const FloatArray wavetable);
  void setSampleRate(float value);
  void setFrequency(float freq);
  float getSample(float phase);
  float getNextSample();
  void getSamples(FloatArray samples);
  static WavetableOscillator* create(float sr, int size);
};

class BandPassWavetableOscillator : public Oscillator {
public:
  BandPassWavetableOscillator(float sr, const FloatArray wavetable);
  void setSampleRate(float value);
  void setFrequency(float freq);
  float getSample(float phase);
  float getNextSample();
  void getSamples(FloatArray samples);

};

#endif /* __WavetableOscillator_h__ */
