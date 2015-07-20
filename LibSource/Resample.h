#include "BiquadFilter.hpp"
#
/**
   Implements 4x oversampling
*/  
class Resampler {
private:
  BiquadFilter *downfilter;
  BiquadFilter *upfilter;
  int factor=4;
  int upsampleStages;
  int downsampleStages;
public:
  Resampler():upsampleStages(1), downsampleStages(1){
    // two filters: same coefficients, different state variables
    init();
  }
  Resampler(int stages){
    upsampleStages=stages;
    downsampleStages=stages;
    init();
  }
  Resampler(int aUpsampleStages, int aDownsampleStages){
    upsampleStages=aUpsampleStages;
    downsampleStages=aDownsampleStages;
    init();
  }
  ~Resampler(){
    BiquadFilter::destroy(upfilter);
    BiquadFilter::destroy(downfilter);
  }
  void init(){
    upfilter=BiquadFilter::create(upsampleStages);
    downfilter=BiquadFilter::create(downsampleStages);
    // [B, A]=cheby1(2, 2, 0.25); then use [B, -A(2:end)] , note the minus sign in front of the A coefficients!!!!
    //alternatively, we could use FilterStage to compute the coefficients
    static float downCoeffs[5]={0.07609109, 0.15218218, 0.07609109, +1.16511283,  -0.54828486};
    static float upCoeffs[5]={0.07609109, 0.15218218, 0.07609109, +1.16511283,  -0.54828486};
    for(int n=0; n<3; n++){
      upCoeffs[n]=downCoeffs[n]*factor; //compensate for the gain loss due to zero-stuffing, gives unitary gain after upsampling
    }
    for(int n=3; n<5; n++){
      upCoeffs[n]=downCoeffs[n];
    }
    downfilter->setCoefficients(FloatArray(downCoeffs,5));
    upfilter->setCoefficients(FloatArray(upCoeffs,5));
  }
  void downsample(FloatArray input, FloatArray output){
    ASSERT(input.getSize()==output.getSize()*factor, "wrong size");
    downfilter->process(input, input.getSize());
    float* p = (float*)input;
    for(int i=0; i<input.getSize(); ++i){
      output[i] = *p;
      p += 4;
    }
  }
  void upsample(FloatArray input, FloatArray output){
    ASSERT(input.getSize()*factor==output.getSize(), "wrong size");
    float* p = output;
    for(int i=0; i<input.getSize(); ++i){
      *p++ = input[i];
      *p++ = 0;
      *p++ = 0;
      *p++ = 0;
    }
    upfilter->process(output, output.getSize());
  }
};