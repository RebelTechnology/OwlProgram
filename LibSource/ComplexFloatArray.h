#ifndef __ComplexFloatArray_h__
#define __ComplexFloatArray_h__

#include "FloatArray.h"
#include "basicmaths.h"

struct ComplexFloat {
  float re;
  float im;
  float getMagnitude(){
    return sqrtf(re*re+im*im);  
  }
  float getPhase(){
    return atan2(im,re);
  }
  void setPhase(float phase){
    float magnitude=getMagnitude();
    setPolar(magnitude, phase);
  }
  void setMagnitude(float magnitude){
    float phase=getPhase();
    setPolar(magnitude, phase);
  }
  void setPolar(float magnitude, float phase){
    re=magnitude*cosf(phase);
    im=magnitude*sinf(phase);
  }
};

class ComplexFloatArray {
private:
  ComplexFloat* data;
  int size;
public:
  ComplexFloatArray() :
    data(NULL), size(0) {}
  ComplexFloatArray(ComplexFloat* d, int s) :
    data(d), size(s) {}
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
  int getSize() const{
    return size;
  }
  float getMaxMagnitudeValue();
  int getMaxMagnitudeIndex();
  ComplexFloatArray subArray(int offset, int length);
  void getRealValues(FloatArray& buf);
  void getImaginaryValues(FloatArray& buf);
  void scale(float factor);
  ComplexFloat& operator [](const int i){
    return data[i];
  }
  ComplexFloat& operator [](const int i) const{
    return data[i];
  }
  operator ComplexFloat*() {
    return data;
  }
  operator float*() {
    return (float *)data;
  }
  ComplexFloat* getData(){
    return data;
  }
  bool equals(const ComplexFloatArray& other) const{
    if(size!=other.getSize()){
      return false;
    }
    for(int n=0; n<size; n++){
      if(data[n].re!=other[n].re || data[n].im!=other[n].im){
        return false;
      }
    }
    return true;
  }
  static ComplexFloatArray create(int size);
  static void destroy(ComplexFloatArray);
  void copyFrom(FloatArray source);
  void copyFrom(ComplexFloatArray source);
  void copyFrom(ComplexFloat* other, int length);
  void copyTo(FloatArray destination);
  void copyTo(ComplexFloatArray destination);
  void copyTo(ComplexFloat* other, int length);
  void setAll(ComplexFloat value);
  void setAll(float value);
  void setAll(float valueRe, float valueIm);
  void setPolar(FloatArray magnitude, FloatArray phase);
  void setPolar(FloatArray magnitude, FloatArray phase, int offset, int count);
  void setPhase(FloatArray phase);
  void setPhase(FloatArray phase, int offset, int count);
  void setPhase(FloatArray phase, ComplexFloatArray destination);
  void setPhase(FloatArray phase, int offset, int count, ComplexFloatArray destination);
  void setMagnitude(FloatArray magnitude);
  void setMagnitude(FloatArray magnitude, int offset, int count);
  void setMagnitude(FloatArray magnitude, ComplexFloatArray destination);
  void setMagnitude(FloatArray magnitude, int offset, int count, ComplexFloatArray destination);
};

#endif // __ComplexFloatArray_h__
