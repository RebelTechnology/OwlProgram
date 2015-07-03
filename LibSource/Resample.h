#ifndef __Resample_h__
#define __Resample_h__

class Resample {
private:
  arm_fir_interpolate_instance_f32 upInstance;
  // arm_fir_decimate_instance_f32 downInstance;
  FloatArray upCoefficients;
  FloatArray upStates;
  FloatArray downCoefficients; //comment / uncomment these two lines to cause disruptions
  FloatArray downStates; //comment / uncomment these two lines to cause disruptions
public:
  Resample(){};
  Resample(int factor, int numTaps, int blockSize){
    init(factor, numTaps, blockSize);
  }
  void init(int factor, int numTaps, int blockSize){
    numTaps=(numTaps/factor)*numTaps; //make sure that numState is a multiple of factor
    upCoefficients=FloatArray::create(numTaps);
    upStates=FloatArray::create((numTaps/factor)+blockSize-1);
    ASSERT(ARM_MATH_SUCCESS==arm_fir_interpolate_init_f32(&upInstance, factor, numTaps, upCoefficients, upStates, blockSize), "arm_fir_interpolate_init_f32"); 
    // downCoefficients=FloatArray::create(numTaps);
    // downStates=FloatArray::create(numTaps+blockSize-1);
    // ASSERT(ARM_MATH_SUCCESS==arm_fir_decimate_init_f32(&downInstance, factor, numTaps, downCoefficients, downStates, blockSize), "arm_fir_interpolate_init_f32"); 
  }
  void up(FloatArray in, FloatArray out){
    // ASSERT(in.getSize()<=upInstance.L*out.getSize(), "input ");
    arm_fir_interpolate_f32(&upInstance, in.getData(), out.getData(), in.getSize());
  };
  void down(FloatArray in, FloatArray out){
    // ASSERT(in.getSize()>=downInstance.M*out.getSize(), "input ");
    // arm_fir_decimate_f32(&downInstance, in.getData(), out.getData(), in.getSize());
  };
};

#endif // __Resample_h__
