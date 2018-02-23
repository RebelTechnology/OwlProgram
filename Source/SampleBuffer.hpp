#ifndef __SAMPLEBUFFER_H__
#define __SAMPLEBUFFER_H__

#include <stdint.h>
#include <string.h>
#include "Patch.h"
#include "device.h"
#ifdef ARM_CORTEX
#include "arm_math.h"
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
      // *dest++ = ((int32_t)(left[i] * 8388608.0f));
      // *dest++ = ((int32_t)(right[i] * 8388608.0f));
      // Saturate to 24 bits to avoid nasty clipping on cs4271
      *dest++ = __SSAT((q31_t)(left[i] * 8388608.0f), 24);
      *dest++ = __SSAT((q31_t)(right[i] * 8388608.0f), 24);
    }
  }
  void split16(int32_t* data, uint16_t blocksize){
    uint16_t* input = (uint16_t*)data;
    size = blocksize;
    // for(int i=0; i<size; ++i){
    //   left[i] = ((input[i*4]<<16) | input[i*4+1]) * mul;
    //   right[i] = ((input[i*4+2]<<16) | input[i*4+3]) * mul;
    // }
    float* l = (float*)left;
    float* r = (float*)right;
    int32_t qint;
    while(blocksize){
      qint = (*input++)<<16;
      qint |= *input++;
      *l++ = qint * mul;
      qint = (*input++)<<16;
      qint |= *input++;
      *r++ = qint * mul;
      // *l++ = (int32_t)((*input++)<<16|*++input) * mul;
      // *r++ = (int32_t)((*input++)<<16|*++input) * mul;
      // *l++ = (int32_t)((*input++)<<16|*++input) * mul;
      // *r++ = (int32_t)((*input++)<<16|*++input) * mul;
      blocksize--;
    }
  }
  void comb16(int32_t* output){
    float* l = (float*)left;
    float* r = (float*)right;
    uint32_t blkCnt = size;
    uint16_t* dst = (uint16_t*)output;
    int32_t qint;
    while(blkCnt > 0u){
#ifdef AUDIO_SATURATE_SAMPLES
      qint = clip_q63_to_q31((q63_t)(*l++ * 2147483648.0f));
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
      qint = clip_q63_to_q31((q63_t)(*r++ * 2147483648.0f));
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
#else /* AUDIO_SATURATE_SAMPLES */
      qint = *l++ * 2147483648.0f;
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
      qint = *r++ * 2147483648.0f;
      *dst++ = qint >> 16;
      *dst++ = qint & 0xffff;
#endif /* AUDIO_SATURATE_SAMPLES */
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
