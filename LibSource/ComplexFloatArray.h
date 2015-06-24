#ifndef __ComplexFloatArray_h__
#define __ComplexFloatArray_h__

#include "FloatArray.h"

struct ComplexFloat {
  float re;
  float im;
};

class ComplexFloatArray {
private:
  ComplexFloat* data;
  int sz;
public:
  ComplexFloatArray() :
    data(NULL), sz(0) {}
  ComplexFloatArray(ComplexFloat* d, int s) :
    data(d), sz(s) {}
  float re(const int i){
    return data[i].re;
  }
  float im(const int i){
    return data[i].im;
  }
  float mag(const int i);
  void getMagnitudeValues(FloatArray& buf);
  float mag2(const int i);
  void getMagnitudeSquaredValues(FloatArray& buf);
  void getComplexConjugateValues(ComplexFloatArray& buf);
  void complexDotProduct(ComplexFloatArray& operand2, ComplexFloat& result);
  void complexByComplexMultiplication(ComplexFloatArray& operand2, ComplexFloatArray& result);
  void complexByRealMultiplication(FloatArray& operand2, ComplexFloatArray& result);
  int getSize(){
    return sz;
  }
  float getMaxMagnitudeValue();
  int getMaxMagnitudeIndex();
  ComplexFloatArray subarray(int offset, int length);
  void getRealValues(FloatArray& buf);
  void getImaginaryValues(FloatArray& buf);
  void setSize(int aSize){
    sz = aSize;
  }
  void setData(ComplexFloat* aData){
    data = aData;
  }
  ComplexFloat& operator [](const int i){
    return data[i];
  }
  operator ComplexFloat*() {
    return data;
  }
  operator float*() {
    return (float *)data;
  }
  static ComplexFloatArray create(int size);
};

#endif // __ComplexFloatArray_h__
