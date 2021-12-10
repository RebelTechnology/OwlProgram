#include "TestPatch.hpp"
#include "CircularBuffer.h"
#include <limits.h>

class CircularFloatBufferTestPatch : public TestPatch {
public:
  CircularFloatBufferTestPatch(){
    {
      TEST("Default ctor");
      CircularFloatBuffer empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK(empty.getReadHead() == NULL);
      CHECK(empty.getWriteHead() == NULL);
    }
    {
      TEST("ctor");
      const size_t size = 123;
      float data[size];
      CircularFloatBuffer* buffer = new CircularFloatBuffer(data, size);
      CHECK_EQUAL(size, buffer->getSize());
      CHECK(data == buffer->getReadHead());
      CHECK(data == buffer->getWriteHead());
      delete buffer;
    }
    {
      TEST("create");
      CircularFloatBuffer* buffer = CircularFloatBuffer::create(512);
      CHECK_EQUAL(buffer->getSize(), 512);
      REQUIRE(buffer->getReadHead() != NULL);
      for(size_t i=0; i<512; ++i)
	CHECK_EQUAL(buffer->readAt(i), 0);
      CircularFloatBuffer::destroy(buffer);
    }
    {
      TEST("size/capacity/isEmpty/isFull");
      const size_t size = 64;
      float data[size];
      CircularFloatBuffer* buffer = CircularFloatBuffer::create(size);
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
      CircularFloatBuffer::destroy(buffer);
    }
    {
      TEST("write/read");
      CircularFloatBuffer* buffer = CircularFloatBuffer::create(10);
      FloatArray input = FloatArray::create(5);
      input.ramp(0, 5);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->read(), j, DEFAULT_TOLERANCE);
      CHECK_EQUAL((int)buffer->getWriteCapacity(), 10);
      CHECK_EQUAL((int)buffer->getReadCapacity(), 0);
      CHECK(buffer->isEmpty());
      CHECK(!buffer->isFull());
      CircularFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
    {
      TEST("readAt");
      CircularFloatBuffer* buffer = CircularFloatBuffer::create(7);
      FloatArray input = FloatArray::create(5);
      input.ramp(0, 5);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->readAt(j), j, DEFAULT_TOLERANCE);
      CircularFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
    {
      TEST("delay");
      CircularFloatBuffer* buffer = CircularFloatBuffer::create(60);
      FloatArray input = FloatArray::create(25);
      FloatArray delay = FloatArray::create(25);
      input.noise();
      CHECK(!input.equals(delay));
      buffer->delay(input, delay, input.getSize(), 10);
      CHECK(input.subArray(0, 15).equals(delay.subArray(10, 15)));
      CircularFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
      FloatArray::destroy(delay);
    }
  }
};

