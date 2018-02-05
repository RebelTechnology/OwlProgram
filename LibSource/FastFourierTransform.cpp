#include "FastFourierTransform.h"
#include "message.h"
#include "ProgramVector.h"
#include "ServiceCall.h"

#ifdef ARM_CORTEX
FastFourierTransform::FastFourierTransform(){}

FastFourierTransform::FastFourierTransform(int len){
  init(len);
}

FastFourierTransform::~FastFourierTransform(){}

void FastFourierTransform::init(int len){
  ASSERT(len==32 || len ==64 || len==128 || len==256 || len==512 || len==1024 || len==2048 || len==4096, "Unsupported FFT size");
  void* args[] = {(void*)&instance, (void*)&len};
  getProgramVector()->serviceCall(OWL_SERVICE_ARM_RFFT_FAST_INIT_F32, args, 2);
  // arm_rfft_fast_init_f32(&instance, len);
  // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
}

void FastFourierTransform::fft(FloatArray in, ComplexFloatArray out){
  ASSERT(in.getSize() >= getSize(), "Input array too small");
  ASSERT(out.getSize() >= getSize()/2, "Output array too small");
  arm_rfft_fast_f32(&instance, (float*)in, (float*)out, 0);
}

void FastFourierTransform::ifft(ComplexFloatArray in, FloatArray out){
  ASSERT(in.getSize() >= getSize()/2, "Input array too small");
  ASSERT(out.getSize() >= getSize(), "Output array too small");
  arm_rfft_fast_f32(&instance, (float*)in, (float*)out, 1);
}

int FastFourierTransform::getSize(){
  return instance.fftLenRFFT;
}

#else /* ARM_CORTEX */

FastFourierTransform::FastFourierTransform(){}

FastFourierTransform::FastFourierTransform(int aSize){
  init(aSize);
}

FastFourierTransform::~FastFourierTransform(){
  ComplexFloatArray::destroy(temp);
}

void FastFourierTransform::init(int aSize){
  ASSERT(aSize==32 || aSize ==64 || aSize==128 || aSize==256 || aSize==512 || aSize==1024 || aSize==2048 || aSize==4096, "Unsupported FFT size");
  cfgfft = kiss_fft_alloc(aSize, 0 , 0, 0);
  cfgifft = kiss_fft_alloc(aSize, 1,0, 0);
  temp = ComplexFloatArray::create(getSize());
}

void FastFourierTransform::fft(FloatArray input, ComplexFloatArray output){
  ASSERT(input.getSize() >= getSize(), "Input array too small");
  ASSERT(output.getSize() >= getSize(), "Output array too small");
  for(int n=0; n<getSize(); n++){
    temp[n].re=input[n];
    temp[n].im=0;
  }
  kiss_fft(cfgfft, (kiss_fft_cpx*)(float*)temp.getData(), (kiss_fft_cpx*)(float*)output);
}
  
void FastFourierTransform::ifft(ComplexFloatArray input, FloatArray output){
  ASSERT(input.getSize() >= getSize(), "Input array too small");
  ASSERT(output.getSize() >= getSize(), "Output array too small");
  kiss_fft(cfgifft, (kiss_fft_cpx*)(float*)input, (kiss_fft_cpx*)(float*)temp.getData());
  float scale=1.0f/getSize();
  for(int n=0; n<getSize(); n++){
    output[n]=temp[n].re*scale;
  }
}
    
int FastFourierTransform::getSize(){
  return temp.getSize();
}

#endif /* ifndef ARM_CORTEX */
