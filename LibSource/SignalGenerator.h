#ifndef __SignalGenerator_h__
#define __SignalGenerator_h__

#include "FloatArray.h"
#include "ComplexFloatArray.h"
#include "AudioBuffer.h"

/**
 * Base class for signal generators such as Oscillators.
 * A SignalGenerator produces samples from -1 to 1 unless
 * otherwise stated.
 */
class SignalGenerator {
public:
  virtual ~SignalGenerator(){}
  /**
   * Produce the next consecutive sample.
   */
  virtual float generate() = 0;
  /**
   * Produce a block of samples
   */
  virtual void generate(FloatArray output){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = generate();
  }
};


class MultiSignalGenerator {
public:
  virtual ~MultiSignalGenerator(){}
  virtual void generate(AudioBuffer& output) = 0;
};


/**
 * Base class for stereo signal generators such as Oscillators.
 * A ComplexSignalGenerator produces complex numbers with parts
 * in [-1..1] range unless
 * otherwise stated.
 */
class ComplexSignalGenerator : public MultiSignalGenerator {
public:
  virtual ~ComplexSignalGenerator(){}
  /**
   * Produce the next consecutive sample.
   */
  virtual ComplexFloat generate() = 0;
  /**
   * Produce a block of samples
   */
  virtual void generate(AudioBuffer& output) override{
    size_t size = output.getSize();
    FloatArray left = output.getSamples(0);
    FloatArray right = output.getSamples(1);
    for(size_t i=0; i<size; ++i) {
      ComplexFloat sample = generate();
      left[i] = sample.re;
      right[i] = sample.im;
    }
  }
};
#endif // __SignalGenerator_h__
