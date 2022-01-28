#ifndef __DcBlockingFilter_h__
#define __DcBlockingFilter_h__

#include "FloatArray.h"
#include "SignalProcessor.h"

/**
 * DC Blocking IIR filter, aka Leaky differentiator.
 * Ref: https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
 */
class DcBlockingFilter : public SignalProcessor {
private:
  float x1, y1;
  float R;
public:
  DcBlockingFilter(float R = 0.995): R(R), x1(0), y1(0) {}

  /**
   * Get adaptation time constant in samples.
   */
  float getTimeConstant(){
    return 1/(1-R); // approximate
  }
  /**
   * Set adaptation time constant in samples.
   */
  void setTimeConstant(float tc){
    R = (tc - 1) / tc;
  }
  void reset(){
    x1 = y1 = 0;
  }

  /* process a single sample and return the result */
  float process(float x){
    y1 = x - x1 + R*y1;
    x1 = x;
    return y1;
  }
  
  void process(float* input, float* output, size_t size){
    float x;
    float y = y1;
    while(size--){
      x = *input++;
      y = x - x1 + R*y;
      x1 = x;
      *output++ = y;
    }
    y1 = y;
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

  static DcBlockingFilter* create(float R=0.995){
    return new DcBlockingFilter(R);
  }

  static void destroy(DcBlockingFilter* obj){
    delete obj;
  }
};

class StereoDcBlockingFilter : public MultiSignalProcessor {
private:
  DcBlockingFilter left, right;
public:
  StereoDcBlockingFilter(float R = 0.995): left(R), right(R) {}

  /**
   * Get adaptation time constant in samples.
   */
  float getTimeConstant(){
    return left.getTimeConstant();
  }
  /**
   * Set adaptation time constant in samples.
   */
  void setTimeConstant(float tc){
    left.setTimeConstant(tc);
    right.setTimeConstant(tc);
  }

  void process(AudioBuffer& input, AudioBuffer& output){
    left.process(input.getSamples(LEFT_CHANNEL), output.getSamples(LEFT_CHANNEL));
    right.process(input.getSamples(RIGHT_CHANNEL), output.getSamples(RIGHT_CHANNEL));
  }

  static StereoDcBlockingFilter* create(float R=0.995){
    return new StereoDcBlockingFilter(R);
  }

  static void destroy(StereoDcBlockingFilter* obj){
    delete obj;
  }
};

#endif // __DcBlockingFilter_h__
