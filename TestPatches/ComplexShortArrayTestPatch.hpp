#include "TestPatch.hpp"
#include "ShortArray.h"
#include "ComplexShortArray.h"

class ComplexShortArrayTestPatch : public TestPatch {
public:
  ComplexShortArrayTestPatch(){
    {
      TEST("Default ctor");
      ComplexShortArray empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK_EQUAL((short*)empty, NULL);
    }
    {
      TEST("create");
      ComplexShortArray array = ComplexShortArray::create(512);
      CHECK_EQUAL(array.getSize(), 512);
      REQUIRE(array.getData() != NULL);
    }
    {
      TEST("getSize");
      int size = 123;
      ComplexShortArray array = ComplexShortArray::create(size);
      CHECK_EQUAL(size, array.getSize());
    }
    {
      TEST("ComplexByComplexMultiplication");
      ComplexShortArray cfa = ComplexShortArray::create(1000);
      ComplexShortArray tempc = ComplexShortArray::create(1000);
      ComplexShortArray tempc2 = ComplexShortArray::create(1000);
      cfa.complexByComplexMultiplication(tempc, tempc2);
      int a = tempc2[(const unsigned int)4].re;
      for(int n=0; n < cfa.getSize(); ++n){
        CHECK_CLOSE(tempc2[n].re, (int32_t)cfa[n].re*tempc[n].re - (int32_t)cfa[n].im*tempc[n].im, 2);
        CHECK_CLOSE(tempc2[n].im, (int32_t)cfa[n].re*tempc[n].im + (int32_t)cfa[n].im*tempc[n].re, 2);
      }
      CHECK();
    }
  }
};
