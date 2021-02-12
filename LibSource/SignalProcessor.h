#ifndef __SignalProcessor_h__
#define __SignalProcessor_h__

#include "FloatArray.h"

/**
 * Base class for signal processors such as Filters
 */
class SignalProcessor {
public:
  virtual void process(FloatArray input, FloatArray output) = 0;
};

#endif // __SignalProcessor_h__
