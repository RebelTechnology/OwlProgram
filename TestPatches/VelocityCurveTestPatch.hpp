#include "TestPatch.hpp"
#include "VelocityCurve.h"

/*
 * Unit test for VelocityCurve. The class documents the General MIDI DLS Level 1
 * concave transform, where gain = velocity^2 / 127^2. Tests assert the documented
 * behaviour: full velocity maps to unity gain, the curve is monotonically rising,
 * it follows the square law with the default range, and a wider dynamic range
 * attenuates low velocities more. The exact b/m coefficient formulae are
 * implementation detail and are not asserted.
 */
class VelocityCurveTestPatch : public TestPatch {
public:
  VelocityCurveTestPatch(){
    {
      TEST("full velocity is unity gain");
      VelocityCurve vc;
      CHECK_CLOSE(vc.velocityToGain(127), 1.0f, 0.001); // v=127 -> gain 1
    }
    {
      TEST("monotonic rising");
      VelocityCurve vc;
      float prev = vc.velocityToGain(0);
      for(int v=1; v<=127; ++v){
	float g = vc.velocityToGain(v);
	CHECK(g >= prev); // higher velocity never gives lower gain
	prev = g;
      }
    }
    {
      TEST("follows square law");
      // Documented: gain = velocity^2 / 127^2 for the default curve.
      VelocityCurve vc;
      for(int v=10; v<=127; v+=20){
	float expected = (float)v*v/(127.0f*127.0f);
	CHECK_CLOSE(vc.velocityToGain(v), expected, 0.02);
      }
    }
    {
      TEST("gain within unit range");
      VelocityCurve vc;
      for(int v=0; v<=127; ++v){
	float g = vc.velocityToGain(v);
	CHECK(g >= 0.0f && g <= 1.0001f); // gain is a normalised attenuation
      }
    }
    {
      TEST("wider range attenuates low velocities more");
      VelocityCurve narrow, wide;
      narrow.setDynamicRange(36);
      wide.setDynamicRange(96);
      // At a low velocity the wider dynamic range yields a smaller (more
      // attenuated) gain than the narrower range.
      CHECK(wide.velocityToGain(20) < narrow.velocityToGain(20));
      // Both still reach unity at full velocity.
      CHECK_CLOSE(narrow.velocityToGain(127), 1.0f, 0.01);
      CHECK_CLOSE(wide.velocityToGain(127), 1.0f, 0.01);
    }
  }
};
