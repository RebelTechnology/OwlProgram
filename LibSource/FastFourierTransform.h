#ifndef __FastFourierTransform_h__
#define __FastFourierTransform_h__


/**
 * This class performs direct and inverse Fast Fourier Transform.
 */
 
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
    ASSERT(len==32 || len ==64 || len==128 || len==256 || len==512 || len==1024 || len==2048 || len==4096, "Unsupported FFT size");
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
  /**
   * Default constructor.
   * Does **not** initialize the instance.
   * @remarks You need to call init(int size) before calling any other method
  */
  FastFourierTransform() : data(NULL), size(0){}

  /**
   * Construct and initialize the instance.
   * @param[in] aSize The size of the FFT
   * @remarks Only sizes of 32, 64, 128, 256, 512, 1024, 2048, 4096 are supported, due to limitations of the CMSIS library.
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  */
    FastFourierTransform(int aSize){
    init(aSize);
  }

  ~FastFourierTransform(){
    free(data);
  }

  /**
   * Initialize the instance.
   * @param aSize The size of the FFT
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  */
  void init(int aSize){
    ASSERT(aSize==32 || aSize ==64 || aSize==128 || aSize==256 || aSize==512 || aSize==1024 || aSize==2048 || aSize==4096, "Unsupported FFT size");
    cfgfft = kiss_fft_alloc(aSize, 0 , 0, 0);
    cfgifft = kiss_fft_alloc(aSize, 1,0, 0);
    size=aSize;
    data=(ComplexFloat *)malloc(sizeof(ComplexFloat)*getSize());
    temp=ComplexFloatArray(data, getSize());
  }

  /**
   * Perform the direct FFT.
   * @param[in] input The real-valued input array
   * @param[out] output The complex-valued output array
   * @remarks Calling this method will mess up the content of the **input** array.
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  */
  void fft(FloatArray& input, ComplexFloatArray& output){
    ASSERT(input.getSize() >= getSize(), "Input array too small");
    ASSERT(output.getSize() >= getSize(), "Output array too small");
    for(int n=0; n<getSize(); n++){
      temp[n].re=input[n];
      temp[n].im=0;
    }
    kiss_fft(cfgfft, (kiss_fft_cpx*)(float*)temp.getData(), (kiss_fft_cpx*)(float*)output);
  }
  
  /**
   * Perform the inverse FFT.
   * The output is rescaled by 1/fftSize.
   * @param[in] input The complex-valued input array
   * @param[out] output The real-valued output array
   * @remarks Calling this method will mess up the content of the **input** array.
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
   * 
  */
  void ifft(ComplexFloatArray& input, FloatArray& output){
    ASSERT(input.getSize() >= getSize(), "Input array too small");
    ASSERT(output.getSize() >= getSize(), "Output array too small");
    kiss_fft(cfgifft, (kiss_fft_cpx*)(float*)input, (kiss_fft_cpx*)(float*)temp.getData());
    float scale=1.0f/getSize();
    for(int n=0; n<getSize(); n++){
      output[n]=temp[n].re*scale;
    }
  }
    
  /**
   * Get the size of the FFT
   * @return The size of the FFT
  */
  int getSize(){
    return size;
  }
};
#endif /* ifndef ARM_CORTEX */

#endif // __FastFourierTransform_h__
