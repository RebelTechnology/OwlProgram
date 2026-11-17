#include "TestPatch.hpp"
#include "SmoothingFilter.h"

/*
 * Unit test for SmoothingFilter, documented as an "AC Blocking IIR filter, the
 * Leaky Integrator. Used for signal smoothing." Tests assert the documented
 * behaviour: a constant input is converged to (not AC-blocked away, unlike the
 * DC blocker), the approach is gradual (smoothed), a larger lambda smooths more
 * slowly, and the various process() overloads agree.
 */
class SmoothingFilterTestPatch : public TestPatch {
public:
  SmoothingFilterTestPatch(){
    {
      TEST("converges to constant");
      SmoothingFilter sf(0.99);
      float y = 0;
      for(int i=0; i<20000; ++i)
	y = sf.process(0.75f);
      CHECK_CLOSE(y, 0.75f, 0.001); // settles at the input level
    }
    {
      TEST("gradual approach");
      // The first output after a step from 0 is a small fraction of the step,
      // i.e. the filter smooths rather than jumping.
      SmoothingFilter sf(0.9);
      float first = sf.process(1.0f);
      CHECK_CLOSE(first, 0.1f, 0.001); // (1-lambda)*x on the first sample
      CHECK(first < 1.0f);
    }
    {
      TEST("larger lambda smooths slower");
      SmoothingFilter fast(0.5), slow(0.99);
      float yf = 0, ys = 0;
      for(int i=0; i<50; ++i){
	yf = fast.process(1.0f);
	ys = slow.process(1.0f);
      }
      // After the same number of samples the slower filter is further from the
      // target, i.e. lower.
      CHECK(ys < yf);
    }
    {
      TEST("block matches per sample");
      SmoothingFilter a(0.95), b(0.95);
      const size_t len = 256;
      FloatArray in = FloatArray::create(len);
      FloatArray out = FloatArray::create(len);
      for(size_t i=0; i<len; ++i)
	in[i] = sinf(i*0.2f) + 0.3f;
      b.process(in, out);
      for(size_t i=0; i<len; ++i)
	CHECK_CLOSE(a.process(in[i]), out[i], 0.0001);
      FloatArray::destroy(in);
      FloatArray::destroy(out);
    }
    {
      TEST("in place processing");
      SmoothingFilter a(0.9), b(0.9);
      const size_t len = 128;
      FloatArray buf = FloatArray::create(len);
      FloatArray ref = FloatArray::create(len);
      for(size_t i=0; i<len; ++i)
	buf[i] = ref[i] = (i < len/2) ? 1.0f : -1.0f;
      b.process(buf); // in-place
      for(size_t i=0; i<len; ++i)
	CHECK_CLOSE(buf[i], a.process(ref[i]), 0.0001);
      FloatArray::destroy(buf);
      FloatArray::destroy(ref);
    }
  }
};
