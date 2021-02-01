#ifndef __PatchProcessor_h__
#define __PatchProcessor_h__

#include <stdint.h>
#include "Patch.h"
#include "device.h"

class ParameterUpdater {
public:
  virtual ~ParameterUpdater(){}
  virtual void update(int16_t value) = 0;
  virtual void setParameter(IntParameter* p){}
  virtual void setParameter(FloatParameter* p){}
};

class PatchProcessor {
public:  
  PatchProcessor();
  ~PatchProcessor();
  void clear();
  void setPatch(Patch* patch, const char* name);
  const char* getPatchName(){
    return name;
  }
  int getBlockSize();
  double getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  void setParameterValues(int16_t* parameters);
  Patch* patch;
  uint8_t index;
  void setPatchParameter(int pid, FloatParameter* param);
  void setPatchParameter(int pid, IntParameter* param);
  template<typename T>
  PatchParameter<T> getParameter(const char* name, T min, T max, T defaultValue, float lambda, float delta, float skew);
private:
  void setDefaultValue(int pid, float value);
  void setDefaultValue(int pid, int value);
  uint8_t bufferCount;
  ParameterUpdater* parameters[MAX_NUMBER_OF_PARAMETERS];
  uint8_t parameterCount;
  AudioBuffer* buffers[MAX_BUFFERS_PER_PATCH];
  const char* name;
};


#endif // __PatchProcessor_h__

