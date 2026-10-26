#include "TestPatch.hpp"
#include "IntArray.h"
#include <limits.h>

/*
 * Unit test for IntArray, a fixed-point (1.31) integer array. Tests assert the
 * documented behaviour of the container operations: create/clear, setAll,
 * element access, element-wise add (in-place and to a destination), saturating
 * bitshift, subArray aliasing, and the float <-> 1.31 conversions.
 */
class IntArrayTestPatch : public TestPatch {
public:
  IntArrayTestPatch(){
    {
      TEST("create and clear");
      IntArray a = IntArray::create(8);
      CHECK_EQUAL(a.getSize(), 8);
      for(size_t i=0; i<a.getSize(); ++i)
	CHECK_EQUAL(a[i], 0); // create() clears to zero
      IntArray::destroy(a);
    }
    {
      TEST("setAll and element access");
      IntArray a = IntArray::create(4);
      a.setAll(42);
      for(size_t i=0; i<a.getSize(); ++i)
	CHECK_EQUAL(a.getElement(i), 42);
      a.setElement(2, -7);
      CHECK_EQUAL(a.getElement(2), -7);
      IntArray::destroy(a);
    }
    {
      TEST("add in place");
      IntArray a = IntArray::create(3);
      IntArray b = IntArray::create(3);
      a[0]=1; a[1]=2; a[2]=3;
      b.setAll(10);
      a.add(b); // in-place: a += b
      CHECK_EQUAL(a[0], 11);
      CHECK_EQUAL(a[1], 12);
      CHECK_EQUAL(a[2], 13);
      IntArray::destroy(a);
      IntArray::destroy(b);
    }
    {
      TEST("add to destination");
      IntArray a = IntArray::create(2);
      IntArray b = IntArray::create(2);
      IntArray d = IntArray::create(2);
      a[0]=5; a[1]=6;
      b[0]=1; b[1]=2;
      a.add(b, d); // d = a + b, a unchanged
      CHECK_EQUAL(d[0], 6);
      CHECK_EQUAL(d[1], 8);
      CHECK_EQUAL(a[0], 5); // operands preserved
      IntArray::destroy(a);
      IntArray::destroy(b);
      IntArray::destroy(d);
    }
    {
      TEST("shift");
      IntArray a = IntArray::create(2);
      a[0]=1; a[1]=4;
      a.shift(2); // left shift by 2 == multiply by 4
      CHECK_EQUAL(a[0], 4);
      CHECK_EQUAL(a[1], 16);
      a.shift(-1); // right shift by 1 == divide by 2
      CHECK_EQUAL(a[0], 2);
      CHECK_EQUAL(a[1], 8);
      IntArray::destroy(a);
    }
    {
      TEST("subArray aliases memory");
      IntArray a = IntArray::create(6);
      for(size_t i=0; i<a.getSize(); ++i)
	a[i] = (int)i;
      IntArray sub = a.subArray(2, 3); // elements 2,3,4
      CHECK_EQUAL(sub.getSize(), 3);
      CHECK_EQUAL(sub[0], 2);
      sub[0] = 99; // writes through to the parent (shared memory)
      CHECK_EQUAL(a[2], 99);
      IntArray::destroy(a); // sub shares a's memory, do not destroy separately
    }
    {
      TEST("float conversion round trip");
      IntArray a = IntArray::create(3);
      FloatArray f = FloatArray::create(3);
      FloatArray g = FloatArray::create(3);
      f[0]=0.0f; f[1]=0.5f; f[2]=-0.25f;
      a.fromFloat(f); // float -> 1.31
      a.toFloat(g);    // 1.31 -> float
      for(size_t i=0; i<3; ++i)
	CHECK_CLOSE(g[i], f[i], 1e-6);
      IntArray::destroy(a);
      FloatArray::destroy(f);
      FloatArray::destroy(g);
    }
  }
};
