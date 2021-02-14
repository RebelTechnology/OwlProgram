#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"
#include "Oscillator.h"

class WavetableOscillator : public Oscillator {
private:
  float mul;
  FloatArray wave;
  float acc;
  float inc;
public:
  WavetableOscillator(float sr, const FloatArray wavetable);
  void setSampleRate(float sr);
  void setFrequency(float freq);
  float getSample(float phase);
  float generate();
  float generate(float fm);
  void generate(FloatArray samples);
  static WavetableOscillator* create(float sr, size_t size);
  static void destroy(WavetableOscillator* osc);
};

#endif /* __WavetableOscillator_h__ */
