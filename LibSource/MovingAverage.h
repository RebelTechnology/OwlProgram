#ifndef __MovingAverage_h__
#define __MovingAverage_h__

#include "SignalProcessor.h"

/**
 * Exponential Moving Average EMA
 */
class ExponentialMovingAverage : public SignalProcessor {
private:
  float lambda;
  float y;
public:  
  ExponentialMovingAverage(float lambda = 0.995, float y=0): lambda(lambda), y(y) {}
  void set(float value){
    y = value;
  }
  /**
   * Set the exponential decay time coefficient. Lambda should be a value
   * between 0 and 1, typically greater than 0.9. 
   * Higher values gives more smoothing.
   */
  void setLambda(float value){
    lambda = value;
  }
  float getLambda(){
    return lambda;
  }
  /**
   * Set lambda (time coefficient) as amount of smoothing over number of 
   * observations.
   * With smoothing = 2.0 and observations = N, this corresponds roughly to 
   * an N-point SMA.
   */
  void setSmoothing(size_t observations, float smoothing = 2){
    lambda = smoothing / (observations + 1);
  }
  /**
   * Set lambda (time coefficient) as 1/N.
   * Known as a Modified Moving Average MMA, Smoothed Moving Average SMMA, or 
   * Running Moving Average RMA.
   */
  void setModifiedMovingAverage(size_t N){
    lambda = 1/N;
  }
  /**
   * Set lambda (time coefficient) as the -3dB cutoff frequency, in radians.
   * (i.e. radians = pi times frequency / sample rate).
   * Ref: https://dsp.stackexchange.com/questions/28308/exponential-weighted-moving-average-time-constant
   */
  void setCutoff(float fc){
    float cfc = cosf(fc);
    lambda = cfc - 1 + sqrtf(cfc*cfc - 4*cfc + 3);
  }
  float getNextAverage(float x){
    y = y*lambda + x*(1.0f - lambda);
    return y;
  }
  /* process a single sample and return the result */
  float process(float in){
    return getNextAverage(in);
  }
  using SignalProcessor::process;
  static ExponentialMovingAverage* create(float lambda){
    return new ExponentialMovingAverage(lambda);
  }
  static void destroy(ExponentialMovingAverage* obj){
    delete obj;
  }
};

/**
 * Simple Moving Average SMA
 */
class SimpleMovingAverage : public SignalProcessor {
private:
  FloatArray buffer;
  size_t writeIndex = 0;
  float sum = 0;
public:
  SimpleMovingAverage() {}
  SimpleMovingAverage(FloatArray buf) : buffer(buf), sum(buf.getSum()) {}
  void reset(){
    buffer.clear();
    sum = 0;
  }
  void set(float value){
    buffer.setAll(value);
    sum = value * buffer.getSize();
  }
  float getNextSum(float value){
    sum = sum - buffer[writeIndex] + value;
    buffer[writeIndex] = value;
    // sum = sum - head() + value;
    // write(value);
    if(++writeIndex == buffer.getSize())
      writeIndex = 0;
    return sum;
  }
  float getNextAverage(float push){
    return getNextSum(push)/buffer.getSize();
  }
  size_t getSize(){
    return buffer.getSize();
  }
  float process(float in){
    return getNextAverage(in);
  }
  using SignalProcessor::process;
  static SimpleMovingAverage* create(int samples){
    return new SimpleMovingAverage(FloatArray::create(samples));
  }
  static void destroy(SimpleMovingAverage* buf){
    FloatArray::destroy(buf->buffer);
    delete buf;
  }
};

#endif // __MovingAverage_h__
