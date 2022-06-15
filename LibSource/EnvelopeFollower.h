#ifndef __EnvelopeFollower_h__
#define __EnvelopeFollower_h__

#include "SignalProcessor.h"

/**
 * Envelope follower. Rectifies and smooths the input signal
 * to track changes in intensity over time.
 */
class EnvelopeFollower : public SignalProcessor {
private:
  const float lambda;
  float y;
public:  
  EnvelopeFollower(float lambda = 0.995): lambda(lambda), y(0) {}
  float process(float x){
    y = y*lambda + abs(x)*(1.0f - lambda);
    return y;
  }
  void process(FloatArray in, FloatArray out){
    float* input = in.getData();
    float* output = out.getData();
    size_t len = in.getSize();
    while(len--){
      y = y*lambda + abs(*input++)*(1.0f - lambda);
      *output++ = y;
    }
  }
  static EnvelopeFollower* create(float lambda){
    return new EnvelopeFollower(lambda);
  }
  static void destroy(EnvelopeFollower* obj){
    delete obj;
  }
};

#endif // __EnvelopeFollower_h__
