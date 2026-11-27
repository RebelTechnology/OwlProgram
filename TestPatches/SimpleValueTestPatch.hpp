#include "TestPatch.hpp"
#include "SimpleValue.h"

/*
 * Unit test for SimpleValue, documented as a base class for a value that can be
 * set, updated and retrieved, with absolute updates via `=`/update(), relative
 * updates via `+=`/`-=`, and scaling via `*=`/`/=`, plus an implicit conversion
 * back to the underlying type. Tests assert exactly those documented operators
 * for both the float and int specialisations.
 */
class SimpleValueTestPatch : public TestPatch {
public:
  SimpleValueTestPatch(){
    {
      TEST("construct and get");
      SimpleValue<float> a;          // default constructs to 0
      CHECK_CLOSE(a.get(), 0.0f, 0.0001);
      SimpleValue<float> b(2.5f);
      CHECK_CLOSE(b.get(), 2.5f, 0.0001);
    }
    {
      TEST("assignment is absolute update");
      SimpleValue<float> a;
      a = 1.25f;
      CHECK_CLOSE(a.get(), 1.25f, 0.0001);
      a.update(3.0f);
      CHECK_CLOSE(a.get(), 3.0f, 0.0001);
      a.reset(-1.0f);
      CHECK_CLOSE(a.get(), -1.0f, 0.0001);
    }
    {
      TEST("relative updates");
      SimpleValue<float> a(10.0f);
      a += 5.0f;
      CHECK_CLOSE(a.get(), 15.0f, 0.0001);
      a -= 3.0f;
      CHECK_CLOSE(a.get(), 12.0f, 0.0001);
    }
    {
      TEST("scaling updates");
      SimpleValue<float> a(4.0f);
      a *= 2.0f;
      CHECK_CLOSE(a.get(), 8.0f, 0.0001);
      a /= 4.0f;
      CHECK_CLOSE(a.get(), 2.0f, 0.0001);
    }
    {
      TEST("implicit conversion");
      SimpleValue<float> a(7.5f);
      float f = a; // operator T()
      CHECK_CLOSE(f, 7.5f, 0.0001);
    }
    {
      TEST("int specialisation");
      SimpleValue<int> a(5);
      a += 3;
      CHECK_EQUAL(a.get(), 8);
      a *= 2;
      CHECK_EQUAL(a.get(), 16);
      int i = a;
      CHECK_EQUAL(i, 16);
    }
  }
};
