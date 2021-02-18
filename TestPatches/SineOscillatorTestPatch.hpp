#include "TestPatch.hpp"
#include "SineOscillator.h"

class SineOscillatorTestPatch : public TestPatch {
public:
  SineOscillatorTestPatch(){
    {
      TEST("ctor");
      SineOscillator osc(440, 16000);
      CHECK_EQUAL(osc.getSampleRate(), 16000);
      CHECK_EQUAL((int)osc.getFrequency(), 440);
      osc = SineOscillator(32000);
      CHECK_EQUAL(osc.getFrequency(), 0.0f);
      CHECK_EQUAL(osc.getSampleRate(), 32000);
    }
    {
      TEST("create");
      SineOscillator* osc = SineOscillator::create(120, 32000);
      CHECK_EQUAL(osc->getSampleRate(), 32000);
      CHECK_EQUAL(osc->getFrequency(), 120);
      SineOscillator::destroy(osc);
    }
    {
      TEST("setSampleRate");
      SineOscillator* osc = SineOscillator::create(321, 44100);
      CHECK_EQUAL(osc->getSampleRate(), 44100);
      CHECK_EQUAL(osc->getFrequency(), 321);
      osc->setSampleRate(48000);
      CHECK_EQUAL((int)osc->getSampleRate(), 48000);
      CHECK_EQUAL(osc->getFrequency(), 321); // check that frequency is the same
      SineOscillator::destroy(osc);      
    }
  }
};

