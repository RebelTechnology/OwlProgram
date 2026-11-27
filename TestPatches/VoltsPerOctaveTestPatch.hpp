#include "TestPatch.hpp"
#include "VoltsPerOctave.h"

/*
 * Unit test for VoltsPerOctave, documented as converting samples into volts and
 * frequencies following 1V/octave tuning, tuned to 440Hz. Tests assert the
 * documented conversions and the 1V/octave contract: 0V is 440Hz, +1V doubles
 * the frequency, note<->hertz follows equal temperament (A4=69=440Hz), tune
 * shifts by octaves, and sample<->frequency round-trips. The two-argument
 * (offset, multiplier) constructor is used to avoid the hardware service call in
 * the default constructor.
 */
class VoltsPerOctaveTestPatch : public TestPatch {
public:
  VoltsPerOctaveTestPatch(){
    {
      TEST("volts to hertz 1V per octave");
      CHECK_CLOSE(VoltsPerOctave::voltsToHertz(0), 440.0f, 0.01); // 0V -> 440Hz
      CHECK_CLOSE(VoltsPerOctave::voltsToHertz(1), 880.0f, 0.01); // +1V -> octave up
      CHECK_CLOSE(VoltsPerOctave::voltsToHertz(-1), 220.0f, 0.01); // -1V -> octave down
      CHECK_CLOSE(VoltsPerOctave::voltsToHertz(2), 1760.0f, 0.1);
    }
    {
      TEST("hertz to volts inverse");
      CHECK_CLOSE(VoltsPerOctave::hertzToVolts(440.0f), 0.0f, 0.001);
      CHECK_CLOSE(VoltsPerOctave::hertzToVolts(880.0f), 1.0f, 0.001);
      // round trip volts -> hertz -> volts
      for(float v=-3; v<=3; v+=0.5f)
	CHECK_CLOSE(VoltsPerOctave::hertzToVolts(VoltsPerOctave::voltsToHertz(v)), v, 0.001);
    }
    {
      TEST("note to hertz equal temperament");
      CHECK_CLOSE(VoltsPerOctave::noteToHertz(69), 440.0f, 0.01); // A4
      CHECK_CLOSE(VoltsPerOctave::noteToHertz(81), 880.0f, 0.01); // A5, one octave up
      CHECK_CLOSE(VoltsPerOctave::noteToHertz(57), 220.0f, 0.01); // A3, one octave down
      // hertzToNote is the inverse of noteToHertz
      for(float n=21; n<=108; n+=12)
	CHECK_CLOSE(VoltsPerOctave::hertzToNote(VoltsPerOctave::noteToHertz(n)), n, 0.01);
    }
    {
      TEST("sample volt conversion with offset and multiplier");
      VoltsPerOctave vco(0.0f, 2.0f); // offset 0, multiplier 2
      CHECK_CLOSE(vco.sampleToVolts(0.5f), 1.0f, 0.001); // (0.5-0)*2
      CHECK_CLOSE(vco.voltsToSample(1.0f), 0.5f, 0.001); // inverse
      // round trip
      for(float s=-1; s<=1; s+=0.25f)
	CHECK_CLOSE(vco.voltsToSample(vco.sampleToVolts(s)), s, 0.001);
    }
    {
      TEST("getFrequency and getSample round trip");
      VoltsPerOctave vco(0.0f, 1.0f);
      vco.setTune(0);
      // A 0V sample is 440Hz; feeding that frequency back gives the same sample.
      float f = vco.getFrequency(0.0f);
      CHECK_CLOSE(f, 440.0f, 0.01);
      CHECK_CLOSE(vco.getSample(f), 0.0f, 0.001);
    }
    {
      TEST("tune shifts by octaves");
      VoltsPerOctave vco(0.0f, 1.0f);
      vco.setTune(1); // one octave up
      CHECK_CLOSE(vco.getFrequency(0.0f), 880.0f, 0.1);
      vco.setMidiNote(69); // A4 -> tune 0
      CHECK_CLOSE(vco.getFrequency(0.0f), 440.0f, 0.1);
      vco.setMidiNote(81); // A5 -> one octave up
      CHECK_CLOSE(vco.getFrequency(0.0f), 880.0f, 0.2);
    }
  }
};
