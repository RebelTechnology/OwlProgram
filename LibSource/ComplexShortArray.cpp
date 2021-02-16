#include "ComplexShortArray.h"
#include "basicmaths.h"
#include "message.h"

int16_t ComplexShortArray::mag(const int i){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_q15((int16_t*)&(data[i]), &result,1);
#else
  #error TODO
  result=sqrtf(mag2(i));
#endif
  return result;
}

void ComplexShortArray::getMagnitudeValues(ShortArray destination){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_q15((int16_t*)data, (int16_t*)destination, size);
  // function above returns 2.14, so we shift it back to 1.15
  destination.shift(1);
#else
  for(int i=0; i<size; i++){
    destination[i]=mag(i);
  }
#endif
}

int16_t ComplexShortArray::mag2(const int i){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_squared_q15((int16_t*)&(data[i]), &result, 1);
  // function above returns 3.13, so we shift it back to 1.15
  // this is saturating
  arm_shift_q15((int16_t*)&result, 2, (int16_t*)&result, 1);
#else
  #error TODO
  int16_t re=data[i].re;
  int16_t im=data[i].im;
  result=re*re+im*im;
#endif  
  return result;
}

void ComplexShortArray::getMagnitudeSquaredValues(ShortArray destination){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_squared_q15((int16_t*)data, (int16_t*)destination, size);
  // function above returns 3.13, so we shift it back to 1.15
  // this is saturating
  arm_shift_q15((int16_t*)destination, 2, (int16_t*)destination,
    destination.getSize());
#else
  for(int i=0; i<size; i++){
    destination[i]=mag2(i);
  }
#endif  
}

