#ifndef __DelayProcessor_h__
#define __DelayProcessor_h__

#include "CircularBuffer.h"
#include "SignalProcessor.h"
#include "FastInterpolatedCircularBuffer.h"

/**
 * Delay line signal processor implemented with a circular buffer
 */
class DelayProcessor : public SignalProcessor {
protected:
  CircularFloatBuffer buffer;
  int delay;
public:
  DelayProcessor() : delay(0) {}
  DelayProcessor(float* buffer, size_t len) : buffer(buffer, len), delay(0) {}
  int getDelay(){
    return delay;
  }
  void setDelay(int samples){
    delay = samples;
    buffer.setDelay(samples);
  }
  float process(float input){
    buffer.write(input);
    return buffer.read();
  }
  // using SignalProcessor::process;
  void process(FloatArray input, FloatArray output){
    buffer.write(input, input.getSize());
    buffer.read(output, output.getSize());
    // buffer.delay(input, output, input.getSize(), delay);
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
 * Interpolating delay line signal processor with fractional delay times.
 */
class FractionalDelayProcessor : public SignalProcessor {
protected:
  FastInterpolatedCircularFloatBuffer buffer;
  float delay;
public:
  FractionalDelayProcessor() : delay(0) {}
  FractionalDelayProcessor(float* buffer, float* delta, size_t len) : buffer(buffer, delta, len), delay(0) {}
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
  static FractionalDelayProcessor* create(size_t len){
    return new FractionalDelayProcessor(new float[len], new float[len], len);
  }
  static void destroy(FractionalDelayProcessor* obj){
    delete[] obj->buffer.getData();
    delete[] obj->buffer.getDelta();
    delete obj;
  }
};

#endif // __DelayProcessor_h__
