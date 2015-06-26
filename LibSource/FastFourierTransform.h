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
  void fft(float* in, ComplexFloatArray& out){
#ifdef ARM_CORTEX
    arm_rfft_fast_f32(&instance, in, (float *)out, 0);
#endif /* ARM_CORTEX */
  }
  void ifft(ComplexFloatArray& in, float* out){
#ifdef ARM_CORTEX
   arm_rfft_fast_f32(&instance, (float *)in, out, 1);
#endif /* ARM_CORTEX */
  }
};

#endif // __FastFourierTransform_h__
