#include "TestPatch.hpp"
#include "MovingAverage.h"
#include <limits.h>


class MovingAverageTestPatch : public TestPatch {
public:
  
  MovingAverageTestPatch(){
    {
      TEST("create/destroy");
      ExponentialMovingAverage* ema = ExponentialMovingAverage::create(0.9);
      SimpleMovingAverage* sma = SimpleMovingAverage::create(16);
      CHECK(ema != NULL);
      CHECK_EQUAL(ema->getLambda(), 0.9);
      CHECK(sma != NULL);
      CHECK_EQUAL(sma->getSize(), 16);
      ExponentialMovingAverage::destroy(ema);
      SimpleMovingAverage::destroy(sma);
    }
    {
      TEST("ema");
      ExponentialMovingAverage* ema = ExponentialMovingAverage::create(0.998);
      const size_t len = 1024;
      FloatArray a = FloatArray::create(len);
      FloatArray b = FloatArray::create(len);
      a.noise(); // generate noise in the range [-1, 1]
      a.add(10); // bias
      ema->set(10);
      ema->process(a, b);
      for(size_t i=32; i<len; ++i)
	CHECK_CLOSE(b[i], 10, 0.1); // ensure average stays within 10% of bias
      ExponentialMovingAverage::destroy(ema);
    }
    {
      TEST("sma");
      SimpleMovingAverage* sma = SimpleMovingAverage::create(16);
      const size_t len = 512;
      FloatArray a = FloatArray::create(len);
      FloatArray b = FloatArray::create(len);
      a.ramp(0, len);
      sma->process(a, b);
      for(size_t i=16; i<len; i+=16)
	CHECK_CLOSE(b[i], i-16+8.5, DEFAULT_TOLERANCE);
      SimpleMovingAverage::destroy(sma);
    }
  }
};

