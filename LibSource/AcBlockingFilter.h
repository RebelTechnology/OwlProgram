#ifndef __AcBlockingFilter_h__
#define __AcBlockingFilter_h__

#include "FloatArray.h"
#include "SignalProcessor.h"

/**
 * AC Blocking IIR filter, the Leaky Integrator. Used for signal smoothing.
 */
class AcBlockingFilter : public SignalProcessor {
private:
  const float lambda;
  float y;
public:
  AcBlockingFilter(float lambda = 0.995): lambda(lambda), y(0) {}

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

  static AcBlockingFilter* create(float lambda){
    return new AcBlockingFilter(lambda);
  }

  static void destroy(AcBlockingFilter* obj){
    delete obj;
  }
};

class StereoAcBlockingFilter : public MultiSignalProcessor {
private:
  AcBlockingFilter left, right;
public:
  StereoAcBlockingFilter(float lambda = 0.995): left(lambda), right(lambda) {}
  void process(AudioBuffer& input, AudioBuffer& output){
    left.process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    right.process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }

  static StereoAcBlockingFilter* create(float lambda){
    return new StereoAcBlockingFilter(lambda);
  }

  static void destroy(StereoAcBlockingFilter* obj){
    delete obj;
  }
};

#endif // __AcBlockingFilter_h__
