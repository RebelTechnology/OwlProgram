#include "BiquadFilter.hpp"
#
/**
   Implements 4x oversampling
*/  
class Downsample {
private:
  BiquadFilter *downfilter;
  int factor;
  int stages;
public:
  Downsample(){
    // two filters: same coefficients, different state variables
    factor=4;
    stages=1;
    init();
  }
  void init(){
    downfilter=BiquadFilter::create(stages);
    // [B, A]=cheby1(2, 2, 0.25); matlabToC([B, -A(2:end)])
    static float coeffs[5]={0.07609109, 0.15218218, 0.07609109, +1.16511283,  -0.54828486};
    for(int n=0; n<3; n++){
      coeffs[n]*=factor; //compensate for the gain loss due to zero-stuffing
    }
    downfilter->setCoefficients(FloatArray(coeffs,5));
  }
  float* getCoefficients(int stage){
    return downfilter->getFilterStage(stage).getCoefficients();
  }
  void down(FloatArray input, FloatArray output){
    ASSERT(input.getSize()==output.getSize()*factor, "wrong size");
    downfilter->process(input, input.getSize());
    float* p = (float*)input;
    for(int i=0; i<input.getSize(); ++i){
      output[i] = *p;
      p += 4;
    }
  }
};

  
class Upsample {
private:
  BiquadFilter *upfilter;
  int factor;
  int stages;
public:
  Upsample(){
    factor=4;
    stages=1;
    init();
  }
  void init(){
    upfilter=BiquadFilter::create(stages);
    // [B, A]=cheby1(2, 2, 0.25); matlabToC([B, -A(2:end)])
    static float coeffs[5]={0.07609109, 0.15218218, 0.07609109, +1.16511283, -0.54828486};
    for(int n=0; n<3; n++){
      coeffs[n]*=factor; //compensate for the gain loss due to zero-stuffing
    }
    upfilter->setCoefficients(FloatArray(coeffs,5));
  }
  float* getCoefficients(int stage){
    return upfilter->getFilterStage(stage).getCoefficients();
  }
  void up(FloatArray input, FloatArray output){
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