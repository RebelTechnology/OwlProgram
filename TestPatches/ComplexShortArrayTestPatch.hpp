#include "TestPatch.hpp"
#include "ShortArray.h"
#include "ComplexShortArray.h"

class ComplexShortArrayTestPatch : public TestPatch {
public:
  ComplexShortArrayTestPatch(){
    {
      TEST("sizeof");
      CHECK(sizeof(ComplexShortArray) == sizeof(ComplexShort*)+sizeof(size_t));
    }
    {
      TEST("Default ctor");
      ComplexShortArray empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK((ComplexShort*)empty == NULL);
      ComplexShortArray::destroy(empty);
    }
    {
      TEST("create");
      ComplexShortArray array = ComplexShortArray::create(512);
      CHECK_EQUAL(array.getSize(), 512);
      REQUIRE(array.getData() != NULL);
      ComplexShortArray::destroy(array);
    }
    {
      TEST("getSize");
      size_t size = 123;
      ComplexShortArray array = ComplexShortArray::create(size);
      CHECK_EQUAL(size, array.getSize());
      ComplexShortArray::destroy(array);
    }
    {
      TEST("ComplexByComplexMultiplication");
      ComplexShortArray cfa = ComplexShortArray::create(1000);
      ComplexShortArray tempc = ComplexShortArray::create(1000);
      ComplexShortArray tempc2 = ComplexShortArray::create(1000);
      cfa[0].re = 10000;
      cfa[0].im = 12000;
      cfa[1].re = 14000;
      cfa[1].im = 23000;
      cfa.complexByComplexMultiplication(tempc, tempc2);
      for(size_t n=0; n < cfa.getSize(); ++n){
        CHECK_CLOSE(tempc2[n].re, (cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im) >> 17, 4);
        CHECK_CLOSE(tempc2[n].im, (cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re) >> 17, 4);
      }
      ComplexShortArray::destroy(cfa);
      ComplexShortArray::destroy(tempc);
      ComplexShortArray::destroy(tempc2);
    }
  }
};
