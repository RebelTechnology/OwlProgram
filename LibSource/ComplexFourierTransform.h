#ifndef __ComplexFourierTransform_h__
#define __ComplexFourierTransform_h__

#include "ComplexFloatArray.h"

#ifdef ARM_CORTEX
#include "arm_const_structs.h"

class ComplexFourierTransform {
private:
  arm_cfft_instance_f32 instance;
public:
  ComplexFourierTransform(){};
  ComplexFourierTransform(int len){
    init(len);
  }
  void init(int len){
    ASSERT(len==32 || len ==64 || len==128 || len==256 || len==512 || len==1024 || len==2048 || len==4096, "Unsupported FFT size");
    void* args[] = {(void*)&instance, (void*)&len};
    getProgramVector()->serviceCall(OWL_SERVICE_ARM_CFFT_INIT_F32, args, 2);
    // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
  }
  void fft(ComplexFloatArray inout){
    ASSERT(inout.getSize() >= getSize(), "Input array too small");
    arm_cfft_f32(&instance, (float*)inout.getData(), 0, 1); //forward
  }
  void ifft(ComplexFloatArray inout){
    ASSERT(inout.getSize() >= getSize(), "Input array too small");
   arm_cfft_f32(&instance, (float*)inout.getData(), 1, 1); //inverse
  }
  int getSize(){
    return instance.fftLen;
  }
};
#endif /* ARM_CORTEX */

#ifndef ARM_CORTEX
#include "kiss_fft.h"
class ComplexFourierTransform {
private:
  kiss_fft_cfg cfgfft;
  kiss_fft_cfg cfgifft;
  ComplexFloatArray temp;
public:
  ComplexFourierTransform(){}
  ComplexFourierTransform(int len){
    init(len);
  }
  ~ComplexFourierTransform(){
    ComplexFloatArray::destroy(temp);
  }
  void init(int len){
    ASSERT(len==32 || len ==64 || len==128 || len==256 || len==512 || len==1024 || len==2048 || len==4096, "Unsupported FFT size");
    cfgfft = kiss_fft_alloc(len, 0 , 0, 0);
    cfgifft = kiss_fft_alloc(len, 1,0, 0);
    temp = ComplexFloatArray::create(getSize());
  }
  void fft(ComplexFloatArray inout){
    ASSERT(inout.getSize() >= getSize(), "Input array too small");
    kiss_fft(cfgfft, (kiss_fft_cpx*)(float*)inout.getData(), (kiss_fft_cpx*)(float*)temp.getData());
    inout.copyFrom(temp);
  }
  void ifft(ComplexFloatArray inout){
    ASSERT(inout.getSize() >= getSize(), "Input array too small");
    kiss_fft(cfgifft, (kiss_fft_cpx*)(float*)inout.getData(), (kiss_fft_cpx*)(float*)temp.getData());
    temp.scale(1.0f/getSize());
    inout.copyFrom(temp);
  }
  size_t getSize(){
    return temp.getSize();
  }
};
#endif /* ifndef ARM_CORTEX */

#endif // __ComplexFourierTransform_h__
