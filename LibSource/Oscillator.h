#ifndef __Oscillator_h
#define __Oscillator_h

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
protected:
  static float polyblep(float t, float dt){
    if(t < dt){ // 0 <= t < 1
      t /= dt;
      return t+t - t*t - 1;
    }else if(t > 1.0 - dt){ // -1 < t < 0      
      t = (t - 1.0) / dt;
      return t*t + t+t + 1;
    }
    // 0 otherwise
    return 0;
  }
};

#endif /* __Oscillator_h */
