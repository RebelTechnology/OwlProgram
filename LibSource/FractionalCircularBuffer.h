#ifndef _FractionalCircularBuffer_hpp_
#define _FractionalCircularBuffer_hpp_

#include <stdint.h>

/**
 * Circular buffer that keeps a delta table of differences for faster fractional delay lines.
 * Note that this class requires twice as much buffer space as CircularBuffer.
 */
template<typename T = float>
class FractionalCircularBuffer {
private:
  T* data;
  T* delta;
  const size_t size;
  size_t writepos = 0;
  float readpos = 0;

protected:
  /**
   * Update an entry in the internal data and delta tables
   */
  inline T update(T previous, T current, size_t i){
    delta[i] = previous - current;
    data[i] = current;
    return current;
  }
public:
  FractionalCircularBuffer(): data(NULL), delta(NULL), size(0){}
  FractionalCircularBuffer(T* data, T* delta, size_t size) : data(data), delta(delta), size(size){}

  size_t getSize() const {
    return size;
  }

  T* getData() {
    return data;
  }

  T* getDelta() {
    return delta;
  }
  
  bool isEmpty() const {
    return abs(writepos-readpos) < 1;
  }

  void write(T value){
    size_t pos = (writepos-1+size) % size;
    update(data[pos], value, writepos);
    if(++writepos >= size)
      writepos = 0;
  }

  void write(T* src, size_t len){
    T previous = data[(writepos-1+size) % size];
    size_t rem = size-writepos;
    if(len > rem){
      for(size_t i=writepos; i<size; ++i)
	previous = update(previous, *src++, i);
      writepos = len-rem;
      for(size_t i=0; i<writepos; ++i)
	previous = update(previous, *src++, i);
    }else{
      size_t i = writepos;
      writepos += len;
      for(; i<writepos; ++i)
	previous = update(previous, *src++, i);
    }
  }
    
  void writeAt(size_t index, T value){
    size_t pos = (index-1+size) % size;
    update(data[pos], value, index);
  }

  T read(){
    size_t idx = (size_t)readpos;
    T current = data[idx];
    T diff = delta[idx];
    float fraction = readpos-idx;
    readpos += 1;
    if(readpos >= size)
      readpos -= size;
    return current + fraction*diff;
  }

  void read(T* dst, size_t len){
    size_t idx = (size_t)readpos;
    float fraction = 1 - (readpos-idx);
    idx = (idx+1) % size;

    size_t rem = size-idx;
    T* pdata = data+idx;
    T* pdelta = delta+idx;
    if(len > rem){
      readpos += len - size;
      for(size_t i=idx; i<size; ++i)
	*dst++ = *pdata++ + fraction*(*pdelta++);
      idx = (size_t)readpos;
      for(size_t i=0; i<idx; ++i)
	*dst++ = *pdata++ + fraction*(*pdelta++);	
    }else{
      readpos += len;
      while(len--)
      	*dst++ = *pdata++ + fraction*(*pdelta++);
    }
  }
  
  T readAt(float index){
    size_t idx = (size_t)index;
    float fraction = 1 - (index-idx);
    idx = (idx+1) % size;
    return data[idx] + fraction*delta[idx];
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

  void moveWriteHead(int samples){
    writepos += samples;
    if(writepos >= size)
      writepos -= size;
  }

  float getReadIndex(){
    return readpos;
  }

  void setReadIndex(float pos){
    readpos = fmodf(pos+size, size);
  }

  T* getReadHead(){
    return data+(size_t)readpos;
  }

  void moveReadHead(float samples){
    setReadIndex(readpos+samples);
  }

  /**
   * Set the read index @param samples behind the write index.
   */
  void setDelay(float samples){
    setReadIndex(writepos-samples);
  }
  
  /**
   * Get the read index expressed as delay behind the write index.
   */
  float getDelay(){
    return fmodf(writepos-readpos+size, size);
  }

  /**
   * Write to buffer and read with a delay
   */
  void delay(T* in, T* out, size_t len, float delay){
    setDelay(delay);
    write(in, len);
    read(out, len);
  }
  
  /**
   * Write to buffer and read with a delay that ramps up or down
   * from @param beginDelay to @param endDelay
   */
  void delay(T* in, T* out, size_t len, float beginDelay, float endDelay){
    setDelay(beginDelay);
    float pos = readpos;
    float incr = (len+endDelay-beginDelay)/len;
    T previous = data[(writepos-1+size) % size];
    size_t rem = size-writepos;
    if(len > rem){
      for(size_t i=writepos; i<size; ++i){
	previous = update(previous, *in++, i);
	*out++ = readAt(pos);
	pos += incr;
      }
      writepos = len-rem;
      for(size_t i=0; i<writepos; ++i){
	previous = update(previous, *in++, i);
	*out++ = readAt(pos);
	pos += incr;
      }
    }else{
      size_t i = writepos;
      writepos += len;
      for(; i<writepos; ++i){
	previous = update(previous, *in++, i);
	*out++ = readAt(pos);
	pos += incr;
      }
    }
    setDelay(endDelay);
  }

  size_t getReadCapacity(){
    return (writepos + size - (size_t)readpos) % size;
  }

  size_t getWriteCapacity(){
    return size - getReadCapacity();
  }

  size_t getContiguousWriteCapacity(){
    if(writepos < (size_t)readpos)
      return (size_t)readpos - writepos;
    else
      return size - writepos;
  }

  size_t getContiguousReadCapacity(){
    if(writepos < (size_t)readpos)
      return size - (size_t)readpos;
    else
      return writepos - (size_t)readpos;
  }

  void setAll(const T value){
    for(size_t i=0; i<size; ++i){
      data[i] = value;
      delta[i] = value;
    }
  }

  void reset(){
    readpos = writepos = 0;
  }

  void clear(){
    setAll(0);
  }

  static FractionalCircularBuffer<T>* create(size_t len){
    FractionalCircularBuffer<T>* obj = new FractionalCircularBuffer<T>(new T[len], new T[len], len);
    obj->clear();
    return obj;
  }

  static void destroy(FractionalCircularBuffer<T>* obj){
    delete[] obj->data;
    delete[] obj->delta;
    delete obj;
  }
};

typedef FractionalCircularBuffer<float> FractionalCircularFloatBuffer;
typedef FractionalCircularBuffer<int16_t> FractionalCircularShortBuffer;
typedef FractionalCircularBuffer<int32_t> FractionalCircularIntBuffer;

#endif /* _FractionalCircularBuffer_hpp_ */
