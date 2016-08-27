#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"
#include "Oscillator.h"

class WavetableOscillator : public Oscillator {
private:
  float timeBaseOverFs;
  FloatArray wave;
  int32_t acc;
  int32_t inc;
  int32_t intSize;
  const unsigned short fracBits = 16;
  const uint32_t floatToPhase = 1 << fracBits;
  float freq;
  float getCurrentSample();
public:
  WavetableOscillator(const FloatArray wavetable);
  WavetableOscillator(unsigned int timeBase, const FloatArray wavetable);
  void setFrequency(float newFreq);
  float getSample(float phase);
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
  static float interpolate(float x0, float x1, float frac){
    return x0 * (1 - frac) + x1 * frac;
  }
  SmoothWavetableOscillator(float sr, const FloatArray wavetable);
  // should include a check of the table size and content
  void setTable(const FloatArray wavetable);
  void setFrequency(float freq);
  float getSample(float phase);
  float getNextSample();
  void getSamples(FloatArray samples);
};
#endif /* __WavetableOscillator_h__ */
