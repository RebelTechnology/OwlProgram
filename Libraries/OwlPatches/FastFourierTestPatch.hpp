#ifndef __FastFourierTestPatch_hpp__
#define __FastFourierTestPatch_hpp__

#include "StompBox.h"

class FastFourierTransform {
private:
  arm_rfft_fast_instance_f32 instance;

public:
  void init(int len){
    // void* args[] = {(void*)&instance, (void*)&len};
    // getProgramVector()->serviceCall(OWL_SERVICE_ARM_RFFT_FAST_INIT_F32, args, 2);
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
    registerParameter(PARAMETER_A, "Gain");
    transform.init(getBlockSize());
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A);
    float* in = buffer.getSamples(0);
    int size = buffer.getSize();
    float buf[size];
    transform.fft(in, buf);
    // for(int i=0; i<size; i++)
    //   buf[i] *= gain;
    transform.ifft(buf, in);
  }
};

#endif // __FastFourierTestPatch_hpp__
