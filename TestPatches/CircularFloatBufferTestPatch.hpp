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
      CHECK_EQUAL((int)buffer->getSize(), 512);
      REQUIRE(buffer->getReadHead() != NULL);
      for(size_t i=0; i<512; ++i)
	CHECK_EQUAL(buffer->readAt(i), 0);
      CircularFloatBuffer::destroy(buffer);
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

