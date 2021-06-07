#ifndef __DelayProcessor_h__
#define __DelayProcessor_h__

#include "CircularBuffer.h"
#include "SignalProcessor.h"
#include "FractionalCircularBuffer.h"

/**
 * Delay line signal processor implemented with a circular buffer.
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
 * Delay line signal processor implemented with a circular buffer, allowing fractional delay times.
 */
template<InterpolationMethod im = LINEAR_INTERPOLATION>
class FractionalDelayProcessor : public SignalProcessor {
protected:
  // CircularFloatBuffer buffer;
  InterpolatingCircularFloatBuffer<im> buffer;
  float delay;
public:
  FractionalDelayProcessor() : delay(0) {}
  FractionalDelayProcessor(float* buffer, size_t len) : buffer(buffer, len), delay(0) {}
  float getDelay(){
    return delay;
  }
  void setDelay(float samples){
    delay = samples;
  }
  float process(float input){
    buffer.write(input);
    return buffer.readAt(buffer.getWriteIndex()-delay);
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
  }
  float process(float input){
    buffer.setDelay(delay);
    buffer.write(input);
    return buffer.read();
  }
  void process(FloatArray input, FloatArray output){
    smooth(input, output, delay);
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

/**
 * Delay line signal processor that crossfades to smooth changes in delay time.
 */
class CrossFadingDelayProcessor : public SignalProcessor {
protected:
  CircularFloatBuffer* ringbuffer;
  FloatArray buffer;
  float delay = 0;
public:
  CrossFadingDelayProcessor(CircularFloatBuffer* ringbuffer, FloatArray buffer)
    : ringbuffer(ringbuffer), buffer(buffer) {}  
  float getDelay(){
    return delay;
  }
  void setDelay(float samples){
    delay = samples;
  }
  float process(float input){
    ringbuffer->write(input);
    float sample = ringbuffer->read();
    ringbuffer->setDelay(delay);
    sample += ringbuffer->read();
    return sample*0.5;
  }
  void process(FloatArray input, FloatArray output){
    smooth(input, output, delay);
  }
  /**
   * Delay ramping smoothly from the previous delay time to @param newDelay
   */
  void smooth(FloatArray input, FloatArray output, float newDelay){
    ASSERT(buffer.getSize() >= input.getSize(), "buffer array too small");
    ASSERT(output.getSize() >= input.getSize(), "output array too small");
    size_t len = input.getSize();
    ringbuffer->write(input.getData(), len);
    ringbuffer->read(buffer.getData(), len); // read at previous delay position
    buffer.scale(1, 0); // ramp down
    ringbuffer->setDelay(newDelay);
    ringbuffer->read(output.getData(), len); // read at new delay position
    output.scale(0, 1); // ramp up
    output.add(buffer);
    delay = newDelay;
  }
  static CrossFadingDelayProcessor* create(size_t delay_len, size_t buffer_len){
    return new CrossFadingDelayProcessor(CircularFloatBuffer::create(delay_len),
					 FloatArray::create(buffer_len));
  }
  static void destroy(CrossFadingDelayProcessor* obj){
    CircularFloatBuffer::destroy(obj->ringbuffer);
    FloatArray::destroy(obj->buffer);
    delete obj;
  }
};

#endif // __DelayProcessor_h__
