#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"
#include "Oscillator.h"

class WavetableOscillator : public Oscillator {
private:
  virtual float getCurrentSample();
protected:
  float timeBaseOverFs;
  int32_t acc;
  int32_t inc;
  const unsigned short fracBits = 16;
  float freq;
  int32_t intSize;
  FloatArray wave;
public:
  WavetableOscillator();
  WavetableOscillator(const FloatArray wavetable);
  WavetableOscillator(unsigned int timeBase, const FloatArray wavetable);
  void setFrequency(float newFreq);
  float getNextSample();
  float getNextSample(float fm);
  void setTimeBase(unsigned int samples);
  void setTable(FloatArray table);
  FloatArray getTable();
  static WavetableOscillator* create(int size);
  static void destroy(WavetableOscillator* wavetableOscillator);
};

class SmoothWavetableOscillator : public WavetableOscillator {
private:
  float getCurrentSample();
public:
  using WavetableOscillator::WavetableOscillator;
  static float interpolate(float x0, float x1, float frac){
    return x0 * (1 - frac) + x1 * frac;
  }
  // should include a check of the table size and content
  void setTable(const FloatArray wavetable);
};
#endif /* __WavetableOscillator_h__ */
