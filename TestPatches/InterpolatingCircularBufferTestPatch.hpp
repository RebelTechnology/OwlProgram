#include "TestPatch.hpp"
#include "InterpolatingCircularBuffer.h"
#include <limits.h>


class InterpolatingCircularBufferTestPatch : public TestPatch {
public:
  typedef InterpolatingCircularFloatBuffer<LINEAR_INTERPOLATION> LinearFloatBuffer;
  // typedef InterpolatingCircularFloatBuffer<COSINE_INTERPOLATION> CosineFloatBuffer;
  // typedef InterpolatingCircularFloatBuffer<HERMITE_INTERPOLATION> HermiteFloatBuffer;
  
  InterpolatingCircularBufferTestPatch(){
    {
      TEST("Default ctor");
      LinearFloatBuffer empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK(empty.isEmpty());
      CHECK(!empty.isFull());
      CHECK(empty.getReadHead() == NULL);
      CHECK(empty.getWriteHead() == NULL);
    }
    {
      TEST("ctor");
      const size_t size = 123;
      float data[size];
      LinearFloatBuffer* buffer = new LinearFloatBuffer(data, size);
      CHECK_EQUAL(size, buffer->getSize());
      CHECK(data == buffer->getReadHead());
      CHECK(data == buffer->getWriteHead());
      delete buffer;
    }
    {
      TEST("create");
      LinearFloatBuffer* buffer = LinearFloatBuffer::create(512);
      CHECK_EQUAL(buffer->getSize(), 512);
      REQUIRE(buffer->getReadHead() != NULL);
      for(size_t i=0; i<512; ++i)
	CHECK_EQUAL(buffer->readAt(i), 0);
      LinearFloatBuffer::destroy(buffer);
    }
    {
      TEST("size/capacity/isEmpty/isFull");
      const size_t size = 64;
      float data[size];
      LinearFloatBuffer* buffer = LinearFloatBuffer::create(size);
      CHECK_EQUAL((int)buffer->getSize(), size);
      CHECK_EQUAL((int)buffer->getWriteCapacity(), size);
      CHECK_EQUAL((int)buffer->getReadCapacity(), 0);
      CHECK(buffer->isEmpty());
      CHECK(!buffer->isFull());
      buffer->write(data, size/2);
      CHECK_EQUAL((int)buffer->getWriteCapacity(), size/2);
      CHECK_EQUAL((int)buffer->getReadCapacity(), size/2);
      CHECK(!buffer->isEmpty());
      CHECK(!buffer->isFull());
      buffer->write(data, size/2);
      CHECK_EQUAL((int)buffer->getWriteCapacity(), 0);
      CHECK_EQUAL((int)buffer->getReadCapacity(), size);
      CHECK(!buffer->isEmpty());
      CHECK(buffer->isFull());
      // buffer->read(data, size);
      for(size_t j=0; j<size; ++j)
	buffer->read();
      CHECK_EQUAL((int)buffer->getWriteCapacity(), size);
      CHECK_EQUAL((int)buffer->getReadCapacity(), 0);
      CHECK(buffer->isEmpty());
      CHECK(!buffer->isFull());
      LinearFloatBuffer::destroy(buffer);
    }
    {
      TEST("write/read");
      LinearFloatBuffer* buffer = LinearFloatBuffer::create(10);
      FloatArray input = FloatArray::create(5);
      input.ramp(0, 5);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->read(), j, DEFAULT_TOLERANCE);
      CHECK_EQUAL((int)buffer->getWriteCapacity(), 10);
      CHECK_EQUAL((int)buffer->getReadCapacity(), 0);
      CHECK(buffer->isEmpty());
      CHECK(!buffer->isFull());
      LinearFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
    {
      TEST("readAt");
      LinearFloatBuffer* buffer = LinearFloatBuffer::create(7);
      FloatArray input = FloatArray::create(5);
      input.ramp(0, 5);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->readAt(j), j, DEFAULT_TOLERANCE);
      LinearFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
    {
      TEST("delay");
      LinearFloatBuffer* buffer = LinearFloatBuffer::create(60);
      FloatArray input = FloatArray::create(25);
      FloatArray delay = FloatArray::create(25);
      input.noise();
      CHECK(!input.equals(delay));
      buffer->delay(input, delay, input.getSize(), 10);
      CHECK(input.subArray(0, 15).equals(delay.subArray(10, 15)));
      LinearFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
      FloatArray::destroy(delay);
    }
    {
      TEST("interpolated delay");
      LinearFloatBuffer* buffer = LinearFloatBuffer::create(20);
      FloatArray input = FloatArray::create(20);
      FloatArray delay = FloatArray::create(20);
      input.ramp(0, 20);
      buffer->delay(input, delay, input.getSize(), 0.5); // delay by half a sample
      for(int i=1; i<input.getSize(); ++i){
	CHECK_CLOSE(delay[i], (input[i-1] + input[i])/2, DEFAULT_TOLERANCE);
      }
      LinearFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
      FloatArray::destroy(delay);
    }
    {
      TEST("interpolated readAt");
      LinearFloatBuffer* buffer = LinearFloatBuffer::create(7);
      FloatArray input = FloatArray::create(7);
      input.ramp(0, 7);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->readAt(j+0.5f), j+0.5f, DEFAULT_TOLERANCE);
      LinearFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
  }
};

