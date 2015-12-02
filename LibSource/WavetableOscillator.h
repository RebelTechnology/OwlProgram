#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"

class WavetableOscillator {
private:
  const float multiplier;
  FloatArray wave;
  float acc;
  float inc;
public:
  WavetableOscillator(float sr, int size=1024);
  WavetableOscillator(float sr, const FloatArray wavetable);
  float setFrequency(float freq);
  float getSample(float phase);
  float getNextSample();
  void getSamples(FloatArray samples);
};

#endif /* __WavetableOscillator_h__ */
