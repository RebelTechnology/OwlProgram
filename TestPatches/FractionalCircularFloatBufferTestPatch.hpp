#include "TestPatch.hpp"
#include "FractionalCircularBuffer.h"
#include <limits.h>

class FractionalCircularFloatBufferTestPatch : public TestPatch {
public:
  FractionalCircularFloatBufferTestPatch(){
    {
      TEST("Default ctor");
      FractionalCircularFloatBuffer empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK(empty.isEmpty());
      CHECK(empty.getReadHead() == NULL);
      CHECK(empty.getWriteHead() == NULL);
    }
    {
      TEST("ctor");
      const size_t size = 13;
      float data[size];
      float delta[size];
      FractionalCircularFloatBuffer* buffer = new FractionalCircularFloatBuffer(data, delta, size);
      CHECK_EQUAL(size, buffer->getSize());
      CHECK(data == buffer->getData());
      CHECK(delta == buffer->getDelta());
      delete buffer;
    }
    {
      TEST("create");
      FractionalCircularFloatBuffer* buffer = FractionalCircularFloatBuffer::create(27);
      CHECK_EQUAL((int)buffer->getSize(), 27);
      REQUIRE(buffer->getReadHead() != NULL);
      for(size_t i=0; i<27; ++i)
	CHECK_EQUAL(buffer->readAt(i), 0);
      FractionalCircularFloatBuffer::destroy(buffer);
    }
    {
      TEST("write/read");
      FractionalCircularFloatBuffer* buffer = FractionalCircularFloatBuffer::create(10);
      FloatArray input = FloatArray::create(5);
      input.ramp(0, 5);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->read(), j, DEFAULT_TOLERANCE);
      FractionalCircularFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
    {
      TEST("readAt");
      FractionalCircularFloatBuffer* buffer = FractionalCircularFloatBuffer::create(7);
      FloatArray input = FloatArray::create(6);
      input.ramp(0, 6);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->readAt(j+0.25), j+0.25, DEFAULT_TOLERANCE);
      FractionalCircularFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
    {
      TEST("fractional delay");
      FractionalCircularFloatBuffer* buffer = FractionalCircularFloatBuffer::create(20);
      FloatArray input = FloatArray::create(20);
      FloatArray delay = FloatArray::create(20);
      input.ramp(0, 20);
      buffer->delay(input, delay, input.getSize(), 0.5); // delay by half a sample
      for(int i=1; i<input.getSize(); ++i){
	CHECK_CLOSE(delay[i], (input[i-1] + input[i])/2, DEFAULT_TOLERANCE);
      }
      FractionalCircularFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
      FloatArray::destroy(delay);
    }
    {
      TEST("fractional readAt");
      FractionalCircularFloatBuffer* buffer = FractionalCircularFloatBuffer::create(7);
      FloatArray input = FloatArray::create(7);
      input.ramp(0, 7);
      buffer->write(input, input.getSize());
      for(size_t j=0; j<5; ++j)
	CHECK_CLOSE(buffer->readAt(j+0.5f), j+0.5f, DEFAULT_TOLERANCE);
      FractionalCircularFloatBuffer::destroy(buffer);
      FloatArray::destroy(input);
    }
  }
};

