#include "TestPatch.hpp"
#include "../GenSource/genlib.cpp"

/*
 * Unit test for genlib_ops.h operators taken from Max 9: wrap() range,
 * cubic_interp() (4-point Lagrange) and fastcubic_interp(), Delay reads,
 * the fast approximations, and Noise (xoshiro128+).
 */
class GenOpsTestPatch : public TestPatch {
public:
  GenOpsTestPatch(){
    {
      TEST("wrap stays in range");
      CHECK_CLOSE(wrap(1.5f, 0, 1), 0.5f, 1e-6);
      CHECK_CLOSE(wrap(-0.25f, 0, 1), 0.75f, 1e-6);
      CHECK_CLOSE(wrap(7.0f, 2, 5), 4.0f, 1e-5);
      float v = wrap(-1e-8f, 0, 1); // v + 1 rounds to exactly 1 in float
      CHECK(v >= 0.0f && v < 1.0f);
      for(int i=-1000; i<=1000; ++i){
	float x = i * 0.0137f - 1e-7f;
	float w = wrap(x, -1, 1);
	CHECK(w >= -1.0f && w < 1.0f);
      }
    }
    {
      TEST("cubic_interp is the Lagrange cubic");
      // exact for a cubic polynomial through the 4 points at -1, 0, 1, 2
      auto p = [](float t){ return t*t*t - 2*t + 0.5f; };
      for(int i=0; i<=10; ++i){
	float a = i * 0.1f;
	CHECK_CLOSE(cubic_interp(a, p(-1), p(0), p(1), p(2)), p(a), 1e-4);
      }
      CHECK_EQUAL(cubic_interp(0, 1, 2, 3, 4), 2.0f);
      CHECK_CLOSE(cubic_interp(1, 1, 2, 3, 4), 3.0f, 1e-6);
    }
    {
      TEST("fastcubic_interp is the previous cubic");
      CHECK_EQUAL(fastcubic_interp(0, 1, 2, 5, 3), 2.0f);
      CHECK_CLOSE(fastcubic_interp(1, 1, 2, 5, 3), 5.0f, 1e-6);
      // not exact for a cubic, unlike cubic_interp
      auto p = [](float t){ return t*t*t; };
      CHECK(fabsf(fastcubic_interp(0.5f, p(-1), p(0), p(1), p(2)) - p(0.5f)) > 0.01f);
    }
    {
      TEST("Delay reads");
      Delay d;
      d.reset("delaytest", 64);
      for(int i=0; i<64; ++i){
	d.write(i);
	d.step();
      }
      // the delay holds a ramp, which every interpolation except fastcubic reproduces exactly.
      // As in Max 9, the 4-point reads are one sample shorter than linear,
      // and the 6-point spline6 two samples shorter.
      for(float t : {9.0f, 10.0f, 10.25f, 10.5f, 20.75f}){
	float lin = d.read_linear(t);
	CHECK_CLOSE(d.read_cubic(t), lin + 1, 1e-3);
	if(t == floorf(t)) // the old cubic is not exact for a ramp between samples
	  CHECK_CLOSE(d.read_fastcubic(t), lin + 1, 1e-3);
	CHECK_CLOSE(d.read_spline(t), lin + 1, 1e-3);
	CHECK_CLOSE(d.read_spline6(t), lin + 2, 1e-3);
      }
      CHECK_EQUAL(d.read_step(10), d.read_linear(10));
    }
    {
      TEST("fastertanh");
      float maxerr = 0;
      for(int i=-800; i<=800; ++i){
	float x = i * 0.01f;
	float y = fastertanh(x);
	CHECK(y >= -1.0f && y <= 1.0f);
	CHECK_CLOSE(fastertanh(-x), -y, 1e-4); // odd, apart from a 3e-5 offset at 0
	maxerr = fmaxf(maxerr, fabsf(y - tanhf(x)));
      }
      CHECK(maxerr < 0.001f);
      CHECK_CLOSE(fastertanh(-4), -1.0f, 0.001); // Max 9's version returns +22 here
    }
    {
      TEST("fasteratan2");
      for(int i=0; i<36; ++i){
	float t = i * (float)M_PI / 18 - (float)M_PI + 0.05f;
	CHECK_CLOSE(fasteratan2(sinf(t), cosf(t)), t, 0.075);
      }
    }
    {
      TEST("fasterpow2 and fasterlog2");
      CHECK_CLOSE(fasterpow2(3), 8.0f, 0.5);
      CHECK_CLOSE(fasterlog2(8), 3.0f, 0.1);
    }
    {
      TEST("Noise");
      Noise a(0.5), b(0.5), c(0.25);
      bool same = true, differ = false;
      double sum = 0, sum2 = 0;
      const int n = 100000;
      for(int i=0; i<n; ++i){
	float x = a();
	if(x != b()) same = false;
	if(x != c()) differ = true;
	CHECK(x >= -1.0f && x < 1.0f);
	sum += x; sum2 += x*x;
      }
      CHECK(same);   // same seed, same sequence
      CHECK(differ); // different seed, different sequence
      CHECK(fabs(sum / n) < 0.01);              // mean 0
      CHECK(fabs(sum2 / n - 1.0/3.0) < 0.01);   // variance of uniform -1..1
    }
  }
};
