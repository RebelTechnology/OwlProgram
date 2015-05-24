#ifndef __FastFourierTestPatch_hpp__
#define __FastFourierTestPatch_hpp__

#include "StompBox.h"

class FastFourierTransform {
private:
  arm_rfft_fast_instance_f32 instance;

public:
  void init(int len){
    arm_rfft_fast_init_f32(&instance, len);
// Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
  }
  void fft(float* in, float* out){
    arm_rfft_fast_f32(&instance, in, out, 0);
  }
  void ifft(float* in, float* out){
    arm_rfft_fast_f32(&instance, in, out, 1);
  }
};

class FastFourierTestPatch : public Patch {
private:
  FastFourierTransform transform;
public:
  FastFourierTestPatch(){
    transform.init(getBlockSize());
  }
  void processAudio(AudioBuffer &buffer){
    float* in = buffer.getSamples(0);
    float out[getBlockSize()];
    transform.fft(in, out);
    transform.ifft(out, in);
  }
};

#endif // __FastFourierTestPatch_hpp__
