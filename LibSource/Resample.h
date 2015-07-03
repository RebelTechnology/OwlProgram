#ifndef __Resample_h__
#define __Resample_h__

class Upsample{
  arm_fir_interpolate_instance_f32 upInstance;
  FloatArray downCoefficients;
  FloatArray upStates;
  int blockSize;
public:
  Upsample(){};
  Upsample(int factor, int numTaps, int aBlockSize){
    init(factor, numTaps, aBlockSize);
  }
  void init(int factor, int numTaps, int aBlockSize){
    blockSize=aBlockSize;
    numTaps=(numTaps/factor)*numTaps; //make sure that numState is a multiple of factor
    downCoefficients=FloatArray::create(numTaps);
    /*hardcoded coefficents from matlab, does not work*/
    downCoefficients[0]=0.011733;
    downCoefficients[1]=0.052308;
    downCoefficients[2]=0.164578;
    downCoefficients[3]=0.271380;
    downCoefficients[4]=0.271380;
    downCoefficients[5]=0.164578;
    downCoefficients[6]=0.052308;
    downCoefficients[7]=0.011733;

    upStates=FloatArray::create((numTaps/factor)+blockSize-1);
    int ret=arm_fir_interpolate_init_f32(&upInstance, factor, numTaps, downCoefficients, upStates, blockSize);
    ASSERT(ARM_MATH_SUCCESS==ret, "init arm_fir_interpolate_init_f32");
  }
  void up(FloatArray in, FloatArray out){
    // ASSERT(in.getSize()<=upInstance.L*out.getSize(), "input ");
    arm_fir_interpolate_f32(&upInstance, in.getData(), out.getData(), in.getSize());
  }
  int getBlockSize(){
    return upInstance.L;
  }
};

class Downsample{
private:
  arm_fir_decimate_instance_f32 downInstance;
  FloatArray downCoefficients; //comment / uncomment these two lines to cause disruptions
  FloatArray downStates; //comment / uncomment these two lines to cause disruptions
  int blockSize;
public:
  Downsample(){}
  Downsample(int factor, int numTaps, int aBlockSize){
    init(factor, numTaps, aBlockSize);
  }
  void init(int factor, int numTaps, int aBlockSize){
    blockSize=aBlockSize;
    numTaps=(numTaps/factor)*numTaps; //make sure that numState is a multiple of factor
    downCoefficients=FloatArray::create(numTaps);
    /*hardcoded coefficents from matlab, does not work*/
    downCoefficients[0]=0.011733;
    downCoefficients[1]=0.052308;
    downCoefficients[2]=0.164578;
    downCoefficients[3]=0.271380;
    downCoefficients[4]=0.271380;
    downCoefficients[5]=0.164578;
    downCoefficients[6]=0.052308;
    downCoefficients[7]=0.011733;
    downStates=FloatArray::create(numTaps+blockSize-1);
    int ret=arm_fir_decimate_init_f32(&downInstance, numTaps, factor, downCoefficients, downStates, blockSize);
    // ASSERT(ARM_MATH_SUCCESS==ret, "init arm_fir_decimate_init_f32"); 
  }
  void down(FloatArray in, FloatArray out){
    ASSERT(in.getSize()==getBlockSize(), "input ");
    arm_fir_decimate_f32(&downInstance, in.getData(), out.getData(), in.getSize());
  }
  int getBlockSize(){
    return blockSize;
  }
  float *getCoefficients(){
    return downCoefficients;
  }
};

#endif // __Resample_h__
