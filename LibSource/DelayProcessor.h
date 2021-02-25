#ifndef __DelayProcessor_h__
#define __DelayProcessor_h__

#include "CircularBuffer.h"
#include "SignalProcessor.h"
#include "FractionalCircularBuffer.h"

/**
 * Delay line signal processor implemented with a circular buffer
 */
class DelayProcessor : public SignalProcessor {
protected:
  CircularFloatBuffer buffer;
  size_t delay;
public:
  DelayProcessor() : delay(0) {}
  DelayProcessor(float* buffer, size_t len) : buffer(buffer, len), delay(0) {}
  size_t getDelay(){
    return delay;
  }
  void setDelay(size_t samples){
    delay = samples;
    buffer.setDelay(samples);
  }
  float process(float input){
    buffer.write(input);
    return buffer.read();
  }
  void process(FloatArray input, FloatArray output){
    buffer.delay(input, output, input.getSize(), delay);
  }
  static DelayProcessor* create(size_t len){
    return new DelayProcessor(new float[len], len);
  }
  static void destroy(DelayProcessor* obj){
    delete[] obj->buffer.getData();
    delete obj;
  }
};

/**
 * Delay line signal processor implemented with a circular buffer
 */
class FractionalDelayProcessor : public SignalProcessor {
protected:
  CircularFloatBuffer buffer;
  float delay;
public:
  FractionalDelayProcessor() : delay(0) {}
  FractionalDelayProcessor(float* buffer, size_t len) : buffer(buffer, len), delay(0) {}
  float getDelay(){
    return delay;
  }
  void setDelay(float samples){
    delay = samples;
    buffer.setDelay(samples);
  }
  float process(float input){
    buffer.write(input);
    return buffer.interpolatedReadAt(buffer.getWriteIndex()+delay);
  }
  using SignalProcessor::process;
  /**
   * Delay ramping smoothly from the previous delay time to @param newDelay
   */
  void smooth(FloatArray input, FloatArray output, float newDelay){
    buffer.interpolatedDelay(input, output, input.getSize(), delay, newDelay);
    delay = newDelay;
  }
  static FractionalDelayProcessor* create(size_t len){
    return new FractionalDelayProcessor(new float[len], len);
  }
  static void destroy(FractionalDelayProcessor* obj){
    delete[] obj->buffer.getData();
    delete obj;
  }
};

/**
 * Interpolating delay line signal processor with fractional delay times.
 */
class FastFractionalDelayProcessor : public SignalProcessor {
protected:
  FractionalCircularFloatBuffer buffer;
  float delay;
public:
  FastFractionalDelayProcessor() : delay(0) {}
  FastFractionalDelayProcessor(float* buffer, float* delta, size_t len) : buffer(buffer, delta, len), delay(0) {}
  float getDelay(){
    return delay;
  }
  void setDelay(float samples){
    delay = samples;
    buffer.setDelay(samples);
  }
  float process(float input){
    buffer.write(input);
    return buffer.read();
  }
  void process(FloatArray input, FloatArray output){
    buffer.delay(input, output, input.getSize(), delay);
  }
  /**
   * Delay ramping smoothly from the previous delay time to @param newDelay
   */
  void smooth(FloatArray input, FloatArray output, float newDelay){
    buffer.delay(input, output, input.getSize(), delay, newDelay);
    delay = newDelay;
  }
  static FastFractionalDelayProcessor* create(size_t len){
    return new FastFractionalDelayProcessor(new float[len], new float[len], len);
  }
  static void destroy(FastFractionalDelayProcessor* obj){
    delete[] obj->buffer.getData();
    delete[] obj->buffer.getDelta();
    delete obj;
  }
};

#endif // __DelayProcessor_h__
