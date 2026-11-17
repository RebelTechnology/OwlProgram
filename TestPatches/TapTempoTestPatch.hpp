#include "TestPatch.hpp"
#include "TapTempo.h"

/*
 * Unit test for TapTempo. The key assertion ("single sample clock measures
 * period") is the regression test for the clock() fix: per-sample clock() must
 * be able to count up into the valid (min_limit, max_limit) window so that a
 * subsequent trigger() records the measured period. Other tests cover the
 * documented BPM/frequency/period conversions.
 */
class TapTempoTestPatch : public TestPatch {
public:
  TapTempoTestPatch(){
    const float sr = 48000;
    {
      TEST("single sample clock measures period");
      // Tap, count N samples one clock() at a time, tap again. The measured
      // period should be N samples. This only works if clock() can count past
      // min_limit up to max_limit.
      TapTempo tempo(sr, 16, 1<<17);
      const size_t period = 2400; // 20 Hz at 48k, well inside (16, 131072)
      tempo.trigger(true);  // first tap: starts counting
      tempo.trigger(false);
      for(size_t i=0; i<period; ++i)
	tempo.clock();
      tempo.trigger(true);  // second tap: records the counted period
      CHECK_CLOSE((float)tempo.getPeriodInSamples(), (float)period, 2);
    }
    {
      TEST("multi step clock clamps at max");
      TapTempo tempo(sr, 16, 1000);
      tempo.trigger(true);
      tempo.trigger(false);
      tempo.clock(5000); // overshoot max_limit
      tempo.trigger(true);
      // trig was clamped to max_limit (1000), which is NOT < max_limit, so the
      // period is not updated and stays at its init value (max/2 = 500).
      CHECK_EQUAL(tempo.getPeriodInSamples(), 500);
    }
    {
      TEST("period and frequency accessors");
      TapTempo tempo(sr, 1<<17);
      tempo.setPeriodInSamples(4800); // 10 Hz
      CHECK_CLOSE(tempo.getFrequency(), 10.0f, 0.001);
      CHECK_CLOSE(tempo.getBeatsPerMinute(), 600.0f, 0.01); // 10 Hz * 60
      tempo.setFrequency(5.0f);
      CHECK_CLOSE((float)tempo.getPeriodInSamples(), 9600.0f, 1);
      tempo.setBeatsPerMinute(120.0f);
      CHECK_CLOSE(tempo.getFrequency(), 2.0f, 0.001); // 120 BPM = 2 Hz
    }
    {
      TEST("static conversions");
      CHECK_CLOSE(TapTempo::bpmToFrequency(120), 2.0f, 0.0001);
      CHECK_CLOSE(TapTempo::frequencyToBpm(2), 120.0f, 0.0001);
      CHECK_CLOSE(TapTempo::bpmToSamplePeriod(120, sr), sr/2, 0.01);
      CHECK_CLOSE(TapTempo::samplePeriodToBpm(sr/2, sr), 120.0f, 0.01);
    }
  }
};
