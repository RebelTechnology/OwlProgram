#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include "FloatArray.h"
#include "SignalGenerator.h"

/**
 * An Oscillator is a SignalGenerator that operates at a given frequency
 * and that can be frequency modulated.
 */
class Oscillator : public SignalGenerator {
public:
  Oscillator(){}
  virtual ~Oscillator(){}
  using SignalGenerator::generate;
  /**
   * Set oscillator frequency (at given sample rate @param sr)
   */
  void setFrequency(float freq, float sr){
    setFrequency(freq/sr);
  }
  /**
   * Set normalised frequency.
   * Normalised frequency equals frequency divided by sample rate.
   * Values 0 to 0.5 represents 0 Hz to Nyquist frequency.
   */
  virtual void setFrequency(float value){}
  /**
   * Reset oscillator (typically resets phase)
   */
  virtual void reset(){}
  /**
   * Produce a sample with frequency modulation.
   */
  virtual float generate(float fm) = 0;
  /**
   * Produce a block of samples with frequency modulation.
   */
  virtual void generate(FloatArray output, FloatArray fm){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = generate(fm[i]);
  }
  [[deprecated("use generate() instead.")]]
  float getNextSample(){ return generate(); }
  [[deprecated("use generate() instead.")]]
  float getNextSample(float fm){ return generate(fm); }
  [[deprecated("use generate() instead.")]]
  void getSamples(FloatArray output){
    generate(output);
  }
  [[deprecated("use generate() instead.")]]
  void getSamples(FloatArray output, FloatArray fm){
    generate(output, fm);
  }
  [[deprecated("use setFrequency(freq, SR) instead.")]]
  virtual void setSampleRate(float value){}
};

#endif /* OSCILLATOR_HPP */
