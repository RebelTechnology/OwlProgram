#include "TestPatch.hpp"
#include "ShortArray.h"
#include <limits.h>
#include <stdio.h>

class ShortArrayTestPatch : public TestPatch {
public:
  ShortArrayTestPatch(){
    {
      TEST("Default ctor");
      ShortArray empty;
      CHECK_EQUAL(empty.getSize(), 0);
      CHECK(empty.getData() == NULL);
    }
    {
      TEST("create");
      ShortArray array = ShortArray::create(512);
      CHECK_EQUAL(array.getSize(), 512);
      REQUIRE(array.getData() != NULL);
      for(int i=0; i<512; ++i){
	CHECK_CLOSE(array[i], 0.0, DEFAULT_TOLERANCE);
      }
      ShortArray::destroy(array);
    }
    {
      TEST("getSize, getData");
      const int size = 123;
      ShortArray ar = ShortArray::create(size);
      CHECK_EQUAL(size, ar.getSize());
      int16_t data[size];
      ShortArray ar2 = ShortArray(data, size);
      CHECK_EQUAL(size, ar2.getSize());
      CHECK(data == ar2.getData());
      ShortArray::destroy(ar);
    }
    {
      TEST("minmax");
      ShortArray ar = ShortArray::create(10);
      ar[0] = 0;
      ar[1] = 1;
      ar[2] = 2;
      ar[3] = -1;
      ar[4] = -2;
      ar[5] = -3;
      ar[6] = 3;
      ar[7] = 4;
      ar[8] = 2;
      ar[9] = -2;
      int16_t value;
      int index;
      ar.getMin(&value, &index);
      CHECK(value == -3);
      CHECK(index == 5);
      CHECK(value == ar.getMinValue());
      CHECK(index == ar.getMinIndex());

      ar.getMax(&value, &index);
      CHECK(value == 4);
      CHECK(index == 7);
      CHECK(value == ar.getMaxValue());
      CHECK(index == ar.getMaxIndex());
      ShortArray::destroy(ar);
    }
    {
      TEST("clear,noise");
      ShortArray ar = ShortArray::create(1000);
      int32_t acc = 0;
      for(int n = 0; n < ar.getSize(); ++n){
        acc += abs(ar[n]);
      }
      // A newly created arra should be init'd to 0
      CHECK(acc==0);
      acc = 0;
      ar.noise();
      for(int n = 0; n < ar.getSize(); ++n){
        acc += abs(ar[n]);
      }
      CHECK(acc > 0);
      ar.clear();
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(ar[n] == 0);
      }
      ar.noise();
      CHECK(ar.getMinValue() < -200);
      CHECK(ar.getMaxValue() > 200);
      ShortArray::destroy(ar);
    }
    {
      TEST("copy,equal");
      ShortArray ar = ShortArray::create(100);
      ShortArray ar2 = ShortArray::create(100);
      ar.noise();
      CHECK(ar.equals(ar));
      ar.copyTo(ar2);
      CHECK(ar2.equals(ar));
      ar.clear();
      ar.copyFrom(ar2);
      CHECK(ar.equals(ar));
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
    }
    {
      TEST("rectify");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ar.noise();
      ar2.copyFrom(ar);
      ar2.rectify();
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(abs(ar[n]) == ar2[n]); 
      }
      ar2.clear();
      ar.rectify(ar2);
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(abs(ar[n]) == ar2[n]); 
      }
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
    }
    {
      TEST("reverse");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ar.noise();
      ar.copyTo(ar2);
      ar2.reverse();
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(ar[n] == ar2[ar2.getSize() - n - 1]); 
      }
      ar.reverse(ar2);
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(ar[n] == ar2[ar2.getSize() - n - 1]); 
      }
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
    }
    {
      TEST("reciprocal");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ar.noise();
      ar.copyTo(ar2);
      ar2.reciprocal();
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK_CLOSE(ar2[n], (0.5 + 1.f/ar[n]), 2);
      }
      ar.reciprocal(ar2);
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK_CLOSE(ar2[n], (0.5 + 1.f/ar[n]), 2);
      }
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
    }
    {
      TEST("negate");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ar.noise();
      ar2.copyFrom(ar);
      ar2.negate();
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(ar2[n] == -ar[n]);
      }
      ar.negate(ar2);
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(ar2[n] == -ar[n]);
      }
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
    }
    {
      TEST("rms");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ar.noise();
      ar.reciprocal(ar2);
      float acc = 0;
      for(int n=0; n<ar.getSize(); ++n){
        acc += ar[n] * ar[n] / (float)ar.getSize(); 
      } 
      int rms=(sqrtf(acc + 0.5));
      CHECK_CLOSE(rms, ar.getRms(), 5);
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
    }
    {
      TEST("mean");
      ShortArray ar = ShortArray::create(1000);
      ar.noise();
      int32_t sum = 0;
      for(int n = 0; n < ar.getSize(); ++n){
        sum += ar[n];
      }
      int32_t mean = sum / ar.getSize();
      mean = mean > SHRT_MAX ? SHRT_MAX : mean;
      mean = mean < SHRT_MIN ? SHRT_MIN : mean;

      CHECK_CLOSE(mean, ar.getMean(), 30);
      ShortArray::destroy(ar);
    }
    {
      TEST("power");
      ShortArray ar = ShortArray::create(1000);
      ar.noise();
      int64_t power = 0;
      for(int n = 0; n < ar.getSize(); ++n){
        power += ar[n] * ar[n];
      }
      CHECK_CLOSE(power, ar.getPower(), 30);
      ShortArray::destroy(ar);
    }
    {
      TEST("std");
      // TODO
    }
    {
      TEST("variance");
    }
    {
      TEST("scale");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ShortArray ar3 = ShortArray::create(1000);
      ar.noise();
      ar3.copyFrom(ar);
      uint8_t shift = 0;
      uint16_t factor = 0.5 * SHRT_MAX;
      ar.scale(factor, shift, ar2); 
      ar3.scale(factor, shift);

      for(int n=0; n < ar.getSize(); ++n){
        int32_t value = factor *ar[n];
        int16_t shortValue;
        if(shift > 0)
          value = value << shift;
        else 
          value = value >> -shift;
        if(value > 1 << 30)
          shortValue = SHRT_MAX;
        else if(value < ((int32_t)~(1 << 30)) - 1)
          shortValue = SHRT_MIN;
        else
          shortValue = value >> 15;
        CHECK(ar2[n] == shortValue);
        CHECK(ar3[n] == shortValue);
      }
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
      ShortArray::destroy(ar3);
    }
    {
      TEST("clip");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ShortArray ar3 = ShortArray::create(1000);
      ar.noise();
      int16_t m, M;
      int16_t range = 5350;
      m = -range;
      M = range;
      ar2.copyFrom(ar);
      ar2.clip(range); 
      for(int n = 0; n < ar.getSize(); ++n){
        int16_t value = ar[n];
        value = value > M ? M : value;
        value = value < m ? m : value;
        ar3[n] = (int16_t)value;
      }
      CHECK(ar3.equals(ar2));
      m = -1233;
      M = 4214; 
      ar2.copyFrom(ar);
      ar2.clip(m, M); 
      for(int n = 0; n < ar.getSize(); ++n){
        int16_t value = ar[n];
        value = value > M ? M : value;
        value = value < m ? m : value;
        ar3[n] = (int16_t)value;
      }
      CHECK(ar3.equals(ar2));
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
      ShortArray::destroy(ar3);
    }
    {
      TEST("add");
      ShortArray ar = ShortArray::create(1000);
      ShortArray ar2 = ShortArray::create(1000);
      ShortArray ar3 = ShortArray::create(1000);
      ShortArray ar4 = ShortArray::create(1000);
      ar.noise();
      ar2.noise();
      for(int n = 0; n < ar.getSize(); ++n){
        int32_t value = ar[n] + ar2[n];
        value = value > SHRT_MAX ? SHRT_MAX : value < SHRT_MIN ? SHRT_MIN : value;
        ar3[n] = (int16_t)value;
      }
      ar4.copyFrom(ar);
      ar4.add(ar2);
      CHECK(ar3.equals(ar4));
      ar4.clear();
      ar.add(ar2, ar4);
      CHECK(ar3.equals(ar4));
      ar4.clear();
      int32_t addValue = 124;
      ar4.copyFrom(ar);
      ar4.add(addValue);
      for(int n = 0; n < ar.getSize(); ++n){
        int32_t value = ar[n] + addValue;
        value = value > SHRT_MAX ? SHRT_MAX : value < SHRT_MIN ? SHRT_MIN : value;
        ar3[n] = (int16_t)value;
      }
      CHECK(ar3.equals(ar4));
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
      ShortArray::destroy(ar3);
      ShortArray::destroy(ar4);
    }
    {
      TEST("subtract");
      ShortArray ar = ShortArray::create(1023);
      ShortArray ar2 = ShortArray::create(1023);
      ShortArray ar3 = ShortArray::create(1023);
      ShortArray ar4 = ShortArray::create(1023);
      ar.noise();
      ar2.noise();
      for(int n = 0; n < ar.getSize(); ++n){
        int32_t value = ar[n] - ar2[n];
        value = value > SHRT_MAX ? SHRT_MAX : value < SHRT_MIN ? SHRT_MIN : value;
        ar3[n] = (int16_t)value;
      }
      ar4.copyFrom(ar);
      ar4.subtract(ar2);
      CHECK(ar3.equals(ar4));
      ar4.clear();
      ar.subtract(ar2, ar4);
      CHECK(ar3.equals(ar4));
      ar4.clear();
      int32_t subValue = 1000;
      ar4.copyFrom(ar);
      ar4.subtract(subValue);
      for(int n = 0; n < ar.getSize(); ++n){
        int32_t value = ar[n] - subValue;
        value = value > SHRT_MAX ? SHRT_MAX : value < SHRT_MIN ? SHRT_MIN : value;
        ar3[n] = (int16_t)value;
      }
      CHECK(ar3.equals(ar4));
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
      ShortArray::destroy(ar3);
      ShortArray::destroy(ar4);
    }
    {
      TEST("multiply");
      ShortArray ar = ShortArray::create(1023);
      ShortArray ar2 = ShortArray::create(1023);
      ShortArray ar3 = ShortArray::create(1023);
      ShortArray ar4 = ShortArray::create(1023);
      ar.noise();
      ar2.noise();
      for(int n = 0; n < ar.getSize(); ++n){
        int32_t value = ar[n] * ar2[n];
        value = value >> 15;
        value = value > SHRT_MAX ? SHRT_MAX : value < SHRT_MIN ? SHRT_MIN : value;
        ar3[n] = (int16_t)value;
      }
      ar4.copyFrom(ar);
      ar4.multiply(ar2);
      CHECK(ar3.equals(ar4));
      ar4.clear();
      ar.multiply(ar2, ar4);
      CHECK(ar3.equals(ar4));
      ar4.clear();
      int32_t mulValue = 1000;
      ar4.copyFrom(ar);
      ar4.multiply(mulValue);
      for(int n = 0; n < ar.getSize(); ++n){
        int32_t value = ar[n] * mulValue;
        value = value >> 15;
        value = value > SHRT_MAX ? SHRT_MAX : value < SHRT_MIN ? SHRT_MIN : value;
        ar3[n] = (int16_t)value;
      }
      CHECK(ar3.equals(ar4));
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
      ShortArray::destroy(ar3);
      ShortArray::destroy(ar4);
    }
    {
      TEST("setAll");
      ShortArray ar = ShortArray::create(1023);
      ar.noise();
      int16_t value = -1255;
      ar.setAll(value);
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK(ar[n] == value);
      }
      ShortArray::destroy(ar);
    }
    {
      TEST("subArray");
      ShortArray ar = ShortArray::create(1023);
      ShortArray ar2 = ShortArray::create(1023);
      int32_t offset = 249;
      int32_t length = 167;
      ShortArray sub = ar.subArray(offset, length);
      CHECK(sub.getSize() == length);
      CHECK(sub.getData() == ar.getData() + offset);
      for(int n = 0; n < sub.getSize(); ++n){
        CHECK(sub[n] == ar2[n + offset]);
      }
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
    }
    {
      TEST("convolve");
      //TODO
    }
    {
      TEST("correlate");
      //TODO
    }
    {
      TEST("shift");
      ShortArray ar = ShortArray::create(1023);
      ShortArray ar2 = ShortArray::create(1023);
      ShortArray ar3 = ShortArray::create(1023);
      int shiftValue = 1;
      for(int k = 0; k < 2; ++k){
        ar.noise();
        ar2.copyFrom(ar);
        ar2.shift(shiftValue);
        for(int n = 0; n < ar.getSize(); ++n){
          int16_t value = ar[n];
          if(shiftValue > 0){
            int32_t v = (int32_t)value << shiftValue;
            if(v < SHRT_MIN)
              v = SHRT_MIN;
            else if (v > SHRT_MAX)
              v = SHRT_MAX;
            value = (int16_t)v;
          } else {
            value = value >> -shiftValue;
          }
          ar3[n] = value;
        } 
        CHECK(ar3.equals(ar2));
        shiftValue = -shiftValue;
      }
      ShortArray::destroy(ar);
      ShortArray::destroy(ar2);
      ShortArray::destroy(ar3);
    }
    {
      TEST("float");
      ShortArray ar = ShortArray::create(1023);
      FloatArray fa = FloatArray::create(1023);
      for(int n = 0; n < ar.getSize(); ++n){
        float value = rand() / (float)RAND_MAX * 2 - 1;
        ar.setFloatValue(n, value);
        CHECK_CLOSE(ar[n], value * (float)-SHRT_MIN, 2);
        CHECK_CLOSE(ar.getFloatValue(n), ar[n] / (float)-SHRT_MIN, 0.0004f);
        CHECK_CLOSE(ar.getFloatValue(n), value, 0.0004f);
      }
      fa.noise();
      ar.copyFrom(fa);
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK_CLOSE(ar.getFloatValue(n), fa[n], 0.0004f);
      }
      fa.clear();
      ar.copyTo(fa);
      for(int n = 0; n < ar.getSize(); ++n){
        CHECK_CLOSE(ar.getFloatValue(n), fa[n], 0.0004f);
      }
      ShortArray::destroy(ar);
      FloatArray::destroy(fa);
    }
  }
  //TODO: destroy all the created arrays
};

