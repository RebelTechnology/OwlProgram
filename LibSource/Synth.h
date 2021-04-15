#ifndef __Synth_h__
#define __Synth_h__

#include <stdint.h>

class Synth {
public:
  virtual ~Synth(){}
  virtual void setFrequency(float freq) = 0;
  virtual void setGain(float gain) = 0;
  virtual void gate(bool state) = 0;
  virtual void trigger() = 0;
  /**
   * Set a Synth parameter. The definition of parameter is up to the Synth,
   * but parameter 1 is dedicated to modulation
   */
  virtual void setParameter(uint8_t parameter_id, float value){}
};

#endif // __Synth_h__
