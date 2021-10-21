#ifndef __COMPLEX_OSCILLATOR_H__
#define __COMPLEX_OSCILLATOR_H__

#include "SignalGenerator.h"
#include "Oscillator.h"

/**
 * A complex oscillator is a MultiSignalGenerator with 2 channels that
 * operates at a given frequency and that can be frequency modulated.
 *
 * A single sample is represented as a ComplexFloat value, while blocks
 * of audio are stored in an AudioBuffer with 2 channels.
 */
class ComplexOscillator : public ComplexSignalGenerator {
public:
  ComplexOscillator() = default;
  virtual ~ComplexOscillator() = default;
  using ComplexSignalGenerator::generate;
  /**
   * Set oscillator sample rate
   */
  virtual void setSampleRate(float value){}
  /**
   * Set oscillator frequency in Hertz
   */
  virtual void setFrequency(float value) = 0;
  /**
   * Get oscillator frequency in Hertz
   */
  virtual float getFrequency() = 0;
  /**
   * Set current oscillator phase in radians
   * @param phase a value between 0 and 2*pi
   */
  virtual void setPhase(float phase) = 0;
  /**
   * Get current oscillator phase in radians
   * @return a value between 0 and 2*pi
   */
  virtual float getPhase() = 0;
  /**
   * Reset oscillator (typically resets phase)
   */
  virtual void reset() = 0;
  /**
   * Produce a sample with frequency modulation.
   */
  virtual ComplexFloat generate(float fm) = 0;
  /**
   * Produce a block of samples with frequency modulation.
   */
  virtual void generate(ComplexFloatArray output, FloatArray fm){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = generate(fm[i]);
  }

};

template<typename OscillatorClass>
using ComplexOscillatorTemplate = OscillatorTemplate<OscillatorClass, ComplexOscillator, ComplexFloat>;
#endif
