#ifndef __PolyBlepOscillator_h__
#define __PolyBlepOscillator_h__

#include "FloatArray.h"
#include "polyblep/dsp/oscillator.h"

class PolyBlepOscillator {
private:
  const float multiplier;
  stmlib::Oscillator osc;
  float frequency, shape, pw;
public:
  PolyBlepOscillator(float sr);
  void setFrequency(float value);
  /** set waveform shape: 0.0 for saw, 1.0 for square wave */
  void setShape(float value);
  /** set square wave pulse width: 0.0 to 1.0 for 0% to 100% */
  void setPulseWidth(float value);
  float getNextSample();
  void getSamples(FloatArray samples);
};

#endif /* __PolyBlepOscillator_h__ */
