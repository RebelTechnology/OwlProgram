#ifndef __FastFourierTransform_h__
#define __FastFourierTransform_h__


#ifdef ARM_CORTEX
class FastFourierTransform {
private:
  arm_rfft_fast_instance_f32 instance;
public:
  FastFourierTransform(){};
  FastFourierTransform(int len){
    init(len);
  }
  void init(int len){
    ASSERT(len==32 || len ==64 || len==128 || len==256 || len==512 || len==1024 || len==2048 || len==4096, "Wrong fft size");
    void* args[] = {(void*)&instance, (void*)&len};
    getProgramVector()->serviceCall(OWL_SERVICE_ARM_RFFT_FAST_INIT_F32, args, 2);
    // arm_rfft_fast_init_f32(&instance, len);
    // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
  }
  void fft(FloatArray& in, ComplexFloatArray& out){
    ASSERT(in.getSize() >= getSize(), "Input array too small");
    ASSERT(out.getSize() >= getSize(), "Output array too small");
    arm_rfft_fast_f32(&instance, (float*)in, (float*)out, 0);
  }
  void ifft(ComplexFloatArray& in, FloatArray& out){
    ASSERT(in.getSize() >= getSize(), "Input array too small");
    ASSERT(out.getSize() >= getSize(), "Output array too small");
   arm_rfft_fast_f32(&instance, (float*)in, (float*)out, 1);
  }
  int getSize(){
    return instance.fftLenRFFT;
  }
};
#endif /* ARM_CORTEX */

#ifndef ARM_CORTEX
#include "kiss_fft.h"
class FastFourierTransform {
private:
  kiss_fft_cfg cfgfft;
  kiss_fft_cfg cfgifft;
  ComplexFloatArray temp;
  ComplexFloat *data;
  int size;
public:
  FastFourierTransform(){};
  FastFourierTransform(){
    data=NULL;
  }
  FastFourierTransform(int len){
    init(len);
  }
  ~FastFourierTransform(){
    free(data);
  }
  void init(int len){
    ASSERT(len==32 || len ==64 || len==128 || len==256 || len==512 || len==1024 || len==2048 || len==4096, "Wrong fft size");
    cfgfft = kiss_fft_alloc(len, 0 , 0, 0);
    cfgifft = kiss_fft_alloc(len, 1,0, 0);
    size=len;
    data=(ComplexFloat *)malloc(sizeof(ComplexFloat)*getSize());
    temp=ComplexFloatArray(data, getSize());
  }
  void fft(FloatArray& in, ComplexFloatArray& out){
    ASSERT(in.getSize() >= getSize(), "Input array too small");
    ASSERT(out.getSize() >= getSize(), "Output array too small");
    for(int n=0; n<getSize(); n++){
      temp[n].re=in[n];
      temp[n].im=0;
    }
    kiss_fft(cfgfft, (kiss_fft_cpx*)(float*)temp.getData(), (kiss_fft_cpx*)(float*)out);
  }
  void ifft(ComplexFloatArray& in, FloatArray& out){
    ASSERT(in.getSize() >= getSize(), "Input array too small");
    ASSERT(out.getSize() >= getSize(), "Output array too small");
    kiss_fft(cfgifft, (kiss_fft_cpx*)(float*)in, (kiss_fft_cpx*)(float*)temp.getData());
    float scale=1.0f/getSize();
    for(int n=0; n<getSize(); n++){
      out[n]=temp[n].re*scale;
    }
  }
  int getSize(){
    return size;
  }
};
#endif /* ifndef ARM_CORTEX */
#endif // __FastFourierTransform_h__
