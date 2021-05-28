#ifndef _CircularBuffer_hpp_
#define _CircularBuffer_hpp_

#include <stdint.h>
#include <string.h> // for memcpy
#include "Interpolator.h"

template<typename T>
class CircularBuffer {
protected:
  T* data;
  const size_t size;
  size_t writepos = 0;
  size_t readpos = 0;
public:
  CircularBuffer(): data(NULL), size(0){}
  CircularBuffer(T* data, size_t size): data(data), size(size){}

  size_t getSize() const {
    return size;
  }

  T* getData() {
    return data;
  }
  
  bool isEmpty() const {
    return writepos == readpos;
  }

  void write(T c){
    data[writepos++] = c;
    if(writepos >= size)
      writepos = 0;
  }

  void write(T* source, size_t len){
    ASSERT(getWriteCapacity() >= len, "overflow");
    T* dest = getWriteHead();
    size_t rem = size-writepos;
    if(len > rem){
      memcpy(dest, source, rem*sizeof(T));
      writepos = len-rem;
      memcpy(data, source+rem, writepos*sizeof(T));
    }else{
      memcpy(dest, source, len*sizeof(T));
      writepos += len;
    }
  }
    
  void writeAt(size_t index, T value){
    data[index % size] = value;
  }

  T read(){
    T c = data[readpos++];
    if(readpos >= size)
      readpos = 0;
    return c;
  }

  void read(T* dst, size_t len){
    ASSERT(getReadCapacity() >= len, "underflow");
    T* src = getReadHead();
    size_t rem = size-readpos;
    if(len > rem){
      memcpy(dst, src, rem*sizeof(T));
      readpos = len-rem;
      memcpy(dst+rem, data, readpos*sizeof(T));
    }else{
      memcpy(dst, src, len*sizeof(T));
      readpos += len;
    }
  }
  
  T readAt(size_t index){
    return data[index % size];
  }

  void skipUntilLast(char c){
    T* src = getReadHead();
    size_t rem = size-readpos;
    for(int i=0; i<rem; ++i){
      if(src[i] != c){
	readpos += i;
	return;
      }
    }
    rem = writepos;
    for(int i=0; i<rem; ++i){
      if(data[i] != c){
	readpos = i;
	return;
      }
    }
  }

  size_t getWriteIndex(){
    return writepos;
  }

  void setWriteIndex(size_t pos){
    writepos = pos % size;
  }

  T* getWriteHead(){
    return data+writepos;
  }

  void moveWriteHead(size_t samples){
    ASSERT(getWriteCapacity() < samples, "overflow");
    writepos += samples;
    if(writepos >= size)
      writepos -= size;
  }

  size_t getReadIndex(){
    return readpos;
  }

  void setReadIndex(size_t pos){
    readpos = pos % size;
  }

  T* getReadHead(){
    return data+readpos;
  }

  void moveReadHead(size_t samples){
    ASSERT(getReadCapacity() < samples, "underflow");
    readpos += samples;
    if(readpos >= size)
      readpos -= size;
  }

  /**
   * Set the read index @param samples behind the write index.
   */
  void setDelay(int samples){
    readpos = (writepos-samples+size) % size;
  }

  /**
   * Get the read index expressed as delay behind the write index.
   */
  int getDelay(){
    return (writepos-readpos+size) % size;
  }

  /**
   * Write to buffer and read with a delay
   */
  void delay(T* in, T* out, size_t len, int delay){
    setDelay(delay);
    write(in, len);
    read(out, len);
  }

  size_t getReadCapacity(){
    return (writepos + size - readpos) % size;
  }

  size_t getWriteCapacity(){
    return size - getReadCapacity();
  }

  size_t getContiguousWriteCapacity(){
    if(writepos < readpos)
      return readpos - writepos;
    else
      return size - writepos;
  }

  size_t getContiguousReadCapacity(){
    if(writepos < readpos)
      return size - readpos;
    else
      return writepos - readpos;
  }

  void setAll(const T value){
    for(size_t i=0; i<size; ++i)
      data[i] = value;
  }

  void reset(){
    readpos = writepos = 0;
  }

  void clear(){
    setAll(0);
  }

  static CircularBuffer<T>* create(size_t len){
    CircularBuffer<T>* obj = new CircularBuffer<T>(new T[len], len);
    obj->clear();
    return obj;
  }

  static void destroy(CircularBuffer<T>* obj){
    delete[] obj->data;
    delete obj;
  }
};

typedef CircularBuffer<float> CircularFloatBuffer;
typedef CircularBuffer<int16_t> CircularShortBuffer;
typedef CircularBuffer<int32_t> CircularIntBuffer;

