#ifndef __FastFourierTransform_h__
#define __FastFourierTransform_h__

class FastFourierTransform {
private:
#ifdef ARM_CORTEX
  arm_rfft_fast_instance_f32 instance;
#endif /* ARM_CORTEX */
public:
  void init(int len){
#ifdef ARM_CORTEX
    void* args[] = {(void*)&instance, (void*)&len};
    getProgramVector()->serviceCall(OWL_SERVICE_ARM_RFFT_FAST_INIT_F32, args, 2);
    // arm_rfft_fast_init_f32(&instance, len);
    // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
#endif /* ARM_CORTEX */
  }
  void fft(FloatArray& in, ComplexFloatArray& out){
    ASSERT(in.getSize() >= getSize(), "Input array too small");
    ASSERT(out.getSize() >= getSize(), "Output array too small");
#ifdef ARM_CORTEX
    arm_rfft_fast_f32(&instance, (float*)in, (float*)out, 0);
#endif /* ARM_CORTEX */
  }
  void ifft(ComplexFloatArray& in, FloatArray& out){
    ASSERT(in.getSize() >= getSize(), "Input array too small");
    ASSERT(out.getSize() >= getSize(), "Output array too small");
#ifdef ARM_CORTEX
   arm_rfft_fast_f32(&instance, (float*)in, (float*)out, 1);
#endif /* ARM_CORTEX */
  }
  int getSize(){
#ifdef ARM_CORTEX
    return instance.fftLenRFFT;
#endif /* ARM_CORTEX */
  }
};

#endif // __FastFourierTransform_h__
