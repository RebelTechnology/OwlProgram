#include "TestPatch.hpp"

#ifdef powf
#undef powf
#endif

class FastPowTestPatch : public TestPatch {
public:
  FastPowTestPatch(){
    {
      TEST("FastPow");
      float maxPerc = 0;
#ifdef ARM_CORTEX
      float threshold = 0.04;
#else
      float threshold = 0.544; // maximum relative error accepted: 0.04 for big tables, 0.544 for small
#endif      
      int errs = 0;
      int tests = 0;
      for(int n = -1000; n < 1000; n++){
        float base = randf() * 10;
        float exponent = n*10/1000.f;
        float approx = fast_powf(base, exponent);
        float exact = powf(base, exponent);
        float err = fabsf(approx - exact);
        float perc = err/exact * 100;
	CHECK(perc<threshold);
	// CHECK_CLOSE(approx, exact, threshold);
        maxPerc = maxPerc > perc ? maxPerc : perc;
        if(fabsf(perc) > threshold)
	  errs++;
	tests++;
      }
      debugMessage("threshold / errors %:", threshold, 100.0f*errs/tests);
      debugMessage("max error %:", maxPerc);
    }
  }
};

