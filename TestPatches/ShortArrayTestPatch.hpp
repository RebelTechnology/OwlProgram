#include "TestPatch.hpp"
#include "ShortArray.h"

class ShortArrayTestPatch : public TestPatch {
public:
  ShortArrayTestPatch(){
    {
      TEST("Default ctor");
      ShortArray empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK_EQUAL((short*)empty, NULL);
    }
    {
      TEST("create");
      ShortArray array = ShortArray::create(512);
      CHECK_EQUAL(array.getSize(), 512);
      REQUIRE(array.getData() != NULL);
      for(int i=0; i<512; ++i)
	CHECK_CLOSE(array[i], 0.0, DEFAULT_TOLERANCE);
    }
  }
};
