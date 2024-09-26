#include "TestPatch.hpp"
#include "SmoothValue.h"
#include <limits.h>

class SmoothValueTestPatch : public TestPatch {
public:
  
  SmoothValueTestPatch(){
    {
      TEST("float");
      SmoothFloat a = 0.5;
      CHECK_CLOSE(a, 0.5, DEFAULT_TOLERANCE); // initialsed value
      CHECK_CLOSE(a.lambda, 0.9, DEFAULT_TOLERANCE); // default lambda
      a.lambda = 0.6;
      a = 0;
      CHECK_CLOSE(a, 0.5*0.6, DEFAULT_TOLERANCE); // correct smoothing
      float prev = a;
      for(int i=0; i<32; ++i){
	a = -1;
	CHECK(a < prev); // monotonically descending
	prev = a;
      }
      CHECK_CLOSE(a, -1, DEFAULT_TOLERANCE); // arrives near target
    }
    {
// typedef SmoothValue<int> SmoothInt;
      TEST("int");
      SmoothValue<float, SimpleValue<int>> a = 5;
      CHECK_CLOSE(a, 5, DEFAULT_TOLERANCE);
      CHECK_CLOSE(a.lambda, 0.9, DEFAULT_TOLERANCE); // default lambda
      a = 0;
      a.lambda = 0.6;
      CHECK_CLOSE(a, 5*0.9, DEFAULT_TOLERANCE);
      for(int i=0; i<32; ++i)
	a = -10;
      CHECK_CLOSE(a, -10, DEFAULT_TOLERANCE);
    }
    {
      TEST("weirdint");
      SmoothInt a = 5;
      CHECK_CLOSE(a, 5, DEFAULT_TOLERANCE);
      CHECK_CLOSE(a.lambda, 0.9, DEFAULT_TOLERANCE); // default lambda
      a = 0;
    }
  }
};

