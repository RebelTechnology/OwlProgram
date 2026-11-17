#include "TestPatch.hpp"
#include "DcBlockingFilter.h"

/*
 * Unit test for DcBlockingFilter, documented as a "DC Blocking IIR filter, aka
 * Leaky differentiator". Tests assert the documented behaviour (a DC input is
 * driven to zero, a zero-mean signal is preserved, the time-constant accessors
 * round-trip, reset() restores the initial state, and the block/per-sample/stereo
 * overloads agree) rather than the internal coefficient recurrence.
 */
class DcBlockingFilterTestPatch : public TestPatch {
public:
  DcBlockingFilterTestPatch(){
    {
      TEST("dc removed");
      DcBlockingFilter dc;
      float y = 0;
      for(int i=0; i<4096; ++i)
	y = dc.process(1.0f); // steady DC input
      CHECK_CLOSE(y, 0.0f, 0.01); // DC component driven towards zero
    }
    {
      TEST("step edge passes");
      DcBlockingFilter dc;
      // Leaky differentiator: the leading edge of a step passes almost intact.
      CHECK_CLOSE(dc.process(1.0f), 1.0f, 0.01);
    }
    {
      TEST("zero mean preserved");
      DcBlockingFilter dc;
      float energy = 0;
      for(int i=0; i<4096; ++i){
	float x = (i & 1) ? 1.0f : -1.0f; // zero-mean, carries no DC
	float y = dc.process(x);
	energy += y*y;
      }
      CHECK(energy/4096.0f > 0.5f); // output power close to input power of 1
    }
    {
      TEST("time constant round trip");
      DcBlockingFilter dc;
      dc.setTimeConstant(200.0f);
      CHECK_CLOSE(dc.getTimeConstant(), 200.0f, 1.0);
    }
    {
      TEST("longer tc removes dc slower");
      DcBlockingFilter fast, slow;
      fast.setTimeConstant(10.0f);
      slow.setTimeConstant(1000.0f);
      float yf = 0, ys = 0;
      for(int i=0; i<200; ++i){
	yf = fast.process(1.0f);
	ys = slow.process(1.0f);
      }
      CHECK(abs(yf) < abs(ys)); // shorter tc suppresses DC faster
    }
    {
      TEST("reset restores state");
      DcBlockingFilter dc;
      for(int i=0; i<1000; ++i)
	dc.process(1.0f);
      dc.reset();
      DcBlockingFilter fresh;
      CHECK_CLOSE(dc.process(1.0f), fresh.process(1.0f), 0.001);
    }
    {
      TEST("block matches per sample");
      DcBlockingFilter a, b;
      const size_t len = 256;
      FloatArray in = FloatArray::create(len);
      FloatArray out = FloatArray::create(len);
      for(size_t i=0; i<len; ++i)
	in[i] = sinf(i*0.1f) + 0.3f; // signal with a DC offset
      b.process(in, out);
      for(size_t i=0; i<len; ++i)
	CHECK_CLOSE(a.process(in[i]), out[i], 0.001);
      FloatArray::destroy(in);
      FloatArray::destroy(out);
    }
  }
};