#if 0
void ComplexShortArray::complexDotProduct(ComplexShortArray operand2, ComplexFloat& result){
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
#endif /* 0 */

void ComplexShortArray::complexByComplexMultiplication(ComplexShortArray operand2, ComplexShortArray result){
  //ASSERT(operand2.size == size && result.getSize() >= size, "Arrays size mismatch");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mult_cmplx_q15((int16_t*)getData(), (int16_t*)operand2.getData(), (int16_t*)result.getData(), size );  
#else
  ASSERT(false, "TODO");
  float *pSrcA=(float*)data;
  float *pSrcB=(float*)operand2;
  float *pDst=(float*)result;
  for(int n=0; n<size; n++) {        
    pDst[(2*n)+0] = pSrcA[(2*n)+0] * pSrcB[(2*n)+0] - pSrcA[(2*n)+1] * pSrcB[(2*n)+1];        
    pDst[(2*n)+1] = pSrcA[(2*n)+0] * pSrcB[(2*n)+1] + pSrcA[(2*n)+1] * pSrcB[(2*n)+0];        
  }        
#endif  
}

ComplexShortArray ComplexShortArray::create(unsigned int size){
  return ComplexShortArray(new ComplexShort[size], size);
}

void ComplexShortArray::destroy(ComplexShortArray array){
  delete array.data;
}

void ComplexShortArray::setAll(int16_t value){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_fill_q15(value, (int16_t*)data, size*2 ); //note the *2 multiplier which accounts for real and imaginary parts
#else
  ComplexFloat val;
  val.re=value;
  val.im=value;
  setAll(val);
#endif /* ARM_CORTEX */
}

#if 0
void ComplexShortArray::add(ComplexShortArray operand2, ComplexShortArray destination){
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

void ComplexShortArray::add(ComplexShortArray operand2){
  add(operand2, *this);
}

void ComplexShortArray::subtract(ComplexShortArray operand2, ComplexShortArray destination){
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

void ComplexShortArray::subtract(ComplexShortArray operand2){
  subtract(operand2, *this);
}

void ComplexShortArray::getComplexConjugateValues(ComplexShortArray destination){
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

void ComplexShortArray::complexByRealMultiplication(ShortArray operand2, ComplexShortArray result){
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

int ComplexShortArray::getMaxMagnitudeIndex(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxIndex() on it
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

ComplexShortArray ComplexShortArray::subArray(int offset, int length){
  ASSERT(size >= offset+length, "Array too small");
  return ComplexShortArray(data+offset, length);
}

float ComplexShortArray::getMaxMagnitudeValue(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxValue() on it
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

void ComplexShortArray::getRealValues(ShortArray buf){
  for(int n=0; n<size; n++){
    buf[n]=data[n].re;
  }
}

void ComplexShortArray::getImaginaryValues(ShortArray buf){
  for(int n=0; n<size; n++){
    buf[n]=data[n].im;
  }
}

void ComplexShortArray::scale(float factor){
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

/* Copies real values from a ShortArray, sets imaginary values to 0
 */
void ComplexShortArray::copyFrom(ShortArray source){
  for(int n=0; n<min(size,source.getSize()); n++){
    data[n].re=source[n];
    data[n].im=0;
  }
}

/* Copies real part to a ShortArray */
void ComplexShortArray::copyTo(ShortArray destination){
  for(int n=0; n<min(size,destination.getSize()); n++){
    destination[n]=data[n].re;
  }
}

/*BEGIN -- methods copied and adapted from ComplexShortArray.cpp*/
void ComplexShortArray::copyTo(ComplexShortArray destination){
  copyTo(destination, min(size, destination.getSize()));
}

void ComplexShortArray::copyFrom(ComplexShortArray source){
  copyFrom(source, min(size, source.getSize()));
}

void ComplexShortArray::copyTo(ComplexFloat* other, int length){
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

void ComplexShortArray::copyFrom(ComplexFloat* other, int length){
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

void ComplexShortArray::setAll(int16_t value){
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
/*END -- methods copied and adapted from ComplexShortArray.cpp*/

void ComplexShortArray::setAll(ComplexFloat value){
  for(int n=0; n<size; n++){
    data[n].re=value.re;
    data[n].im=value.im;
  }
}

void ComplexShortArray::setAll(float valueRe, float valueIm){
  ComplexFloat value;
  value.re=valueRe;
  value.im=valueIm;
  setAll(value);
}

void ComplexShortArray::setPolar(ShortArray magnitude, ShortArray phase){
  setPolar(magnitude, phase, 0, size);
}
void ComplexShortArray::setPolar(ShortArray magnitude, ShortArray phase, int offset, int count){
  for(int n=offset; n<count+offset; n++){
    data[n].setPolar(magnitude[n], phase[n]);
  }
}
void ComplexShortArray::setPhase(ShortArray phase){
  setPhase(phase, 0, size);
}
void ComplexShortArray::setPhase(ShortArray phase, int offset, int count){
  for(int n=offset; n<count+offset; n++){
    data[n].setPhase(phase[n]);
  }
}
void ComplexShortArray::setPhase(ShortArray phase, ComplexShortArray destination){
  setPhase(phase, 0, size, destination);
}
void ComplexShortArray::setPhase(ShortArray phase, int offset, int count, ComplexShortArray destination){
  ASSERT(destination.getSize()>=count+offset, "Wrong size");
  for(int n=offset; n<count+offset; n++){
    destination.getData()[n].setPolar(getData()[n].getMagnitude(), phase[n]);
  }
}
void ComplexShortArray::setMagnitude(ShortArray magnitude){
  setMagnitude(magnitude, 0, size);
}
void ComplexShortArray::setMagnitude(ShortArray magnitude, int offset, int count){
  setMagnitude(magnitude, offset, count, *this);
}
void ComplexShortArray::setMagnitude(ShortArray magnitude, ComplexShortArray destination){
  setMagnitude(magnitude, 0, size, destination);
}
void ComplexShortArray::setMagnitude(ShortArray magnitude, int offset, int count, ComplexShortArray destination){
  ASSERT(getSize()==magnitude.getSize(),"wrong size0");
  ASSERT(getSize()==destination.getSize(),"wrong size1");
  ASSERT(offset+count<=destination.getSize(), "Wrong size2");
  ASSERT(offset+count<=getSize(), "Wrong size3");
  for(int n=offset; n<count+offset; n++){
    destination.getData()[n].setPolar(magnitude[n], getData()[n].getPhase());
  }
#endif /* if 0 */

