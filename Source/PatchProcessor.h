#ifndef __PatchProcessor_h__
#define __PatchProcessor_h__

#include <stdint.h>
#include "StompBox.h"
#include "device.h"

class ParameterUpdater {
public:
  virtual ~ParameterUpdater(){}
  virtual void update(uint16_t value) = 0;
  virtual void setParameter(IntParameter* p){}
  virtual void setParameter(FloatParameter* p){}
};

class PatchProcessor {
public:  
  PatchProcessor();
  ~PatchProcessor();
  void clear();
  void setPatch(Patch* patch);
  int getBlockSize();
  double getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  void setParameterValues(uint16_t *parameters);
  Patch* patch;
  uint8_t index;
  void setPatchParameter(int pid, FloatParameter* param);
  void setPatchParameter(int pid, IntParameter* param);

  template<typename T>
  PatchParameter<T> getParameter(const char* name, T min, T max, T defaultValue, float lambda, float delta, float skew);
private:
  uint8_t bufferCount;
  ParameterUpdater* parameters[NOF_ADC_VALUES];
  uint8_t parameterCount;
  AudioBuffer* buffers[MAX_BUFFERS_PER_PATCH];
};


#endif // __PatchProcessor_h__

