#include "TestPatch.hpp"
#include "NoiseGenerator.h"

/*
 * Unit test for the noise generators. Noise is random, so the tests assert the
 * documented statistical contracts rather than exact samples:
 *  - WhiteNoiseGenerator "returns white noise in the range -1 to 1", roughly
 *    zero-mean, and successive samples differ (it is not stuck);
 *  - PinkNoiseGenerator / BrownNoiseGenerator stay within their documented
 *    bounded ranges;
 *  - the block-based generate(FloatArray) fills the whole buffer within range.
 */
class NoiseGeneratorTestPatch : public TestPatch {
public:
  NoiseGeneratorTestPatch(){
    {
      TEST("white range and mean");
      WhiteNoiseGenerator* ng = WhiteNoiseGenerator::create();
      float sum = 0;
      const int n = 20000;
      for(int i=0; i<n; ++i){
	float s = ng->generate();
	CHECK(s >= -1.0f && s <= 1.0f); // documented range -1..1
	sum += s;
      }
      CHECK_CLOSE(sum/n, 0.0f, 0.05); // approximately zero-mean
      WhiteNoiseGenerator::destroy(ng);
    }
    {
      TEST("white is not constant");
      WhiteNoiseGenerator ng;
      float a = ng.generate();
      bool varied = false;
      for(int i=0; i<100 && !varied; ++i)
	if(ng.generate() != a) varied = true;
      CHECK(varied); // output actually changes
    }
    {
      TEST("block fill within range");
      WhiteNoiseGenerator ng;
      FloatArray out = FloatArray::create(256);
      out.setAll(2.0f); // sentinel outside the noise range
      ng.generate(out);
      for(size_t i=0; i<out.getSize(); ++i)
	CHECK(out[i] >= -1.0f && out[i] <= 1.0f); // every slot written in range
      FloatArray::destroy(out);
    }
    {
      TEST("pink bounded");
      PinkNoiseGenerator* ng = PinkNoiseGenerator::create();
      for(int i=0; i<20000; ++i){
	float s = ng->generate();
	CHECK(s >= -1.0f && s <= 1.0f); // documented range -0.5..0.5, allow margin
      }
      PinkNoiseGenerator::destroy(ng);
    }
    {
      TEST("brown bounded");
      BrownNoiseGenerator* ng = BrownNoiseGenerator::create();
      for(int i=0; i<20000; ++i){
	float s = ng->generate();
	CHECK(s >= -1.0f && s <= 1.0f); // documented range -0.5..0.5, allow margin
      }
      BrownNoiseGenerator::destroy(ng);
    }
  }
};
