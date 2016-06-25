#include "ComplexFloatArray.h"
#include "basicmaths.h"
#include "message.h"

float ComplexFloatArray::mag(const int i){
  float result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_f32((float*)&(data[i]), &result,1);
#else
  result=sqrtf(mag2(i));
#endif
  return result;
}

void ComplexFloatArray::getMagnitudeValues(FloatArray destination){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_f32((float*)data, (float*)destination, size);
#else
  for(int i=0; i<size; i++){
    destination[i]=mag(i);
  }
#endif
}

float ComplexFloatArray::mag2(const int i){
  float result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_squared_f32((float*)&(data[i]), &result, 1);
#else
  float re=data[i].re;
  float im=data[i].im;
  result=re*re+im*im;
#endif  
  return result;
}

void ComplexFloatArray::getMagnitudeSquaredValues(FloatArray destination){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_squared_f32((float*)data, (float*)destination, size);
#else
  for(int i=0; i<size; i++){
    destination[i]=mag2(i);
  }
#endif  
}

void ComplexFloatArray::complexDotProduct(ComplexFloatArray operand2, ComplexFloat& result){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_dot_prod_f32 ( (float*)data, (float*)operand2, size, &(result.re), &(result.im) );
#else
  float *pSrcA=(float*)data;
  float *pSrcB=(float*)operand2;
  float realResult=0;    
  float imagResult=0;    
  for(int n=0; n<size; n++) {    
      realResult += pSrcA[(2*n)+0]*pSrcB[(2*n)+0] - pSrcA[(2*n)+1]*pSrcB[(2*n)+1];    
      imagResult += pSrcA[(2*n)+0]*pSrcB[(2*n)+1] + pSrcA[(2*n)+1]*pSrcB[(2*n)+0];    
  }
  result.re=realResult;
  result.im=imagResult;
#endif  
}

void ComplexFloatArray::complexByComplexMultiplication(ComplexFloatArray operand2, ComplexFloatArray result){
  ASSERT(operand2.size == size && result.size >= size, "Arrays size mismatch");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mult_cmplx_f32( (float*)data, (float*)operand2, (float*)result, size );  
#else
  float *pSrcA=(float*)data;
  float *pSrcB=(float*)operand2;
  float *pDst=(float*)result;
  for(int n=0; n<size; n++) {        
    pDst[(2*n)+0] = pSrcA[(2*n)+0] * pSrcB[(2*n)+0] - pSrcA[(2*n)+1] * pSrcB[(2*n)+1];        
    pDst[(2*n)+1] = pSrcA[(2*n)+0] * pSrcB[(2*n)+1] + pSrcA[(2*n)+1] * pSrcB[(2*n)+0];        
  }        
#endif  
}

void ComplexFloatArray::add(ComplexFloatArray operand2, ComplexFloatArray destination){
  ASSERT(operand2.size == size && destination.size >= size, "Arrays size mismatch");
#ifdef ARM_CORTEX
  arm_add_f32((float*)data, (float*)operand2.data, (float*)destination.data, size*2);
#else
  for(int n=0; n<size; n++){
    destination[n].re = data[n].re + operand2[n].re;
    destination[n].im = data[n].im + operand2[n].im;
  }
#endif /* ARM_CORTEX */  
}

void ComplexFloatArray::add(ComplexFloatArray operand2){
  add(operand2, *this);
}

void ComplexFloatArray::subtract(ComplexFloatArray operand2, ComplexFloatArray destination){
  ASSERT(operand2.size == size && destination.size >= size, "Arrays size mismatch");
#ifdef ARM_CORTEX
  arm_sub_f32((float*)data, (float*)operand2.data, (float*)destination.data, size*2);
#else
  for(int n=0; n<size; n++){
    destination[n].re = data[n].re - operand2[n].re;
    destination[n].im = data[n].im - operand2[n].im;
  }
#endif /* ARM_CORTEX */  
}

void ComplexFloatArray::subtract(ComplexFloatArray operand2){
  subtract(operand2, *this);
}

void ComplexFloatArray::getComplexConjugateValues(ComplexFloatArray destination){
  ASSERT(size==destination.getSize(), "Wrong array size");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_conj_f32( (float*)data, (float*)destination, size );  
#else
  float *pSrc=(float*)data;
  float *pDst=(float *)destination;
  for(int n=0; n<size; n++) {        
    pDst[(2*n)+0] = pSrc[(2*n)+0];     // real part        
    pDst[(2*n)+1] = -pSrc[(2*n)+1];    // imag part        
}   
#endif  
}

void ComplexFloatArray::complexByRealMultiplication(FloatArray operand2, ComplexFloatArray result){
  ASSERT(size==operand2.getSize(), "Wrong size"); 
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mult_real_f32 ( (float*)data, (float*)operand2, (float*)result, size );  
#else
  float *pSrcCmplx=(float*)data;
  float *pSrcReal=(float*)operand2;
  float *pCmplxDst=(float*)result;
  for(int n=0; n<size; n++) {
      pCmplxDst[(2*n)+0] = pSrcCmplx[(2*n)+0] * pSrcReal[n];        
      pCmplxDst[(2*n)+1] = pSrcCmplx[(2*n)+1] * pSrcReal[n];        
  }        
#endif
}

int ComplexFloatArray::getMaxMagnitudeIndex(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxIndex() on it
  float maxMag=-1;
  int maxInd=-1;
  for(int n=0; n<size; n++){
    float magnitude=mag2(n); //uses mag2 which is faster
    if(magnitude>maxMag){
      maxMag=magnitude;
      maxInd=n;
    }
  }
  return maxInd;
}

ComplexFloatArray ComplexFloatArray::subArray(int offset, int length){
  ASSERT(size >= offset+length, "Array too small");
  return ComplexFloatArray(data+offset, length);
}

