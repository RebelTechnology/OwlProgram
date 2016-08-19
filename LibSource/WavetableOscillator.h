#ifndef __WavetableOscillator_h__
#define __WavetableOscillator_h__

#include "FloatArray.h"
#include "Oscillator.h"

class WavetableOscillator : public Oscillator {
private:
  float timeBaseOverFs;
  FloatArray wave;
  float acc;
  float inc;
  float freq;
public:
  WavetableOscillator(const FloatArray wavetable);
  WavetableOscillator(unsigned int timeBase, const FloatArray wavetable);
  void setFrequency(float newFreq);
  float getSample(float phase);
  float getNextSample();
  void setTimeBase(unsigned int samples);
  void setTable(FloatArray table);
  FloatArray getTable();
  static WavetableOscillator* create(int size);
  static void destroy(WavetableOscillator* wavetableOscillator);
};

/*
class BandPassWavetableOscillator : public Oscillator {
public:
  BandPassWavetableOscillator(float sr, const FloatArray wavetable);
  void setTable(const FloatArray wavetable);
  void setFrequency(float freq);
  float getSample(float phase);
  float getNextSample();
  void getSamples(FloatArray samples);

};
*/
#endif /* __WavetableOscillator_h__ */
