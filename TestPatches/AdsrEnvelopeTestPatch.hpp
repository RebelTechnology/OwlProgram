#include "TestPatch.hpp"
#include "AdsrEnvelope.h"

/*
 * Unit test for AdsrEnvelope (linear and exponential specialisations). Tests
 * assert the documented ADSR contract rather than the per-stage increment math:
 *  - with the gate held on, the envelope rises through attack then settles at
 *    the sustain level;
 *  - releasing the gate decays the level back towards zero;
 *  - a higher sustain setting holds a higher level;
 *  - setLevel/getLevel round-trip.
 * Both LinearAdsrEnvelope and ExponentialAdsrEnvelope are exercised.
 */
class AdsrEnvelopeTestPatch : public TestPatch {
public:
  AdsrEnvelopeTestPatch(){
    const float sr = 48000;
    {
      TEST("linear reaches sustain under gate");
      LinearAdsrEnvelope env(sr);
      env.setAttack(0.005f);
      env.setDecay(0.005f);
      env.setSustain(0.6f);
      env.setRelease(0.01f);
      env.gate(true);
      float level = 0;
      for(int i=0; i<sr*0.1f; ++i) // 100ms, past attack+decay
	level = env.generate();
      CHECK_CLOSE(level, 0.6f, 0.02); // settles at sustain
    }
    {
      TEST("attack rises from zero");
      LinearAdsrEnvelope env(sr);
      env.setAttack(0.05f);
      env.setDecay(0.01f);
      env.setSustain(1.0f);
      env.gate(true);
      float first = env.generate();
      float later = 0;
      for(int i=0; i<100; ++i) later = env.generate();
      CHECK(later > first); // level is rising during attack
    }
    {
      TEST("release decays to zero");
      LinearAdsrEnvelope env(sr);
      env.setAttack(0.001f);
      env.setDecay(0.001f);
      env.setSustain(0.8f);
      env.setRelease(0.01f);
      env.gate(true);
      for(int i=0; i<sr*0.05f; ++i) env.generate(); // reach sustain
      env.gate(false);
      float level = 1;
      for(int i=0; i<sr*0.2f; ++i) // 200ms release, past release time
	level = env.generate();
      CHECK_CLOSE(level, 0.0f, 0.001); // linear envelope returns to 0
    }
    {
      TEST("higher sustain holds higher level");
      LinearAdsrEnvelope lo(sr), hi(sr);
      lo.setAttack(0.002f); lo.setDecay(0.002f); lo.setSustain(0.3f);
      hi.setAttack(0.002f); hi.setDecay(0.002f); hi.setSustain(0.9f);
      lo.gate(true); hi.gate(true);
      float l = 0, h = 0;
      for(int i=0; i<sr*0.05f; ++i){ l = lo.generate(); h = hi.generate(); }
      CHECK(h > l);
      CHECK_CLOSE(l, 0.3f, 0.02);
      CHECK_CLOSE(h, 0.9f, 0.02);
    }
    {
      TEST("level accessor round trip");
      LinearAdsrEnvelope env(sr);
      env.setLevel(0.42f);
      CHECK_CLOSE(env.getLevel(), 0.42f, 0.0001);
    }
    {
      TEST("exponential reaches sustain under gate");
      ExponentialAdsrEnvelope env(sr);
      env.setAttack(0.005f);
      env.setDecay(0.01f);
      env.setSustain(0.5f);
      env.setRelease(0.01f);
      env.gate(true);
      float level = 0;
      for(int i=0; i<sr*0.2f; ++i)
	level = env.generate();
      CHECK_CLOSE(level, 0.5f, 0.05); // settles near sustain
    }
  }
};
