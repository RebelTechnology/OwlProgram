#include "TestPatch.hpp"
#include "SineOscillator.h"

class SineOscillatorTestPatch : public TestPatch {
public:
  SineOscillatorTestPatch(){
    {
      TEST("ctor");
      SineOscillator osc(440, 16000);
      CHECK_EQUAL(osc.getSampleRate(), 16000);
      CHECK_EQUAL(osc.getFrequency(), 440);
      osc = SineOscillator(32000);
      CHECK_EQUAL(osc.getFrequency(), 0.0f);
    }
    {
      TEST("create");
      SineOscillator* osc = SineOscillator::create(120.0f, 32000);
      CHECK_EQUAL(osc->getFrequency(), 120.0f);
      SineOscillator::destroy(osc);
    }
    {
      TEST("setSampleRate");
      SineOscillator* osc = SineOscillator::create(321.0f, 44100);
      
    }
  }
};

