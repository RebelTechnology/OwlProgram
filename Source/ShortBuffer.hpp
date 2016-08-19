#ifndef __SHORTBUFFER_H__
#define __SHORTBUFFER_H__

#include <stdint.h>
#include <string.h>
#include "device.h"

#ifdef ARM_CORTEX
#include "arm_math.h"
#endif //ARM_CORTEX
class ShortBuffer : public AudioBuffer {
protected:
  int16_t left[AUDIO_MAX_BLOCK_SIZE];
  int16_t right[AUDIO_MAX_BLOCK_SIZE];
  uint16_t size;
public:
  void split(int16_t* input, uint16_t blocksize){
#if AUDIO_BITDEPTH == 16
    size = blocksize;
    float* l = left;
    float* r = right;
    uint32_t blkCnt = size >> 1u;
    while(blkCnt > 0u){
      *l++ = (*input++);
      *l++ = (*input++);
      *r++ = (*input++);
      *r++ = (*input++);
      blkCnt--;
    }
#else /* AUDIO_BITDEPTH != 16 */
    size = blocksize;
#ifdef AUDIO_BIGEND
    float* l = left;
    float* r = right;
    uint32_t blkCnt = size;
    while(blkCnt > 0u){
      *l++ = *input;
      input += 2;
      *r++ = *input;
      input += 2;
      blkCnt--;
    }
#else /* AUDIO_BIGEND */
    float* l = left;
    float* r = right;
    uint32_t blkCnt = size>>1;
    input++;
    while(blkCnt > 0u){      
      *l++ = *input;
      input += 2;
      *r++ = *input;
      input += 2;
      blkCnt--;
      // ++input;
      // *l++ = *input++;
      // ++input;
      // *r++ = *input++;
      // blkCnt--;
    }
#endif /* AUDIO_BIGEND */
#endif /* AUDIO_BITDEPTH != 16 */
  }
  void comb(int16_t* output){
#if AUDIO_BITDEPTH == 16
    float* l = left;
    float* r = right;
    uint32_t blkCnt = size >> 1u;
    while(blkCnt > 0u){
#ifdef AUDIO_SATURATE_SAMPLES
#error todo!
#else
      *output++ = *l++;
      *output++ = *r++;
      *output++ = *l++;
      *output++ = *r++;
#endif
      blkCnt--;
    }
#else /* AUDIO_BITDEPTH != 16 */
#ifdef AUDIO_BIGEND
    float* l = left;
    float* r = right;
    uint32_t blkCnt = size;
    int16_t* dst = output;
    int32_t qint;
    while(blkCnt > 0u){
#ifdef AUDIO_SATURATE_SAMPLES
#error todo!
#else /* AUDIO_SATURATE_SAMPLES */
      *output++ = *l++;
      *output++ = 0;
      *output++ = *r++;
      *output++ = 0;
      *output++ = *l++;
      *output++ = 0;
      *output++ = *r++;
      *output++ = 0;
#endif /* AUDIO_SATURATE_SAMPLES */
      blkCnt--;
    }
#else /* AUDIO_BIGEND */
    float* l = left;
    float* r = right;
    uint32_t blkCnt = size>>1;
    int32_t* dst = (int32_t*)output;
    while(blkCnt > 0u){
      *output++ = *l++;
      *output++ = *r++;
      *output++ = *l++;
      *output++ = *r++;
      blkCnt--;
    }
#endif /* AUDIO_BIGEND */
#endif /* AUDIO_BITDEPTH == 16 */
  }
  void clear(){
    memset(left, 0, getSize()*sizeof(int16_t));
    memset(right, 0, getSize()*sizeof(int16_t));
  }
  inline ShortArray getSamples(int channel){
    return channel == 0 ? ShortArray(left, size) : ShortArray(right, size);
  }
  inline int getChannels(){
    return AUDIO_CHANNELS;
  }
  inline int getSize(){
    return size;
  }
};

#endif // __SAMPLEBUFFER_H__
