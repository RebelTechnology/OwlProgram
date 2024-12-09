#include "TestPatch.hpp"
#include "InterpolatingSignalGenerator.h"
#include <limits.h>


class InterpolatingSignalGeneratorTestPatch : public TestPatch {
public:
  typedef InterpolatingSignalGenerator<LINEAR_INTERPOLATION> LerpGen;
  typedef InterpolatingSignalGenerator<LINEAR_INTERPOLATION> TestGen;
  // typedef InterpolatingSignalGenerator<LINEAR_INTERPOLATION> TestGen;
  
  InterpolatingSignalGeneratorTestPatch(){
    {
      TEST("create/destroy");
      TestGen* gen = TestGen::create();
      TestGen::destroy(gen);
    }
    {
      TEST("generateSamples");
      const size_t period = 16;
      LerpGen* gen = LerpGen::create(period);
      gen->update(period);
      for(size_t i=0; i<period; ++i)
	CHECK_CLOSE(gen->generate(), float(i), DEFAULT_TOLERANCE);
      gen->update(period*2);
      for(size_t i=0; i<period; ++i)
	CHECK_CLOSE(gen->generate(), float(i)+period, DEFAULT_TOLERANCE);
      LerpGen::destroy(gen);
    }
    {
      TEST("generateBlock");
      const size_t period = 32;
      FloatArray a = FloatArray::create(period);
      LerpGen* gen = LerpGen::create(period);
      gen->update(1);
      gen->generate(a);
      for(size_t i=0; i<period; ++i)
	CHECK_CLOSE(a[i], float(i)/period, DEFAULT_TOLERANCE);
      LerpGen::destroy(gen);
    }
    {
      TEST("set");
      const size_t period = 48;
      FloatArray a = FloatArray::create(period);
      FloatArray b = FloatArray::create(period);
      TestGen* gen = TestGen::create(period);
      gen->set(0.55);
      gen->generate(a);
      b.setAll(0.55);
      for(size_t i=0; i<period; ++i)
	CHECK_CLOSE(a[i], b[i], DEFAULT_TOLERANCE);      
      FloatArray::destroy(a);
      FloatArray::destroy(b);
      TestGen::destroy(gen);
    }
  }
};

