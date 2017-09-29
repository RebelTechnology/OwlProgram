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
      float threshold = 0.02; // maximum relative error accepted
      int errs = 0;
      int tests = 0;
      for(int n = 10; n <= 10000; n++){
        float x = rand()/(float)RAND_MAX * n;
        float approx = fast_logf(x);
        float exact = logf(x);
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

