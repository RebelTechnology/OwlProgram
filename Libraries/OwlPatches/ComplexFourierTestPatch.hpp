#ifndef __ComplexFourierTestPatch_hpp__
#define __ComplexFourierTestPatch_hpp__

#include "StompBox.h"
#include "arm_const_structs.h"

class ComplexFourierTransform {
private:
const arm_cfft_instance_f32* instance;
public:
  void init(int len){
    switch(len) {
    case 16:
      instance = &arm_cfft_sR_f32_len16;
      break;
    case 32:
      instance = &arm_cfft_sR_f32_len32;
      break;
    case 64:
      instance = &arm_cfft_sR_f32_len64;
      break;
    case 128:
      instance = &arm_cfft_sR_f32_len128;
      break;
    // case 256:
    //   instance = &arm_cfft_sR_f32_len256;
    //   break;
    // case 512:
    //   instance = &arm_cfft_sR_f32_len512;
    //   break;
    // case 1024:
    //   instance = &arm_cfft_sR_f32_len1024;
    //   break;
    // case 2048:
    //   instance = &arm_cfft_sR_f32_len2048;
    //   break;
    // case 4096:
    //   instance = &arm_cfft_sR_f32_len4096;
    //   break;
    }
  }
  void fft(float* in){
    arm_cfft_f32(instance, in, 0, 1);
  }
  void ifft(float* in){
    arm_cfft_f32(instance, in, 1, 1);
  }
};

class ComplexFourierTestPatch : public Patch {
private:
  ComplexFourierTransform transform;
  float* buf;
public:
  ComplexFourierTestPatch(){
    transform.init(getBlockSize());
    buf = createMemoryBuffer(1, getBlockSize()*2)->getSamples(0);
    registerParameter(PARAMETER_A, "ComplexFourierTest");
  }
  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    float* in = buffer.getSamples(0);
    float buf[getBlockSize()*2];
    int j=0;
    for(int i=0; i<size;++i){
      buf[j++] = in[i]; // real
      buf[j++] = 0;     // complex
    }
    transform.fft(buf);
    transform.ifft(buf);
    for(int i=0; i<size;++i)
      in[i] = buf[i*2];
  }
};

#endif // __ComplexFourierTestPatch_hpp__
