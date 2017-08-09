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
  // float left[AUDIO_MAX_BLOCK_SIZE];
  // float right[AUDIO_MAX_BLOCK_SIZE];
  uint16_t size;
public:
  SampleBuffer(int blocksize){
    left = FloatArray::create(blocksize);
    right = FloatArray::create(blocksize);
  }
  void split32(int32_t* input, uint16_t blocksize){
    // ASSERT((blocksize & 0x3) == 0, "invalid blocksize");
    size = blocksize;
    float* l = left;
    float* r = right;
    uint32_t cnt = size >> 1u; // *2/4
    while(cnt > 0u){
      *l++ = (float)(int32_t)((*input++)<<8) / 2147483648.0f;
      *r++ = (float)(int32_t)((*input++)<<8) / 2147483648.0f;
      *l++ = (float)(int32_t)((*input++)<<8) / 2147483648.0f;
      *r++ = (float)(int32_t)((*input++)<<8) / 2147483648.0f;
      cnt--;
    }
  }
  void comb32(int32_t* output){
    int32_t* dest = output;
    int32_t tmp;
    // Seems CS4271 ADC samples are signed, DAC are unsigned. I2S Standard mode.
    for(int i=0; i<size; ++i){
      // tmp = (int32_t)(left[i] * 0x800000);
      // *dest++ = (uint32_t)(tmp+0x800000);
      // tmp = (int32_t)(right[i] * 0x800000);
      // *dest++ = (uint32_t)(tmp+0x800000);
      tmp = ((int32_t)(left[i] * 2147483648.0f));
      *dest++ = tmp>>8;
      tmp = ((int32_t)(right[i] * 2147483648.0f));
      *dest++ = tmp>>8;
    }
  }
  void split16(int32_t* data, uint16_t blocksize){
    uint16_t* input = (uint16_t*)data;
    size = blocksize;
    float* l = (float*)left;
    float* r = (float*)right;
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
