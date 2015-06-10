#ifndef __PatchProcessor_h__
#define __PatchProcessor_h__

#include <stdint.h>
#include "StompBox.h"
#include "device.h"

class PatchProcessor {
public:  
  PatchProcessor();
  ~PatchProcessor();
  void clear();
  void setPatch(Patch* patch);
  float getParameterValue(PatchParameterId pid);
  int getBlockSize();
  double getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  void setParameterValues(uint16_t *parameters);
  Patch* patch;
  uint8_t index;
private:
  uint8_t bufferCount;
  uint16_t parameterValues[NOF_ADC_VALUES];
  AudioBuffer* buffers[MAX_BUFFERS_PER_PATCH];
};

#endif // __PatchProcessor_h__
