#include "TestPatch.hpp"
#include "StateVariableFilter.h"

/*
 * Unit test for StateVariableFilter (Andy Simper's trapezoidal SVF). The filter
 * modes are tested by their documented frequency-domain behaviour, driving a
 * sine well below or well above the cutoff and checking the RMS of the output:
 *  - low-pass passes a low tone and attenuates a high tone;
 *  - high-pass does the opposite;
 *  - band-pass passes a tone at the centre frequency and attenuates far tones;
 *  - reset() clears the filter state.
 * Exact coefficient values are implementation detail and are not asserted.
 */
static float rms(FloatArray a){
  float s = 0;
  for(size_t i=0; i<a.getSize(); ++i) s += a[i]*a[i];
  return sqrtf(s/a.getSize());
}

static void fillSine(FloatArray a, float freq, float sr){
  float phase = 0, inc = 2*M_PI*freq/sr;
  for(size_t i=0; i<a.getSize(); ++i){
    a[i] = sinf(phase);
    phase += inc;
  }
}

class StateVariableFilterTestPatch : public TestPatch {
public:
  StateVariableFilterTestPatch(){
    const float sr = 48000;
    const size_t len = 4096;
    FloatArray in = FloatArray::create(len);
    FloatArray out = FloatArray::create(len);
    // discard the first part of the output so the filter has settled
    const size_t skip = 1024;
    auto tailRms = [&](FloatArray o){ return rms(o.subArray(skip, len-skip)); };
    {
      TEST("low pass passes low attenuates high");
      StateVariableFilter* svf = StateVariableFilter::create(sr);
      svf->setLowPass(1000, 0.7071f);
      fillSine(in, 100, sr); // well below cutoff
      svf->process(in, out);
      float low = tailRms(out);
      svf->reset();
      svf->setLowPass(1000, 0.7071f);
      fillSine(in, 12000, sr); // well above cutoff
      svf->process(in, out);
      float high = tailRms(out);
      CHECK(low > high);          // low tone passes more than high tone
      CHECK_CLOSE(low, 0.707f, 0.1); // passband near unity RMS of a sine
      CHECK(high < 0.1f);          // high tone strongly attenuated
      StateVariableFilter::destroy(svf);
    }
    {
      TEST("high pass passes high attenuates low");
      StateVariableFilter* svf = StateVariableFilter::create(sr);
      svf->setHighPass(1000, 0.7071f);
      fillSine(in, 12000, sr);
      svf->process(in, out);
      float high = tailRms(out);
      svf->reset();
      svf->setHighPass(1000, 0.7071f);
      fillSine(in, 100, sr);
      svf->process(in, out);
      float low = tailRms(out);
      CHECK(high > low);
      CHECK(low < 0.1f); // low tone strongly attenuated
      StateVariableFilter::destroy(svf);
    }
    {
      TEST("band pass peaks at centre");
      StateVariableFilter* svf = StateVariableFilter::create(sr);
      const float fc = 1000;
      svf->setBandPass(fc, 2.0f);
      fillSine(in, fc, sr); // at centre frequency
      svf->process(in, out);
      float centre = tailRms(out);
      svf->reset();
      svf->setBandPass(fc, 2.0f);
      fillSine(in, 50, sr); // far below
      svf->process(in, out);
      float below = tailRms(out);
      svf->reset();
      svf->setBandPass(fc, 2.0f);
      fillSine(in, 16000, sr); // far above
      svf->process(in, out);
      float above = tailRms(out);
      CHECK(centre > below); // centre passes more than either side
      CHECK(centre > above);
      StateVariableFilter::destroy(svf);
    }
    {
      TEST("reset clears state");
      StateVariableFilter* svf = StateVariableFilter::create(sr);
      svf->setLowPass(500, 0.7071f);
      for(int i=0; i<10000; ++i) svf->process(1.0f); // drive with DC
      svf->reset();
      // After reset the first response to a zero input is zero (no residual state).
      CHECK_CLOSE(svf->process(0.0f), 0.0f, 0.0001);
      StateVariableFilter::destroy(svf);
    }
    FloatArray::destroy(in);
    FloatArray::destroy(out);
  }
};
