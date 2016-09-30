#include "TestPatch.hpp"
#include "ShortArray.h"
#include "ComplexShortArray.h"

class ComplexShortArrayTestPatch : public TestPatch {
public:
  ComplexShortArrayTestPatch(){
    debugMessage("");
    {
      TEST("Default ctor");
      ComplexShortArray empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK_EQUAL((ComplexShort*)empty, NULL);
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
      int size = 123;
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
      for(int n=0; n < cfa.getSize(); ++n){
        CHECK_CLOSE(tempc2[n].re, (cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im) >> 17, 4);
        CHECK_CLOSE(tempc2[n].im, (cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re) >> 17, 4);
      }
      ComplexShortArray::destroy(cfa);
      ComplexShortArray::destroy(tempc);
      ComplexShortArray::destroy(tempc2);
    }
    {
      TEST("ShortToInt and IntToShort");
      ComplexShortArray cs = ComplexShortArray::create(1000);
      ComplexIntArray ci = ComplexIntArray::create(1000);
      ShortArray data = ShortArray((int16_t*)cs.getData(), 2000);
      data.noise();      
      ci.copyFrom(cs);
      for(int n = 0; n < cs.getSize(); ++n){
        CHECK(cs[n].re == ci[n].re);
        CHECK(cs[n].im == cs[n].im);
      }
      data.clear();
      ci.copyTo(cs); 
      for(int n = 0; n < cs.getSize(); ++n){
        CHECK(cs[n].re == ci[n].re);
        CHECK(cs[n].im == cs[n].im);
      }
      ComplexShortArray::destroy(cs);
      ComplexIntArray::destroy(ci);
    }
    {
      TEST("ComplexIntArray.add");
      ComplexIntArray ci = ComplexIntArray::create(1000);
      ComplexIntArray ci2 = ComplexIntArray::create(1000);
      ComplexIntArray ci3 = ComplexIntArray::create(1000);
      ShortArray data1 = ShortArray((int16_t*)ci.getData(), 4000);
      ShortArray data2 = ShortArray((int16_t*)ci2.getData(), 4000);
      data1.noise();
      data2.noise();
      ci.add(ci2, ci3); 
      for(int n = 0; n < ci3.getSize(); ++n){
        CHECK(ci3[n].re == ci[n].re + ci2[n].re);
        CHECK(ci3[n].im == ci[n].im + ci2[n].im);
      }
      ComplexIntArray::destroy(ci);
      ComplexIntArray::destroy(ci2);
      ComplexIntArray::destroy(ci3);
    }
    debugMessage("end of tests!");
  }
};
