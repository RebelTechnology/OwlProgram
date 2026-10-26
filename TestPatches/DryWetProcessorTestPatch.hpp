#include "TestPatch.hpp"
#include "DryWetProcessor.h"

/*
 * Unit test for DryWetSignalProcessor, which wraps a SignalProcessor and blends
 * its processed ("wet") output with the unprocessed ("dry") input according to a
 * mix amount. Tests assert the documented crossfade contract:
 *  - mix = 0 returns the dry input unchanged;
 *  - mix = 1 returns the fully wet (processed) output;
 *  - mix = 0.5 returns the average of dry and wet;
 *  - getMix/setMix round-trip.
 * A trivial doubling processor is used so the wet path is easy to predict.
 */

// Processor whose wet output is exactly twice the input, so dry=x and wet=2x.
class DoublingProcessor : public SignalProcessor {
public:
  float process(float x){ return 2*x; }
  void process(FloatArray in, FloatArray out){
    for(size_t i=0; i<out.getSize(); ++i)
      out[i] = 2*in[i];
  }
  static DoublingProcessor* create(){ return new DoublingProcessor(); }
  static void destroy(DoublingProcessor* p){ delete p; }
};

class DryWetProcessorTestPatch : public TestPatch {
public:
  DryWetProcessorTestPatch(){
    const size_t len = 64;
    {
      TEST("mix zero is dry");
      DryWetSignalProcessor<DoublingProcessor>* dw =
	DryWetSignalProcessor<DoublingProcessor>::create(len);
      dw->setMix(0.0f);
      FloatArray in = FloatArray::create(len);
      FloatArray out = FloatArray::create(len);
      for(size_t i=0; i<len; ++i) in[i] = 0.3f;
      dw->process(in, out);
      for(size_t i=0; i<len; ++i)
	CHECK_CLOSE(out[i], 0.3f, 0.0001); // dry input unchanged
      FloatArray::destroy(in);
      FloatArray::destroy(out);
      DryWetSignalProcessor<DoublingProcessor>::destroy(dw);
    }
    {
      TEST("mix one is wet");
      DryWetSignalProcessor<DoublingProcessor>* dw =
	DryWetSignalProcessor<DoublingProcessor>::create(len);
      dw->setMix(1.0f);
      FloatArray in = FloatArray::create(len);
      FloatArray out = FloatArray::create(len);
      for(size_t i=0; i<len; ++i) in[i] = 0.3f;
      dw->process(in, out);
      for(size_t i=0; i<len; ++i)
	CHECK_CLOSE(out[i], 0.6f, 0.0001); // fully processed (2x)
      FloatArray::destroy(in);
      FloatArray::destroy(out);
      DryWetSignalProcessor<DoublingProcessor>::destroy(dw);
    }
    {
      TEST("mix half is average");
      DryWetSignalProcessor<DoublingProcessor>* dw =
	DryWetSignalProcessor<DoublingProcessor>::create(len);
      dw->setMix(0.5f);
      FloatArray in = FloatArray::create(len);
      FloatArray out = FloatArray::create(len);
      for(size_t i=0; i<len; ++i) in[i] = 0.3f;
      dw->process(in, out);
      // 0.5*wet + 0.5*dry = 0.5*0.6 + 0.5*0.3 = 0.45
      for(size_t i=0; i<len; ++i)
	CHECK_CLOSE(out[i], 0.45f, 0.0001);
      FloatArray::destroy(in);
      FloatArray::destroy(out);
      DryWetSignalProcessor<DoublingProcessor>::destroy(dw);
    }
    {
      TEST("mix accessor round trip");
      DryWetSignalProcessor<DoublingProcessor>* dw =
	DryWetSignalProcessor<DoublingProcessor>::create(len);
      dw->setMix(0.37f);
      CHECK_CLOSE(dw->getMix(), 0.37f, 0.0001);
      DryWetSignalProcessor<DoublingProcessor>::destroy(dw);
    }
  }
};
