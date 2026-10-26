#include "TestPatch.hpp"
#include "ExponentialDecayEnvelope.h"

/*
 * Unit test for ExponentialDecayEnvelope, documented as an Envelope that, once
 * triggered, decays exponentially from full scale towards zero. Tests assert the
 * documented contract: trigger() starts at 1, gate() sets the level, generate()
 * returns the current level then decays monotonically towards zero, a longer
 * decay time falls slower, setLevel/getLevel round-trip, and the decay time in
 * seconds scales with the sample rate.
 */
class ExponentialDecayEnvelopeTestPatch : public TestPatch {
public:
  ExponentialDecayEnvelopeTestPatch(){
    {
      TEST("trigger full scale");
      ExponentialDecayEnvelope env(48000);
      env.setDecay(0.1f);
      env.trigger();
      CHECK_CLOSE(env.generate(), 1.0f, 0.001); // first sample is full scale
    }
    {
      TEST("gate sets level");
      ExponentialDecayEnvelope env(48000);
      env.gate(true);
      CHECK_CLOSE(env.getLevel(), 1.0f, 0.001);
      env.gate(false);
      CHECK_CLOSE(env.getLevel(), 0.0f, 0.001);
    }
    {
      TEST("monotonic decay");
      ExponentialDecayEnvelope env(48000);
      env.setDecay(0.5f);
      env.trigger();
      float prev = env.generate();
      for(int i=0; i<2048; ++i){
	float s = env.generate();
	CHECK(s <= prev); // never rises while decaying
	prev = s;
      }
    }
    {
      TEST("decays to zero");
      ExponentialDecayEnvelope env(48000);
      env.setDecay(0.05f);
      env.trigger();
      float s = 1.0f;
      for(int i=0; i<48000; ++i) // one second, well past the decay time
	s = env.generate();
      CHECK_CLOSE(s, 0.0f, 0.01);
    }
    {
      TEST("longer decay falls slower");
      ExponentialDecayEnvelope fast(48000), slow(48000);
      fast.setDecay(0.05f);
      slow.setDecay(0.5f);
      fast.trigger();
      slow.trigger();
      float f = 0, s = 0;
      for(int i=0; i<4800; ++i){ f = fast.generate(); s = slow.generate(); }
      CHECK(s > f); // slower envelope retains a higher level
    }
    {
      TEST("set level round trip");
      ExponentialDecayEnvelope env(48000);
      env.setLevel(0.42f);
      CHECK_CLOSE(env.getLevel(), 0.42f, 0.001);
    }
    {
      TEST("decay scales with sample rate");
      // Same decay time in seconds takes ~twice as many samples at double sr.
      ExponentialDecayEnvelope lo(24000), hi(48000);
      lo.setDecay(0.1f);
      hi.setDecay(0.1f);
      lo.trigger();
      hi.trigger();
      int nlo = 0, nhi = 0;
      while(lo.generate() > 0.5f && nlo < 1000000) ++nlo;
      while(hi.generate() > 0.5f && nhi < 1000000) ++nhi;
      CHECK_CLOSE((float)nhi, 2.0f*nlo, 2.0*nlo*0.1f); // within 10%
    }
  }
};
