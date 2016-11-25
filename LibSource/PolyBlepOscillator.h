#ifndef __PolyBlepOscillator_h__
#define __PolyBlepOscillator_h__

#include "FloatArray.h"
#include "polyblep/dsp/oscillator.h"
#include "Oscillator.h"

class PolyBlepOscillator : public Oscillator {
private:
  float multiplier;
  stmlib::Oscillator osc;
  float frequency, shape, pw;
public:
  PolyBlepOscillator(float sr=48000.0);
  void setSampleRate(float value);
  void setFrequency(float value);
  /** set waveform shape: 0.0 for saw, 1.0 for square wave */
  void setShape(float value);
  /** set square wave pulse width: 0.0 to 1.0 for 0% to 100% */
  void setPulseWidth(float value);
  float getNextSample();
  /* put a block of output samples into @param output */
  void getSamples(FloatArray output);
  /* put a block of output samples into @param samples, 
     with frequency determined by samples in @param frequency */
  void getSamples(FloatArray output, FloatArray frequency);
  static PolyBlepOscillator* create(float sr);
  static void destroy(PolyBlepOscillator* osc);
};

#endif /* __PolyBlepOscillator_h__ */
