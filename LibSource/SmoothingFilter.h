#ifndef __SmoothingFilter_h__
#define __SmoothingFilter_h__

#include "FloatArray.h"
#include "SignalProcessor.h"

/**
 * AC Blocking IIR filter, the Leaky Integrator. Used for signal smoothing.
 */
class SmoothingFilter : public SignalProcessor {
private:
  const float lambda;
  float y;
public:
  SmoothingFilter(float lambda = 0.995): lambda(lambda), y(0) {}

  /* process a single sample and return the result */
  float process(float x){
    y = y*lambda + x*(1.0f - lambda);
    return y;
  }
  
  void process(float* input, float* output, size_t size){
    float x;
    while(size--){
      x = *input++;
      y = y*lambda + x*(1.0f - lambda);
      *output++ = y;
    }
  }
  
  /* perform in-place processing */
  void process(float* buf, int size){
    process(buf, buf, size);
  }

  void process(FloatArray in){
    process(in, in, in.getSize());
  }

  void process(FloatArray in, FloatArray out){
    ASSERT(out.getSize() >= in.getSize(), "output array must be at least as long as input");
    process(in, out, in.getSize());
  }

  static SmoothingFilter* create(float lambda){
    return new SmoothingFilter(lambda);
  }

  static void destroy(SmoothingFilter* obj){
    delete obj;
  }
};

class StereoSmoothingFilter : public MultiSignalProcessor {
private:
  SmoothingFilter left, right;
public:
  StereoSmoothingFilter(float lambda = 0.995): left(lambda), right(lambda) {}
  void process(AudioBuffer& input, AudioBuffer& output){
    left.process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    right.process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }

  static StereoSmoothingFilter* create(float lambda){
    return new StereoSmoothingFilter(lambda);
  }

  static void destroy(StereoSmoothingFilter* obj){
    delete obj;
  }
};

#endif // __SmoothingFilter_h__
