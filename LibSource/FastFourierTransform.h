#ifndef __FastFourierTransform_h__
#define __FastFourierTransform_h__

class FastFourierTransform {
private:
  arm_rfft_fast_instance_f32 instance;
public:
  void init(int len){
    void* args[] = {(void*)&instance, (void*)&len};
    getProgramVector()->serviceCall(OWL_SERVICE_ARM_RFFT_FAST_INIT_F32, args, 2);
    // arm_rfft_fast_init_f32(&instance, len);
    // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
  }
  void fft(float* in, ComplexFloatArray& out){
    arm_rfft_fast_f32(&instance, in, (float *)out, 0);
  }
  void ifft(ComplexFloatArray& in, float* out){
   arm_rfft_fast_f32(&instance, (float *)in, out, 1);
  }
};

#endif // __FastFourierTransform_h__