template<InterpolationMethod im = LINEAR_INTERPOLATION>
class InterpolatingCircularFloatBuffer : public CircularFloatBuffer {
public:
  InterpolatingCircularFloatBuffer() {}
  InterpolatingCircularFloatBuffer(float* data, size_t size): CircularFloatBuffer(data, size) {}
  /**
   * Interpolated write at sub-sample index.
   * Inserts a value linearly interpolated at a fractional index.
   */
  void writeAt(float index, float value);

  /**
   * Interpolated read at sub-sample index.
   * @return a value linearly interpolated at a fractional index
   */
  float readAt(float index);

  /**
   * Read with a fractional delay
   */
  void delay(float* out, size_t len, float delay){
    float pos = fmodf(writepos-delay+size, size);
    while(len--){
      *out++ = readAt(pos);
      pos += 1;
    }
  }  
  
  /**
   * Write to buffer and read with a fractional delay
   * @note input buffer length is added to delay time
   */
  void delay(float* in, float* out, size_t len, float readDelay){
    write(in, len);
    delay(out, len, readDelay);
  }
  
  /**
   * Read with a delay that ramps up or down
   * from @param beginDelay to @param endDelay
   */
  void delay(float* out, size_t len, float beginDelay, float endDelay){
    float pos = fmodf(writepos-beginDelay+size, size);
    float incr = (len+endDelay-beginDelay)/len;
    while(len--){
      *out++ = readAt(pos);
      pos += incr;
    }
  }
  
  /**
   * Write to buffer and read with a delay that ramps up or down
   * from @param beginDelay to @param endDelay
   * @note input buffer length is added to delay times
   */
  void delay(float* in, float* out, size_t len, float beginDelay, float endDelay){
    write(in, len);
    delay(out, len, beginDelay, endDelay);
  }
};

template<>
float InterpolatingCircularFloatBuffer<LINEAR_INTERPOLATION>::readAt(float index){
  size_t idx = (size_t)index;
  float low = CircularFloatBuffer::readAt(idx);
  float high = CircularFloatBuffer::readAt(idx+1);
  float frac = index - idx;
  return Interpolator::linear(low, high, frac);
}

template<>
void InterpolatingCircularFloatBuffer<LINEAR_INTERPOLATION>::writeAt(float index, float value){
  size_t idx = (size_t)index;
  float low = CircularFloatBuffer::readAt(idx);
  float high = CircularFloatBuffer::readAt(idx+1);
  float frac = index - idx;
  writeAt(idx, low + (value-low)*frac);
  writeAt(idx+1, value + (high-value)*frac);
}

template<>
float InterpolatingCircularFloatBuffer<COSINE_INTERPOLATION>::readAt(float index){
  size_t idx = (size_t)index;
  float low = CircularFloatBuffer::readAt(idx);
  float high = CircularFloatBuffer::readAt(idx+1);
  float frac = index - idx;
  return Interpolator::cosine(low, high, frac);
}

template<>
float InterpolatingCircularFloatBuffer<CUBIC_3P_INTERPOLATION>::readAt(float index){
  size_t idx = (size_t)index;
  float y0 = CircularFloatBuffer::readAt(idx-1);
  float y1 = CircularFloatBuffer::readAt(idx);
  float y2 = CircularFloatBuffer::readAt(idx+1);
  return Interpolator::cubic(y0, y1, y2, index - idx);
}

template<>
float InterpolatingCircularFloatBuffer<CUBIC_4P_INTERPOLATION>::readAt(float index){
  size_t idx = (size_t)index;
  float y0 = CircularFloatBuffer::readAt(idx-1);
  float y1 = CircularFloatBuffer::readAt(idx);
  float y2 = CircularFloatBuffer::readAt(idx+1);
  float y3 = CircularFloatBuffer::readAt(idx+2);
  return Interpolator::cubic(y0, y1, y2, y3, index - idx);
}

template<>
float InterpolatingCircularFloatBuffer<CUBIC_4P_SMOOTH_INTERPOLATION>::readAt(float index){
  size_t idx = (size_t)index;
  float y0 = CircularFloatBuffer::readAt(idx-1);
  float y1 = CircularFloatBuffer::readAt(idx);
  float y2 = CircularFloatBuffer::readAt(idx+1);
  float y3 = CircularFloatBuffer::readAt(idx+2);
  return Interpolator::cubicSmooth(y0, y1, y2, y3, index - idx);
}

template<>
float InterpolatingCircularFloatBuffer<HERMITE_INTERPOLATION>::readAt(float index){
  size_t idx = (size_t)index;
  float y0 = CircularFloatBuffer::readAt(idx-1);
  float y1 = CircularFloatBuffer::readAt(idx);
  float y2 = CircularFloatBuffer::readAt(idx+1);
  float y3 = CircularFloatBuffer::readAt(idx+2);
  return Interpolator::hermite(y0, y1, y2, y3, index - idx);
}

#endif /* _CircularBuffer_hpp_ */
