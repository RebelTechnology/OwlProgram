#ifndef __RobotPatch_hpp__
#define __RobotPatch_hpp__

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

class RobotPatch : public Patch {
private:
  FastFourierTransform transform;
  float * buf;
public:
  RobotPatch(){
    registerParameter(PARAMETER_A, "GAIN");
    // registerParameter(PARAMETER_B, "TYPE"); // <=0.5: robotization, >0.5: whisperization
    registerParameter(PARAMETER_C, "N_BINS"); // How many bins we transform 
    transform.init(getBlockSize());
    buf = createMemoryBuffer(1, getBlockSize())->getSamples(0);
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A);
    float nBins= getParameterValue(PARAMETER_C);

    float* in = buffer.getSamples(0);
    transform.fft(in, buf);
    int size=getBlockSize();
    for(int n=0; n<getBlockSize()*0.5; n+=2){
      buf[n]=sqrtf(buf[n]*buf[n]+buf[n+1]*buf[n+1]);
      buf[n+1]=0;
    }
    transform.ifft(buf, in);
    for(int n=0; n<size; n++)
      in[n]=in[n]*0.5;
  }
};

#endif // __RobotPatch_hpp__
