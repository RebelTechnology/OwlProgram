#ifndef __SAMPLEBUFFER_H__
#define __SAMPLEBUFFER_H__

// https://stackoverflow.com/questions/41675438/fastest-way-to-swap-alternate-bytes-on-arm-cortex-m4-using-gcc

#include <stdint.h>
#include <string.h>
#include "Patch.h"
#include "device.h"
#ifdef ARM_CORTEX
#include "arm_math.h"
#define AUDIO_SATURATE_SAMPLES
#else
#undef AUDIO_SATURATE_SAMPLES
#endif //ARM_CORTEX

#define MULTIPLIER_31B 0x80000000
#define MULTIPLIER_30B 0x40000000
#define MULTIPLIER_23B 0x00800000

class SampleBuffer : public AudioBuffer {
protected:
  FloatArray* buffers;
  size_t blocksize;
  size_t channels;
public:
  SampleBuffer(size_t channels, size_t blocksize)
    :channels(channels), blocksize(blocksize) {
    buffers = new FloatArray[channels];
    for(size_t i=0; i<channels; ++i)
      buffers[i] = FloatArray::create(blocksize);
  }
  ~SampleBuffer(){
    for(size_t i=0; i<channels; ++i)
      FloatArray::destroy(buffers[i]);
    delete[] buffers;
  }
  virtual void split(int32_t* input) = 0;
  virtual void comb(int32_t* output) = 0;
  
  void clear(){
    for(size_t i=0; i<channels; ++i)
      buffers[i].clear();
  }
  inline FloatArray getSamples(int channel){
    if(channel < channels)
      return buffers[channel];
    return FloatArray();
  }
  inline int getChannels(){
    return channels;
  }
  inline int getSize(){
    return blocksize;
  }

  static SampleBuffer* create(uint8_t format, size_t blocksize);
  static void destroy(SampleBuffer* obj){
    delete obj;
  }
};

class SampleBuffer32 : public SampleBuffer {
public:
  SampleBuffer32(size_t channels, size_t blocksize) : SampleBuffer(channels, blocksize){}
  void split(int32_t* input){
    const float mul = 1.0f/MULTIPLIER_31B;
    for(size_t i=0; i<blocksize; ++i){
      for(size_t j=0; j<channels; ++j)
	buffers[j][i] = (*input++ << 8) * mul;
    }
  }
  void comb(int32_t* output){
    int32_t* dest = output;
    for(size_t i=0; i<blocksize; ++i){
      for(size_t j=0; j<channels; ++j){
#ifdef AUDIO_SATURATE_SAMPLES
	*dest++ = __SSAT((q31_t)(buffers[j][i] * MULTIPLIER_23B), 24);
#else
	*dest++ = ((int32_t)(buffers[j][i] * MULTIPLIER_23B));
#endif
      }
    }
  }
};
  
class SampleBuffer24 : public SampleBuffer {
public:
  SampleBuffer24(size_t channels, size_t blocksize) : SampleBuffer(channels, blocksize){}
  void split(int32_t* data){
    const float mul = 1.0f/MULTIPLIER_31B;
    uint8_t* input = (uint8_t*)data;
    int32_t qint;
    for(size_t i=0; i<blocksize; ++i){
      qint =( *input++)<<16;
      qint |= (*input++)<<24;
      qint |= (*input++);
      qint |= (*input++)<<8;
      buffers[0][i] = qint * mul;
      qint =( *input++)<<16;
      qint |= (*input++)<<24;
      qint |= (*input++);
      qint |= (*input++)<<8;
      buffers[1][i] = qint * mul;
    }
  }
  void comb(int32_t* output){
    uint8_t* dest = (uint8_t*)output;
    int32_t qint;
    for(size_t i=0; i<blocksize; ++i){
#ifdef AUDIO_SATURATE_SAMPLES
      qint = __SSAT((q31_t)(buffers[0][i] * MULTIPLIER_23B), 24);
#else
      qint = buffers[0][i] * MULTIPLIER_23B;
#endif
      *dest++ = qint >> 24;
      *dest++ = qint >> 16;
      *dest++ = qint >> 8;
      *dest++ = qint;
#ifdef AUDIO_SATURATE_SAMPLES
      qint = __SSAT((q31_t)(buffers[1][i] * MULTIPLIER_23B), 24);
#else
      qint = buffers[1][i] * MULTIPLIER_23B;
#endif
      *dest++ = qint >> 24;
      *dest++ = qint >> 16;
      *dest++ = qint >> 8;
      *dest++ = qint;
    }
  }
};

class SampleBuffer16 : public SampleBuffer {
public:
  SampleBuffer16(size_t channels, size_t blocksize) : SampleBuffer(channels, blocksize){}
  void split(int32_t* data){
    const float mul = 1.0f/MULTIPLIER_31B;
    uint16_t* input = (uint16_t*)data;
    int32_t qint;
    for(size_t i=0; i<blocksize; ++i){
      qint = (*input++)<<16;
      qint |= *input++;
      buffers[0][i] = qint * mul;
      qint = (*input++)<<16;
      qint |= *input++;
      buffers[1][i] = qint * mul;
    }
  }
  void comb(int32_t* output){
    float* l = (float*)buffers[0];
    float* r = (float*)buffers[1];
    uint32_t blkCnt = blocksize;
    uint16_t* dst = (uint16_t*)output;
    int32_t qint;
    while(blkCnt > 0u){
      qint = *l++ * MULTIPLIER_31B;
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
      qint = *r++ * MULTIPLIER_31B;
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
      blkCnt--;
    }
  }
};

SampleBuffer* SampleBuffer::create(uint8_t format, size_t blocksize){
  if(format == AUDIO_FORMAT_24B16_2X){
    debugMessage("16x2", 2, blocksize);
    return new SampleBuffer16(2, blocksize);
  // }else if(format == AUDIO_FORMAT_24B24_2X){
  //   debugMessage("24x2", 2, blocksize);
  //   return new SampleBuffer24(2, blocksize);
  }else if(format == AUDIO_FORMAT_24B32){
    debugMessage("32x2", 2, blocksize);
    return new SampleBuffer32(2, blocksize);
  }else if((format & 0xf0) == AUDIO_FORMAT_24B32){
    debugMessage("32xN", format & 0x0f, blocksize);
    return new SampleBuffer32(format & 0x0f, blocksize);
  }else{
    return NULL; // unrecognised audio format
  }
}

#endif // __SAMPLEBUFFER_H__