float ComplexFloatArray::getMaxMagnitudeValue(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxValue() on it
  float maxMag=-1;
  for(int n=0; n<size; n++){
    float mag=this->mag2(n);
    if(mag>maxMag){
      maxMag=mag;
    }
  }
  maxMag=sqrtf(maxMag);
  return maxMag;
}

void ComplexFloatArray::getRealValues(FloatArray buf){
  for(int n=0; n<size; n++){
    buf[n]=data[n].re;
  }
}

void ComplexFloatArray::getImaginaryValues(FloatArray buf){
  for(int n=0; n<size; n++){
    buf[n]=data[n].im;
  }
}

void ComplexFloatArray::scale(float factor){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX  
  arm_scale_f32((float*)data, factor, (float*)data, size*2); //the *2 accounts for the fact that both real and imaginary parts are scaled
#else
  for(int n=0; n<size; n++){
    data[n].re *= factor;
    data[n].im *= factor;
  }
#endif
}

ComplexFloatArray ComplexFloatArray::create(int size){
  return ComplexFloatArray(new ComplexFloat[size], size);
}

void ComplexFloatArray::destroy(ComplexFloatArray array){
  delete array.data;
}

/* Copies real values from a FloatArray, sets imaginary values to 0
 */
void ComplexFloatArray::copyFrom(FloatArray source){
  for(int n=0; n<min(size,source.getSize()); n++){
    data[n].re=source[n];
    data[n].im=0;
  }
}

/* Copies real part to a FloatArray */
void ComplexFloatArray::copyTo(FloatArray destination){
  for(int n=0; n<min(size,destination.getSize()); n++){
    destination[n]=data[n].re;
  }
}

/*BEGIN -- methods copied and adapted from ComplexFloatArray.cpp*/
void ComplexFloatArray::copyTo(ComplexFloatArray destination){
  copyTo(destination, min(size, destination.getSize()));
}

void ComplexFloatArray::copyFrom(ComplexFloatArray source){
  copyFrom(source, min(size, source.getSize()));
}

void ComplexFloatArray::copyTo(ComplexFloat* other, int length){
  ASSERT(size >= length, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_f32((float *)data, (float *)other, length*sizeof(ComplexFloat)/sizeof(float));
#else
  for(int n=0; n<length; n++){
    other[n].re=data[n].re;
    other[n].im=data[n].im;
  }
#endif /* ARM_CORTEX */
}

void ComplexFloatArray::copyFrom(ComplexFloat* other, int length){
  ASSERT(size >= length, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_f32((float *)other, (float *)data, length*sizeof(ComplexFloat)/sizeof(float));  //note the *2 multiplier which accounts for real and imaginary parts
#else
  for(int n=0; n<length; n++){
    data[n].re=other[n].re;
    data[n].im=other[n].im;
  }
#endif /* ARM_CORTEX */
}

void ComplexFloatArray::setAll(float value){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_fill_f32(value, (float *)data, size *2 ); //note the *2 multiplier which accounts for real and imaginary parts
#else
  ComplexFloat val;
  val.re=value;
  val.im=value;
  setAll(val);
#endif /* ARM_CORTEX */
}
/*END -- methods copied and adapted from ComplexFloatArray.cpp*/

void ComplexFloatArray::setAll(ComplexFloat value){
  for(int n=0; n<size; n++){
    data[n].re=value.re;
    data[n].im=value.im;
  }
}

void ComplexFloatArray::setAll(float valueRe, float valueIm){
  ComplexFloat value;
  value.re=valueRe;
  value.im=valueIm;
  setAll(value);
}

void ComplexFloatArray::setPolar(FloatArray magnitude, FloatArray phase){
  setPolar(magnitude, phase, 0, size);
}
void ComplexFloatArray::setPolar(FloatArray magnitude, FloatArray phase, int offset, int count){
  for(int n=offset; n<count+offset; n++){
    data[n].setPolar(magnitude[n], phase[n]);
  }
}
void ComplexFloatArray::setPhase(FloatArray phase){
  setPhase(phase, 0, size);
}
void ComplexFloatArray::setPhase(FloatArray phase, int offset, int count){
  for(int n=offset; n<count+offset; n++){
    data[n].setPhase(phase[n]);
  }
}
void ComplexFloatArray::setPhase(FloatArray phase, ComplexFloatArray destination){
  setPhase(phase, 0, size, destination);
}
void ComplexFloatArray::setPhase(FloatArray phase, int offset, int count, ComplexFloatArray destination){
  ASSERT(destination.getSize()>=count+offset, "Wrong size");
  for(int n=offset; n<count+offset; n++){
    destination.getData()[n].setPolar(getData()[n].getMagnitude(), phase[n]);
  }
}
void ComplexFloatArray::setMagnitude(FloatArray magnitude){
  setMagnitude(magnitude, 0, size);
}
void ComplexFloatArray::setMagnitude(FloatArray magnitude, int offset, int count){
  setMagnitude(magnitude, offset, count, *this);
}
void ComplexFloatArray::setMagnitude(FloatArray magnitude, ComplexFloatArray destination){
  setMagnitude(magnitude, 0, size, destination);
}
void ComplexFloatArray::setMagnitude(FloatArray magnitude, int offset, int count, ComplexFloatArray destination){
  ASSERT(getSize()==magnitude.getSize(),"wrong size0");
  ASSERT(getSize()==destination.getSize(),"wrong size1");
  ASSERT(offset+count<=destination.getSize(), "Wrong size2");
  ASSERT(offset+count<=getSize(), "Wrong size3");
  for(int n=offset; n<count+offset; n++){
    destination.getData()[n].setPolar(magnitude[n], getData()[n].getPhase());
  }
}
