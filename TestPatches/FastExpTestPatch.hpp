#include "TestPatch.hpp"

#ifdef expf
#undef expf
#endif
#include <math.h>

class FastExpTestPatch : public TestPatch {
public:
  FastExpTestPatch(){
    {
      TEST("FastExp");
      float maxPerc = 0;
#ifdef ARM_CORTEX
      float threshold = 0.02;
#else
      float threshold = 0.544; // maximum relative error accepted: 0.02 for big tables, 0.544 for small
#endif      
      int errs = 0;
      int tests = 0;
      for(int n = -90; n < 90; n++){
        float x = randf() * n;
        float approx = fast_expf(x);
        float exact = expf(x);
        float err = fabsf(approx - exact);
        float perc = (exact == 0.00 || std::isfinite(exact)) == 0 ? err*100 : (err/exact)*100;
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

