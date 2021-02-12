#ifndef __PolyBlepOscillator_h__
#define __PolyBlepOscillator_h__

#include "FloatArray.h"
#include "polyblep/dsp/oscillator.h"
#include "Oscillator.h"

class PolyBlepOscillator : public Oscillator {
private:
  stmlib::Oscillator osc;
  float frequency, shape, pw;
public:
  PolyBlepOscillator();
  PolyBlepOscillator(float freq, float sr);
  using Oscillator::setFrequency;
  void setFrequency(float value);
  /** set waveform shape: 0.0 for saw, 1.0 for square wave */
  void setShape(float value);
  /** set square wave pulse width: 0.0 to 1.0 for 0% to 100% */
  void setPulseWidth(float value);
  float generate();
  float generate(float fm);
  /* put a block of output samples into @param output */
  void generate(FloatArray output);
  /* put a block of output samples into @param output, frequency modulated by @param fm */
  void generate(FloatArray output, FloatArray fm);
  /* put a block of output samples into @param samples, 
     with normalised frequency determined by samples in @param frequency */
  void getSamples(FloatArray output, FloatArray nfreq);
  static PolyBlepOscillator* create();
  static PolyBlepOscillator* create(float freq, float sr);
  static void destroy(PolyBlepOscillator* osc);
};

#endif /* __PolyBlepOscillator_h__ */
