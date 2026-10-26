#include "TestPatch.hpp"
#include "WavetableOscillator.h"
#include "RampOscillator.h"

/*
 * Unit test for WavetableOscillator and the shared oscillator FM phase wrap.
 * Covers two fixes:
 *  - WavetableOscillator::getSample() clamps the table index so phase at or
 *    past 1.0, and negative phase from negative FM, never read out of bounds;
 *  - OscillatorTemplate::generate(fm) wraps phase at BOTH ends, so negative FM
 *    keeps the oscillator's output within its documented range.
 * Also checks that the wavetable plays back the table it was given.
 */
class WavetableOscillatorTestPatch : public TestPatch {
public:
  WavetableOscillatorTestPatch(){
    const float sr = 48000;
    {
      TEST("plays back the wavetable");
      // A ramp wavetable: reading at phase p returns ~p.
      const size_t len = 256;
      FloatArray table = FloatArray::create(len);
      for(size_t i=0; i<len; ++i)
	table[i] = (float)i/len;
      WavetableOscillator osc(sr, table);
      osc.setFrequency(100);
      osc.setPhase(0);
      CHECK_CLOSE(osc.generate(), 0.0f, 0.01); // phase 0 -> table[0]
      osc.setPhase(M_PI); // half way (end_phase=1 maps 2pi)
      CHECK_CLOSE(osc.generate(), 0.5f, 0.02);
      FloatArray::destroy(table);
    }
    {
      TEST("index clamped at phase one");
      // setPhase(2*pi) maps to phase == end_phase == 1.0; index would be size
      // without the clamp. Must not read out of bounds and must return the last
      // table entry.
      const size_t len = 64;
      FloatArray table = FloatArray::create(len);
      for(size_t i=0; i<len; ++i)
	table[i] = (float)i; // distinct values 0..63
      WavetableOscillator osc(sr, table);
      osc.setFrequency(0); // no advance
      osc.setPhase(2*M_PI); // phase -> 1.0
      float s = osc.generate();
      CHECK(s == (float)(len-1)); // clamped to last element, no OOB
      FloatArray::destroy(table);
    }
    {
      TEST("negative fm stays in range");
      // Drive a ramp oscillator with negative FM; the phase-wrap fix must keep
      // every output within the documented [-1, 1] range. FM depth is kept
      // within one period per sample, matching the single-period wrap used for
      // the upper bound too.
      RampOscillator* osc = RampOscillator::create(sr);
      osc->setFrequency(1000);
      osc->reset();
      for(int i=0; i<8000; ++i){
	float s = osc->generate(-0.4f); // negative FM within one period/sample
	CHECK(s >= -1.0001f && s <= 1.0001f);
      }
      RampOscillator::destroy(osc);
    }
    {
      TEST("wavetable negative fm no out of bounds");
      // With negative FM the wavetable index must remain valid (the test fails
      // via garbage output if the clamp or phase wrap is missing).
      const size_t len = 128;
      FloatArray table = FloatArray::create(len);
      for(size_t i=0; i<len; ++i)
	table[i] = sinf(2*M_PI*i/len);
      WavetableOscillator osc(sr, table);
      osc.setFrequency(500);
      osc.reset();
      for(int i=0; i<8000; ++i){
	float s = osc.generate(-0.4f);
	CHECK(s >= -1.0001f && s <= 1.0001f); // table holds a unit sine
      }
      FloatArray::destroy(table);
    }
  }
};
