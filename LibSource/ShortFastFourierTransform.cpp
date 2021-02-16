#include "ShortFastFourierTransform.h"
#include "message.h"
#include "ProgramVector.h"
#include "ServiceCall.h"

#ifdef ARM_CORTEX
ShortFastFourierTransform::ShortFastFourierTransform(){}

ShortFastFourierTransform::ShortFastFourierTransform(int len){
  init(len);
}

ShortFastFourierTransform::~ShortFastFourierTransform(){}

void ShortFastFourierTransform::init(int aSize){
  len = aSize;
  ASSERT(len==32 || len ==64 || len==128 || len==256 || len==512 || len==1024 || len==2048 || len==4096, "Unsupported FFT size");
  void* args[] = {(void*)&instance, (void*)&len};
  // getProgramVector()->serviceCall(OWL_SERVICE_ARM_RFFT_FAST_INIT_Q15, args, 2);
  // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
}

void ShortFastFourierTransform::fft(ShortArray in, ComplexShortArray out){
  ASSERT(in.getSize() >= getSize(), "Input array too small");
  ASSERT(out.getSize() >= getSize(), "Output array too small");
  arm_rfft_init_q15(&instance, len, 0, 1);
  arm_rfft_q15(&instance, (int16_t*)in.getData(), (int16_t*)out.getData());
}

void ShortFastFourierTransform::ifft(ComplexShortArray in, ShortArray out){
  ASSERT(in.getSize() >= getSize(), "Input array too small");
  ASSERT(out.getSize() >= getSize(), "Output array too small");
  arm_rfft_init_q15(&instance, len, 1, 1);
  arm_rfft_q15(&instance, (int16_t*)in.getData(), (int16_t*)out.getData());
}

int ShortFastFourierTransform::getSize(){
  return len;
}

#else /* ARM_CORTEX */

ShortFastFourierTransform::ShortFastFourierTransform(){}

ShortFastFourierTransform::ShortFastFourierTransform(int aSize){
  init(aSize);
}

ShortFastFourierTransform::~ShortFastFourierTransform(){
  ASSERT(false, "TODO");
  ComplexShortArray::destroy(temp);
}

void ShortFastFourierTransform::init(int aSize){
  ASSERT(aSize==32 || aSize ==64 || aSize==128 || aSize==256 || aSize==512 || aSize==1024 || aSize==2048 || aSize==4096, "Unsupported FFT size");
  cfgfft = kiss_fft_alloc(aSize, 0 , 0, 0);
  cfgifft = kiss_fft_alloc(aSize, 1,0, 0);
  temp = ComplexShortArray::create(getSize());
}

void ShortFastFourierTransform::fft(ShortArray input, ComplexShortArray output){
  ASSERT(input.getSize() >= getSize(), "Input array too small");
  ASSERT(output.getSize() >= getSize(), "Output array too small");
  for(int n=0; n<getSize(); n++){
    temp[n].re=input[n];
    temp[n].im=0;
  }
  kiss_fft(cfgfft, (kiss_fft_cpx*)(float*)temp.getData(), (kiss_fft_cpx*)(float*)output);
}
  
void ShortFastFourierTransform::ifft(ComplexShortArray input, ShortArray output){
  ASSERT(input.getSize() >= getSize(), "Input array too small");
  ASSERT(output.getSize() >= getSize(), "Output array too small");
  kiss_fft(cfgifft, (kiss_fft_cpx*)(float*)input, (kiss_fft_cpx*)(float*)temp.getData());
  float scale=1.0f/getSize();
  for(int n=0; n<getSize(); n++){
    output[n]=temp[n].re*scale;
  }
}
    
int ShortFastFourierTransform::getSize(){
  return temp.getSize();
}

#endif /* ifndef ARM_CORTEX */
