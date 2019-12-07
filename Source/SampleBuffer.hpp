#ifndef __SAMPLEBUFFER_H__
#define __SAMPLEBUFFER_H__

// https://stackoverflow.com/questions/41675438/fastest-way-to-swap-alternate-bytes-on-arm-cortex-m4-using-gcc

#include <stdint.h>
#include <string.h>
#include "Patch.h"
#include "device.h"
#ifdef ARM_CORTEX
#include "arm_math.h"
// #define AUDIO_SATURATE_SAMPLES
#else
#undef AUDIO_SATURATE_SAMPLES
#endif //ARM_CORTEX

#define MULTIPLIER_31B 2147483648
#define MULTIPLIER_30B 1073741824
#define MULTIPLIER_23B 8388608

class SampleBuffer : public AudioBuffer {
protected:
  FloatArray* buffers;
  size_t size;
  size_t channels;
public:
  SampleBuffer(size_t channels, size_t blocksize)
    :channels(channels), size(blocksize) {
    buffers = new FloatArray[channels];
    for(size_t i=0; i<channels; ++i)
      buffers[i] = FloatArray::create(blocksize);
  }
  ~SampleBuffer(){
    for(size_t i=0; i<channels; ++i)
      FloatArray::destroy(buffers[i]);
    delete[] buffers;
  }
  void split32(int32_t* input, uint16_t blocksize){
    const float mul = 1.0f/MULTIPLIER_31B;
    size = blocksize;
    for(size_t i=0; i<size; ++i){
      buffers[0][i] = (int32_t)((*input++)<<8) * mul;
      buffers[1][i] = (int32_t)((*input++)<<8) * mul;
    }
  }
  void comb32(int32_t* output){
    int32_t* dest = output;
    for(size_t i=0; i<size; ++i){
#ifdef AUDIO_SATURATE_SAMPLES
      // Saturate to 24 bits to avoid nasty clipping on cs4271
      *dest++ = __SSAT((q31_t)(buffers[0][i] * MULTIPLIER_23B), 24);
      *dest++ = __SSAT((q31_t)(buffers[1][i] * MULTIPLIER_23B), 24);
#else
      *dest++ = ((int32_t)(buffers[0][i] * MULTIPLIER_23B));
      *dest++ = ((int32_t)(buffers[1][i] * MULTIPLIER_23B));
#endif
    }
  }

  void split32xN(int32_t* input, uint16_t blocksize){
    const float mul = 1.0f/MULTIPLIER_31B;
    size = blocksize;
    for(size_t i=0; i<size; ++i){
      for(size_t j=0; j<channels; ++j)
	buffers[j][i] = (int32_t)((*input++)) * mul;
    }
  }
  void comb32xN(int32_t* output){
    int32_t* dest = output;
    for(size_t i=0; i<size; ++i){
      for(size_t j=0; j<channels; ++j){
#ifdef AUDIO_SATURATE_SAMPLES
	*dest++ = __SSAT((q31_t)(buffers[j][i] * MULTIPLIER_31B), 24);
#else
	*dest++ = ((int32_t)(buffers[j][i] * MULTIPLIER_31B));
#endif
      }
    }
  }

  void split32x4(int32_t* input, uint16_t blocksize){
    const float mul = 1.0f/MULTIPLIER_31B;
    size = blocksize;
    for(size_t i=0; i<size; ++i){
      buffers[0][i] = (int32_t)((*input++)) * mul;
      buffers[1][i] = (int32_t)((*input++)) * mul;
      buffers[2][i] = (int32_t)((*input++)) * mul;
      buffers[3][i] = (int32_t)((*input++)) * mul;
    }
  }
  void comb32x4(int32_t* output){
    int32_t* dest = output;
    for(size_t i=0; i<size; ++i){
#ifdef AUDIO_SATURATE_SAMPLES
      *dest++ = __SSAT((q31_t)(buffers[0][i] * MULTIPLIER_31B), 24);
      *dest++ = __SSAT((q31_t)(buffers[1][i] * MULTIPLIER_31B), 24);
      *dest++ = __SSAT((q31_t)(buffers[2][i] * MULTIPLIER_31B), 24);
      *dest++ = __SSAT((q31_t)(buffers[3][i] * MULTIPLIER_31B), 24);
#else
      *dest++ = ((int32_t)(buffers[0][i] * MULTIPLIER_31B));
      *dest++ = ((int32_t)(buffers[1][i] * MULTIPLIER_31B));
      *dest++ = ((int32_t)(buffers[2][i] * MULTIPLIER_31B));
      *dest++ = ((int32_t)(buffers[3][i] * MULTIPLIER_31B));
#endif
    }
  }
  
  void split24(int32_t* data, uint16_t blocksize){
    const float mul = 1.0f/MULTIPLIER_31B;
    size = blocksize;
    uint8_t* input = (uint8_t*)data;
    int32_t qint;
    for(size_t i=0; i<size; ++i){
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

  void comb24(int32_t* output){
    uint8_t* dest = (uint8_t*)output;
    int32_t qint;
    for(size_t i=0; i<size; ++i){
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

  void split16(int32_t* data, uint16_t blocksize){
    const float mul = 1.0f/MULTIPLIER_31B;
    uint16_t* input = (uint16_t*)data;
    size = blocksize;
    int32_t qint;
    for(size_t i=0; i<size; ++i){
      qint = (*input++)<<16;
      qint |= *input++;
      buffers[0][i] = qint * mul;
      qint = (*input++)<<16;
      qint |= *input++;
      buffers[1][i] = qint * mul;
    }
  }
  void comb16(int32_t* output){
    float* l = (float*)buffers[0];
    float* r = (float*)buffers[1];
    uint32_t blkCnt = size;
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
    return size;
  }
};

#endif // __SAMPLEBUFFER_H__
