#ifndef __AudioBuffer_h__
#define __AudioBuffer_h__

#include "FloatArray.h"

class AudioBuffer {
public:
  virtual ~AudioBuffer();
  virtual FloatArray getSamples(int channel) = 0;
  virtual int getChannels() = 0;
  virtual int getSize() = 0;
  virtual void clear() = 0;
  virtual void add(AudioBuffer& buffer) = 0;
  static AudioBuffer* create(int channels, int samples);
  static void destroy(AudioBuffer* buffer);
};

#endif // __AudioBuffer_h__
