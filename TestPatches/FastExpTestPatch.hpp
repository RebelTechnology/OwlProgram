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
      float threshold = 0.02; // maximum relative error accepted
      int errs = 0;
      int tests = 0;
      for(int n = -90; n < 90; n++){
        float x = rand()/(float)RAND_MAX * n;
        float approx = fast_expf(x);
        float exact = expf(x);
        float err = fabsf(approx - exact);
        float perc = (exact == 0.00 || std::isfinite(exact)) == 0 ? err*100 : err/exact*100;
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

