#ifndef __Resample_h__
#define __Resample_h__

#include "SignalProcessor.h"
#include "BiquadFilter.h"

/**
 * Implements 4x oversampling
 */
class [[deprecated("use UpSampler and DownSampler instead.")]] Resampler {
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
    downfilter->copyCoefficients(FloatArray(downCoeffs,5));
    upfilter->copyCoefficients(FloatArray(upCoeffs,5));
  }
  void downsample(FloatArray input, FloatArray output){
    ASSERT(input.getSize()==output.getSize()*factor, "wrong size");
    downfilter->process(input, input);
    float* p = (float*)input;
    for(size_t i=0; i<input.getSize(); ++i){
      output[i] = *p;
      p += 4;
    }
  }
  void upsample(FloatArray input, FloatArray output){
    ASSERT(input.getSize()*factor==output.getSize(), "wrong size");
    float* p = output;
    for(size_t i=0; i<input.getSize(); ++i){
      *p++ = input[i];
      *p++ = 0;
      *p++ = 0;
      *p++ = 0;
    }
    upfilter->process(output, output);
  }
};

class UpSampler : public SignalProcessor {
private:
  BiquadFilter* filter;
  const size_t factor;
public:
  UpSampler(BiquadFilter* filter, int factor=4): filter(filter), factor(factor) {}
  static UpSampler* create(int stages, int factor=4){
    BiquadFilter* filter = BiquadFilter::create(stages);
    // [B, A]=cheby1(2, 2, 0.25); then use [B, -A(2:end)] , note the minus sign in front of the A coefficients!!!!
    //alternatively, we could use FilterStage to compute the coefficients
    float upCoeffs[5]= {0.07609109, 0.15218218, 0.07609109, +1.16511283,  -0.54828486};
    for(int n=0; n<3; n++)
      upCoeffs[n] *= factor; //compensate for the gain loss due to zero-stuffing, gives unitary gain after upsampling
    filter->copyCoefficients(FloatArray(upCoeffs, 5));
    return new UpSampler(filter, factor);
  }
  static void destroy(UpSampler* obj){
    BiquadFilter::destroy(obj->filter);
    delete obj;
  }
  float process(float input){
    return filter->process(input); // this doesn't really make sense
  }
  /**
   * Up-sample param @input and put results into @param output. Note: in-place processing not supported
   */
  void process(FloatArray input, FloatArray output){
    ASSERT(input.getSize()*factor==output.getSize(), "wrong size");
    float* p = output;
    output.clear();
    for(size_t i=0; i<input.getSize(); i+= factor)
      *p++ = input[i];
    filter->process(output, output);
  }
};

class DownSampler : public SignalProcessor {
private:
  BiquadFilter* filter;
  const size_t factor;
public:
  DownSampler(BiquadFilter* filter, int factor=4): filter(filter), factor(factor) {}
  static DownSampler* create(int stages, int factor=4){
    BiquadFilter* filter = BiquadFilter::create(stages);
    // [B, A]=cheby1(2, 2, 0.25); then use [B, -A(2:end)] , note the minus sign in front of the A coefficients!!!!
    //alternatively, we could use FilterStage to compute the coefficients
    static float downCoeffs[5]={0.07609109, 0.15218218, 0.07609109, +1.16511283,  -0.54828486};
    filter->copyCoefficients(FloatArray(downCoeffs,5));
    return new DownSampler(filter, factor);
  }
  static void destroy(DownSampler* obj){
    BiquadFilter::destroy(obj->filter);
    delete obj;
  }
  float process(float input){
    return filter->process(input); // this doesn't really make sense
  }
  /**
   * Down-sample param @input and put results into @param output. Note: in-place processing not supported
   */
  void process(FloatArray input, FloatArray output){
    ASSERT(input.getSize()==output.getSize()*factor, "wrong size");
    filter->process(input, input);
    float* p = (float*)input;
    for(size_t i=0; i<input.getSize(); ++i){
      output[i] = *p;
      p += factor;
    }
  }
};

#endif /* __Resample_h__ */
