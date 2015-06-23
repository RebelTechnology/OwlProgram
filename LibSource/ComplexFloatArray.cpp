#include "ComplexFloatArray.h"
#include "basicmaths.h"

ComplexFloatArray ComplexFloatArray::createComplexFloatArray(int size){
  return ComplexFloatArray((ComplexFloat*)(new float[size*2]), size);
}

float ComplexFloatArray::mag(const int i){
  float result;
  arm_cmplx_mag_f32((float*)&(data[i]), &result,1);
  return result;
}

void ComplexFloatArray::getMagnitudeValues(FloatArray& dest){
  arm_cmplx_mag_f32((float*)data, (float*)dest, sz);
}

float ComplexFloatArray::mag2(const int i){
  float result;
  arm_cmplx_mag_squared_f32((float*)&(data[i]), &result, 1);
  return result;
}

void ComplexFloatArray::getMagnitudeSquaredValues(FloatArray& dest){
  arm_cmplx_mag_squared_f32((float*)data, (float*)dest, sz);
}

void ComplexFloatArray::complexDotProduct(ComplexFloatArray& operand2, ComplexFloat& result){
  arm_cmplx_dot_prod_f32 ( (float*)data, (float*)operand2, sz, &(result.re), &(result.im) );
}

void ComplexFloatArray::complexByComplexMultiplication(ComplexFloatArray& operand2, ComplexFloatArray& result){
  int minSize=min(sz,operand2.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_cmplx_mult_cmplx_f32 ( (float*)data, (float*)operand2, (float*)result, minSize );  
}

void ComplexFloatArray::getComplexConjugateValues(ComplexFloatArray& buf){
  int minSize= min(sz,buf.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_cmplx_conj_f32( (float*)data, (float*)buf, minSize );  
}
void ComplexFloatArray::complexByRealMultiplication(FloatArray& operand2, ComplexFloatArray& result){
  int minSize= min(sz,operand2.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_cmplx_mult_real_f32 ( (float*)data, (float*)operand2, (float*)result, minSize );  
}

int ComplexFloatArray::getMaxMagnitudeIndex(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxIndex() on it
  float maxMag=-1;
  int maxInd=-1;
  for(int n=0; n<sz; n++){
    float magnitude=mag2(n); //uses mag2 which is faster
    if(magnitude>maxMag){
      maxMag=magnitude;
      maxInd=n;
    }
  }
  return maxInd;
}

float ComplexFloatArray::getMaxMagnitudeValue(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxValue() on it
  float maxMag=-1;
  for(int n=0; n<sz; n++){
    float mag=this->mag2(n);
    if(mag>maxMag){
      maxMag=mag;
    }
  }
  maxMag=sqrtf(maxMag);
  return maxMag;
}

void ComplexFloatArray::getRealValues(FloatArray& buf){
  for(int n=0; n<sz; n++){
    buf[n]=data[n].re;
  }
}

void ComplexFloatArray::getImaginaryValues(FloatArray& buf){
  for(int n=0; n<sz; n++){
    buf[n]=data[n].im;
  }
}
