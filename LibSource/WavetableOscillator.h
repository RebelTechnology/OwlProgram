#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"
#include "Oscillator.h"

class WavetableOscillator : public Oscillator {
private:
  FloatArray wave;
  float acc;
  float inc;
public:
  WavetableOscillator(const FloatArray wavetable);
  using Oscillator::setFrequency;
  void setFrequency(float freq);
  float getSample(float phase);
  float generate();
  float generate(float fm);
  void generate(FloatArray samples);
  static WavetableOscillator* create(size_t size);
  static void destroy(WavetableOscillator* osc);
};

class BandPassWavetableOscillator : public Oscillator {
public:
  BandPassWavetableOscillator(const FloatArray wavetable);
  void setFrequency(float freq);
  float getSample(float phase);
  float generate();
  void generate(FloatArray samples);

};

#endif /* __WavetableOscillator_h__ */
