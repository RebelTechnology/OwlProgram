#ifndef __Synth_h__
#define __Synth_h__

#include "SignalGenerator.h"

class Synth : public SignalGenerator {
public:
  virtual ~Synth(){}
  // pure abstract methods that must be implemented by a derived class
  virtual void setFrequency(float freq) = 0;
  virtual void setGain(float gain) = 0;
  virtual void gate(bool state) = 0;
  virtual void trigger() = 0;
};

#endif // __Synth_h__
