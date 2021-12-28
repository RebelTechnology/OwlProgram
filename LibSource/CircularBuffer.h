#ifndef _CircularBuffer_hpp_
#define _CircularBuffer_hpp_

#include <stdint.h>
#include <string.h> // for memcpy

#ifndef FLOW_ASSERT
#define FLOW_ASSERT(x, y)
#endif

template<typename DataType>
class CircularBuffer {
protected:
  DataType* data;
  size_t size;
  size_t writepos = 0;
  size_t readpos = 0;
  bool empty = true;
public:
  CircularBuffer(): data(NULL), size(0){}
  CircularBuffer(DataType* data, size_t size): data(data), size(size){}

  void setData(DataType* data, size_t len) {
    this->data = data;
    size = len;
  }

  size_t getSize() const {
    return size;
  }

  DataType* getData() {
    return data;
  }
  
  bool isEmpty() const {
    return empty;
  }
  
  bool isFull() const {
    return (writepos == readpos) && !empty;
  }

  void write(DataType c){
    FLOW_ASSERT(getWriteCapacity() > 0, "overflow");
    data[writepos++] = c;
    if(writepos >= size)
      writepos = 0;
    empty = false;
  }

  void write(DataType* source, size_t len){
    FLOW_ASSERT(getWriteCapacity() >= len, "overflow");
    DataType* dest = getWriteHead();
    size_t rem = size-writepos;
    if(len >= rem){
      memcpy(dest, source, rem*sizeof(DataType));
      writepos = len-rem;
      memcpy(data, source+rem, writepos*sizeof(DataType));
    }else{
      memcpy(dest, source, len*sizeof(DataType));
      writepos += len;
    }
    empty = false;
  }
    
  void writeAt(size_t index, DataType value){
    data[index % size] = value;
  }

  void overdub(DataType c){
    data[writepos++] += c;
    if(writepos >= size)
      writepos = 0;
    empty = false;
  }

  void overdubAt(size_t index, DataType value){
    data[index % size] += value;
  }

  DataType read(){
    FLOW_ASSERT(getReadCapacity() > 0, "underflow");
    DataType c = data[readpos++];
    if(readpos >= size)
      readpos = 0;
    empty = readpos == writepos;
    return c;
  }

  void read(DataType* dst, size_t len){
    FLOW_ASSERT(getReadCapacity() >= len, "underflow");
    DataType* src = getReadHead();
    size_t rem = size-readpos;
    if(len > rem){
      memcpy(dst, src, rem*sizeof(DataType));
      readpos = len-rem;
      memcpy(dst+rem, data, readpos*sizeof(DataType));
    }else{
      memcpy(dst, src, len*sizeof(DataType));
      readpos += len;
    }
    empty = readpos == writepos;
  }
  
  DataType readAt(size_t index){
    return data[index % size];
  }

  void skipUntilLast(char c){
    DataType* src = getReadHead();
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

  DataType* getWriteHead(){
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

  DataType* getReadHead(){
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
  void delay(DataType* in, DataType* out, size_t len, int delay_samples){
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

  void setAll(const DataType value){
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

  static CircularBuffer<DataType>* create(size_t len){
    CircularBuffer<DataType>* obj = new CircularBuffer<DataType>(new DataType[len], len);
    obj->clear();
    return obj;
  }

  static void destroy(CircularBuffer<DataType>* obj){
    delete[] obj->data;
    delete obj;
  }
};

typedef CircularBuffer<float> CircularFloatBuffer;
typedef CircularBuffer<int16_t> CircularShortBuffer;
typedef CircularBuffer<int32_t> CircularIntBuffer;

#endif /* _CircularBuffer_hpp_ */
