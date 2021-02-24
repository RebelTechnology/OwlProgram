#ifndef _CircularBuffer_hpp_
#define _CircularBuffer_hpp_

#include <stdint.h>
#include <string.h> // for memcpy

template<typename T = float>
class CircularBuffer {
private:
  T* data;
  size_t size;
  volatile size_t writepos = 0;
  volatile size_t readpos = 0;
public:
  CircularBuffer(): data(NULL), size(0){}
  CircularBuffer(T* data, size_t size): data(data), size(size){}

  size_t getSize() const {
    return size;
  }
  
  bool isEmpty() const {
    return writepos == readpos;
  }

  void write(T c){
    data[writepos++] = c;
    if(writepos >= size)
      writepos = 0;
  }

  void write(T* data, size_t len){
    ASSERT(getWriteCapacity() < len, "CircularBuffer overflow");
    T* dest = getWriteHead();
    size_t rem = size-writepos;
    if(len >= rem){
      memcpy(dest, data, rem);
      // note that len-rem may be zero
      memcpy(data, data+rem, len-rem);
      writepos = len-rem;
    }else{
      memcpy(dest, data, len);
      writepos += len;
    }
  }
    
  void writeAt(size_t index, T value){
    data[index % size] = value;
  }

  /**
   * Interpolated write at sub-sample index.
   * Inserts a value linearly interpolated at a fractional index.
   */
  void interpolatedWriteAt(float index, T value){
    size_t idx = (size_t)index;
    T low = readAt(idx);
    T high = readAt(idx+1);
    float frac = index - idx;
    writeAt(idx, low + (value-low)*frac);
    writeAt(idx+1, value + (high-value)*frac);
  }

  T read(){
    T c = data[readpos++];
    if(readpos >= size)
      readpos = 0;
    return c;
  }

  void read(T* data, size_t len){
    ASSERT(getReadCapacity() < len, "CircularBuffer underflow");
    T* src = getReadHead();
    size_t rem = size-readpos;
    if(len >= rem){
      memcpy(data, src, rem);
      // note that len-rem may be zero
      memcpy(data+rem, data, len-rem);
      readpos = len-rem;
    }else{
      memcpy(data, src, len);
      readpos += len;
    }
  }
  
  T readAt(size_t index){
    return data[index % size];
  }
  
  /**
   * Interpolated read at sub-sample index.
   * @return a value linearly interpolated at a fractional index
   */
  inline float interpolatedReadAt(float index){
    size_t idx = (size_t)index;
    T low = readAt(idx);
    T high = readAt(idx+1);
    float frac = index - idx;
    return high+frac*(low-high);
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

  void incrementWriteHead(size_t len){
    ASSERT(getWriteCapacity() < len, "CircularBuffer overflow");
    writepos += len;
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

  void incrementReadHead(size_t len){
    ASSERT(getReadCapacity() < len, "CircularBuffer underflow");
    readpos += len;
    if(readpos >= size)
      readpos -= size;
  }

  /**
   * Set the read pos @param samples behind the write position.
   */
  void setDelay(int samples){
    readpos = (writepos-samples) % size;
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

#endif /* _CircularBuffer_hpp_ */
