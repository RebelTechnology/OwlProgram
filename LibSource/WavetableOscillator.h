#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"

class WavetableOscillator {
private:
  float multiplier;
  FloatArray wave;
  float acc;
  float inc;
public:
  WavetableOscillator(float sr=48000.0, int size=1024);
  WavetableOscillator(float sr, const FloatArray wavetable);
  void setSampleRate(float value);
  float setFrequency(float freq);
  float getSample(float phase);
  float getNextSample();
  void getSamples(FloatArray samples);
};

#endif /* __WavetableOscillator_h__ */
