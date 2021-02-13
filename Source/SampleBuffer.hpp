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


class SampleBuffer : public AudioBuffer {
protected:
  FloatArray left;
  FloatArray right;
  uint16_t size;
  const float mul = 1/2147483648.0f;
public:
  SampleBuffer(int blocksize){
    left = FloatArray::create(blocksize);
    right = FloatArray::create(blocksize);
  }
  ~SampleBuffer(){
    FloatArray::destroy(left);
    FloatArray::destroy(right);
  }
  void split32(int32_t* input, uint16_t blocksize){
    size = blocksize;
    for(int i=0; i<size; ++i){
      left[i] = (int32_t)((*input++)<<8) * mul;
      right[i] = (int32_t)((*input++)<<8) * mul;
    }
  }
  void comb32(int32_t* output){
    int32_t* dest = output;
    for(int i=0; i<size; ++i){
#ifdef AUDIO_SATURATE_SAMPLES
      // Saturate to 24 bits to avoid nasty clipping on cs4271
      *dest++ = __SSAT((q31_t)(left[i] * 8388608.0f), 24);
      *dest++ = __SSAT((q31_t)(right[i] * 8388608.0f), 24);
#else
      *dest++ = ((int32_t)(left[i] * 8388608.0f));
      *dest++ = ((int32_t)(right[i] * 8388608.0f));
#endif
    }
  }

// #define MULTIPLIER 2147483648
#define MULTIPLIER 8388608
// #define MULTIPLIER 1073741824
  
#if 0
  void split24(int32_t* data, uint16_t blocksize){
    size = blocksize;
    uint8_t* input = (uint8_t*)data;
    int32_t qint;
    for(int i=0; i<size; ++i){
      qint =( *input++)<<16;
      qint |= (*input++)<<24;
      qint |= (*input++);
      qint |= (*input++)<<8;
      left[i] = qint * mul;
      qint =( *input++)<<16;
      qint |= (*input++)<<24;
      qint |= (*input++);
      qint |= (*input++)<<8;
      right[i] = qint * mul;
    }
  }

  void comb24(int32_t* output){
    uint8_t* dest = (uint8_t*)output;
    int32_t qint;
    for(int i=0; i<size; ++i){
#ifdef AUDIO_SATURATE_SAMPLES
      qint = __SSAT((q31_t)(left[i] * MULTIPLIER), 24);
#else
      qint = left[i] * MULTIPLIER;
#endif
      *dest++ = qint >> 24;
      *dest++ = qint >> 16;
      *dest++ = qint >> 8;
      *dest++ = qint;
#ifdef AUDIO_SATURATE_SAMPLES
      qint = __SSAT((q31_t)(right[i] * MULTIPLIER), 24);
#else
      qint = right[i] * MULTIPLIER;
#endif
      *dest++ = qint >> 24;
      *dest++ = qint >> 16;
      *dest++ = qint >> 8;
      *dest++ = qint;
    }
  }
#endif

  void split16(int32_t* data, uint16_t blocksize){
    uint16_t* input = (uint16_t*)data;
    size = blocksize;
    int32_t qint;
    for(int i=0; i<size; ++i){
      qint = (*input++)<<16;
      qint |= *input++;
      left[i] = qint * mul;
      qint = (*input++)<<16;
      qint |= *input++;
      right[i] = qint * mul;
    }
  }
  void comb16(int32_t* output){
    float* l = (float*)left;
    float* r = (float*)right;
    uint32_t blkCnt = size;
    uint16_t* dst = (uint16_t*)output;
    int32_t qint;
    while(blkCnt > 0u){
      qint = *l++ * 2147483648.0f;
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
      qint = *r++ * 2147483648.0f;
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
      blkCnt--;
    }
  }

  void clear(){
    left.clear();
    right.clear();
  }
  inline FloatArray getSamples(int channel){
    return channel == LEFT_CHANNEL ? left : right;
    // return channel == 0 ? FloatArray(left, size) : FloatArray(right, size);
  }
  inline int getChannels(){
    return 2;
  }
  inline int getSize(){
    return size;
  }
};

#endif // __SAMPLEBUFFER_H__
