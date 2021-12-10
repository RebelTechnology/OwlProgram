#ifndef _CircularBuffer_hpp_
#define _CircularBuffer_hpp_

#include <stdint.h>
#include <string.h> // for memcpy

#ifndef FLOW_ASSERT
#define FLOW_ASSERT(x, y)
#endif

template<typename T>
class CircularBuffer {
protected:
  T* data;
  size_t size;
  size_t writepos = 0;
  size_t readpos = 0;
  bool empty = true;
public:
  CircularBuffer(): data(NULL), size(0){}
  CircularBuffer(T* data, size_t size): data(data), size(size){}

  void setData(T* data, size_t len) {
    this->data = data;
    size = len;
    empty;
  }

  size_t getSize() const {
    return size;
  }

  T* getData() {
    return data;
  }
  
  bool isEmpty() const {
    return empty;
  }
  
  bool isFull() const {
    return (writepos == readpos) && !empty;
  }

  void write(T c){
    FLOW_ASSERT(getWriteCapacity() > 0, "overflow");
    data[writepos++] = c;
    if(writepos >= size)
      writepos = 0;
    empty = false;
  }

  void write(T* source, size_t len){
    FLOW_ASSERT(getWriteCapacity() >= len, "overflow");
    T* dest = getWriteHead();
    size_t rem = size-writepos;
    if(len >= rem){
      memcpy(dest, source, rem*sizeof(T));
      writepos = len-rem;
      memcpy(data, source+rem, writepos*sizeof(T));
    }else{
      memcpy(dest, source, len*sizeof(T));
      writepos += len;
    }
    empty = false;
  }
    
  void writeAt(size_t index, T value){
    data[index % size] = value;
  }

  void overdub(T c){
    data[writepos++] += c;
    if(writepos >= size)
      writepos = 0;
    empty = false;
  }

  void overdubAt(size_t index, T value){
    data[index % size] += value;
  }

  T read(){
    FLOW_ASSERT(getReadCapacity() > 0, "underflow");
    T c = data[readpos++];
    if(readpos >= size)
      readpos = 0;
    empty = readpos == writepos;
    return c;
  }

  void read(T* dst, size_t len){
    FLOW_ASSERT(getReadCapacity() >= len, "underflow");
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
    empty = readpos == writepos;
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
    empty = readpos == writepos;
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

  void moveWriteHead(int32_t samples){
    FLOW_ASSERT(getWriteCapacity() >= samples, "overflow");
    writepos = (writepos + samples) % size;
    empty = false;
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

  void moveReadHead(int32_t samples){
    FLOW_ASSERT(getReadCapacity() < samples, "underflow");
    readpos = (readpos + samples) % size;
    empty = readpos == writepos;
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
  size_t getDelay() const {
    return (writepos-readpos+size) % size;
  }

  /**
   * Write to buffer and read with a delay
   */
  void delay(T* in, T* out, size_t len, int delay_samples){
    setDelay(delay_samples); // set delay relative to where we start writing
    write(in, len);
    read(out, len);
  }

  size_t getReadCapacity() const {
    return size - getWriteCapacity();
  }

  size_t getWriteCapacity() const {
    return size*empty + (readpos + size - writepos) % size;
  }

  size_t getContiguousWriteCapacity() const {
    if(writepos < readpos)
      return readpos - writepos;
    else
      return size - writepos;
  }

  size_t getContiguousReadCapacity() const {
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
    empty = true;
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
