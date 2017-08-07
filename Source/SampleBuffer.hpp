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
  // FloatArray left;//[AUDIO_MAX_BLOCK_SIZE];
  // FloatArray right;//[AUDIO_MAX_BLOCK_SIZE];
  float left[AUDIO_MAX_BLOCK_SIZE];
  float right[AUDIO_MAX_BLOCK_SIZE];
  uint16_t size;
public:
  void split(int32_t* data, uint16_t blocksize){
    uint16_t* input = (uint16_t*)data;
    size = blocksize;
    float* l = left;
    float* r = right;
    uint32_t blkCnt = size;
    int32_t qint;
    while(blkCnt > 0u){
      qint = (*input++)<<16;
      qint |= *input++;
      *l++ = qint / 2147483648.0f;
      qint = (*input++)<<16;
      qint |= *input++;
      *r++ = qint / 2147483648.0f;
      // *l++ = (int32_t)((*input++)<<16|*++input) / 2147483648.0f;
      // *r++ = (int32_t)((*input++)<<16|*++input) / 2147483648.0f;
      // *l++ = (int32_t)((*input++)<<16|*++input) / 2147483648.0f;
      // *r++ = (int32_t)((*input++)<<16|*++input) / 2147483648.0f;
      blkCnt--;
    }
  }
  void comb(int32_t* output){
    float* l = left;
    float* r = right;
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
    memset(left, 0, getSize()*sizeof(float));
    memset(right, 0, getSize()*sizeof(float));
  }
  inline FloatArray getSamples(int channel){
    return channel == 0 ? FloatArray(left, size) : FloatArray(right, size);
  }
  // inline float* getSamples(int channel){
  //   return channel == 0 ? left : right;
  // }
  inline int getChannels(){
    return AUDIO_CHANNELS;
  }
  // void setSize(uint16_t sz){
  // // size is set by split()
  //   if(sz <= AUDIO_MAX_BLOCK_SIZE)
  //     size = sz;
  // }
  inline int getSize(){
    return size;
  }
};

#endif // __SAMPLEBUFFER_H__
