#include "TestPatch.hpp"

#ifdef logf
#undef logf
#endif

class FastLogTestPatch : public TestPatch {
public:
  FastLogTestPatch(){
    {
      TEST("FastLog");
      float maxPerc = 0;
#ifdef ARM_CORTEX
      float threshold = 0.025;
#else
      float threshold = 0.8283; // maximum relative error accepted: 0.025 for big tables, 0.8283 for small
#endif      
      int errs = 0;
      int tests = 0;
      for(int n = 10; n <= 10000; n++){
        float x = rand()/(float)RAND_MAX * n;
	// float x = randf() * n;
        float approx = fast_logf(x);
        float exact = logf(x);
        float err = fabsf(approx - exact);
        float perc = err/exact * 100;
	CHECK(perc<threshold);
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

