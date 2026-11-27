#include "TestPatch.hpp"
#include "EnvelopeFollower.h"

/*
 * Unit test for EnvelopeFollower, documented as rectifying and smoothing the
 * input to track changes in intensity over time. Tests assert the documented
 * behaviour: the output is a non-negative (rectified) envelope, a constant-
 * amplitude signal settles to that amplitude, a louder input yields a higher
 * envelope, the response is smoothed (not instantaneous), and block processing
 * matches per-sample processing.
 */
class EnvelopeFollowerTestPatch : public TestPatch {
public:
  EnvelopeFollowerTestPatch(){
    {
      TEST("output is rectified non negative");
      EnvelopeFollower ef(0.99);
      // Feed a bipolar signal; the tracked envelope must never go negative.
      for(int i=0; i<4096; ++i){
	float x = (i & 1) ? -0.8f : 0.8f;
	CHECK(ef.process(x) >= 0.0f);
      }
    }
    {
      TEST("tracks constant amplitude");
      // A full-scale square wave of amplitude A has rectified value A, so the
      // smoothed envelope should settle near A.
      EnvelopeFollower ef(0.99);
      float y = 0;
      for(int i=0; i<20000; ++i)
	y = ef.process((i & 1) ? -0.5f : 0.5f);
      CHECK_CLOSE(y, 0.5f, 0.02);
    }
    {
      TEST("louder input higher envelope");
      EnvelopeFollower quiet(0.99), loud(0.99);
      float yq = 0, yl = 0;
      for(int i=0; i<20000; ++i){
	yq = quiet.process((i & 1) ? -0.2f : 0.2f);
	yl = loud.process((i & 1) ? -0.9f : 0.9f);
      }
      CHECK(yl > yq); // tracks intensity
    }
    {
      TEST("response is smoothed");
      // A sudden step should not jump to the full level in one sample; the
      // leaky integrator rises gradually.
      EnvelopeFollower ef(0.99);
      float first = ef.process(1.0f);
      CHECK(first < 0.5f); // far from the eventual level of ~1 after one sample
    }
    {
      TEST("block matches per sample");
      EnvelopeFollower a(0.995), b(0.995);
      const size_t len = 256;
      FloatArray in = FloatArray::create(len);
      FloatArray out = FloatArray::create(len);
      for(size_t i=0; i<len; ++i)
	in[i] = sinf(i*0.3f);
      b.process(in, out);
      for(size_t i=0; i<len; ++i)
	CHECK_CLOSE(a.process(in[i]), out[i], 0.0001);
      FloatArray::destroy(in);
      FloatArray::destroy(out);
    }
  }
};
