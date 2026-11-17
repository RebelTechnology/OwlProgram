#include "TestPatch.hpp"
#include "RampOscillator.h"
#include "TriangleOscillator.h"
#include "SquareWaveOscillator.h"

/*
 * Unit tests for the basic (non-antialiased) oscillators. Each oscillator's
 * documented contract is asserted from its description, not its implementation:
 *  - RampOscillator "generates rising output values from -1 to 1";
 *  - InvertedRampOscillator "generates falling output values from 1 to -1";
 *  - TriangleOscillator produces a bipolar [-1, 1] triangle;
 *  - SquareWaveOscillator produces +/-1 with a settable pulse width.
 * Shared Oscillator contracts (range, frequency/phase accessors, reset, and
 * block == per-sample generation) are checked too.
 */
class OscillatorTestPatch : public TestPatch {
public:
  OscillatorTestPatch(){
    {
      TEST("frequency accessor");
      RampOscillator osc;
      osc.setSampleRate(48000);
      osc.setFrequency(440);
      CHECK_CLOSE(osc.getFrequency(), 440, 0.01); // frequency round-trips
      osc.setSampleRate(44100); // changing sr preserves frequency
      CHECK_CLOSE(osc.getFrequency(), 440, 0.1);
    }
    {
      TEST("ramp rises from -1 to 1");
      RampOscillator* osc = RampOscillator::create(48000);
      osc->setFrequency(100);
      osc->reset();
      float first = osc->generate();
      float prev = first;
      bool rising = true;
      for(int i=0; i<400; ++i){ // less than one full period at 48k/100Hz
	float s = osc->generate();
	CHECK(s >= -1.0001f && s <= 1.0001f); // documented output range
	if(s < prev - 1.0f) ; // phase wrap, ignore
	else if(s < prev) rising = false;
	prev = s;
      }
      CHECK(rising);                 // monotonically rising within a period
      CHECK_CLOSE(first, -1.0f, 0.01); // starts at the bottom of the range
      RampOscillator::destroy(osc);
    }
    {
      TEST("inverted ramp falls");
      InvertedRampOscillator osc(48000);
      osc.setFrequency(100);
      osc.reset();
      float first = osc.generate();
      CHECK_CLOSE(first, 1.0f, 0.01); // starts at the top
      float a = osc.generate();
      float b = osc.generate();
      CHECK(b <= a);                  // falling
    }
    {
      TEST("triangle range and symmetry");
      TriangleOscillator* osc = TriangleOscillator::create(48000);
      osc->setFrequency(200);
      osc->reset();
      float peak = -2, trough = 2;
      for(int i=0; i<2000; ++i){
	float s = osc->generate();
	CHECK(s >= -1.0001f && s <= 1.0001f); // bipolar [-1, 1]
	if(s > peak) peak = s;
	if(s < trough) trough = s;
      }
      CHECK_CLOSE(peak, 1.0f, 0.05);   // reaches +1
      CHECK_CLOSE(trough, -1.0f, 0.05); // reaches -1
      TriangleOscillator::destroy(osc);
    }
    {
      TEST("square is bipolar two valued");
      SquareWaveOscillator* osc = SquareWaveOscillator::create(48000);
      osc->setFrequency(500);
      osc->reset();
      int highs = 0, lows = 0;
      for(int i=0; i<2000; ++i){
	float s = osc->generate();
	CHECK(s == 1.0f || s == -1.0f); // output is exactly +/-1
	if(s > 0) ++highs; else ++lows;
      }
      // Default pulse width is 0.5, so roughly equal time high and low.
      CHECK(highs > 0 && lows > 0);
      CHECK_CLOSE((float)highs, (float)lows, 2000*0.1f); // within 10%
      SquareWaveOscillator::destroy(osc);
    }
    {
      TEST("square pulse width");
      SquareWaveOscillator* osc = SquareWaveOscillator::create(48000);
      osc->setFrequency(500);
      osc->setPulseWidth(0.25f); // high for a quarter of the period
      osc->reset();
      int highs = 0;
      const int n = 4000;
      for(int i=0; i<n; ++i)
	if(osc->generate() > 0) ++highs;
      CHECK_CLOSE((float)highs/n, 0.25f, 0.05); // duty cycle tracks pulse width
      SquareWaveOscillator::destroy(osc);
    }
    {
      TEST("reset restores phase");
      TriangleOscillator osc(48000);
      osc.setFrequency(123);
      osc.reset();
      float a0 = osc.generate();
      for(int i=0; i<37; ++i) osc.generate();
      osc.reset();
      float a1 = osc.generate();
      CHECK_CLOSE(a0, a1, 0.0001); // same output after reset
    }
    {
      TEST("block equals per sample");
      RampOscillator o1(48000), o2(48000);
      o1.setFrequency(377);
      o2.setFrequency(377);
      o1.reset();
      o2.reset();
      FloatArray block = FloatArray::create(512);
      o1.generate(block);
      for(size_t i=0; i<512; ++i)
	CHECK_CLOSE(block[i], o2.generate(), 0.0001);
      FloatArray::destroy(block);
    }
  }
};
