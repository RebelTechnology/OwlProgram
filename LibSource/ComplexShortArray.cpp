#include "ComplexShortArray.h"
#include "basicmaths.h"
#include "message.h"
#include "qint.h"

int16_t ComplexShort::getMagnitude(){
#ifdef ARM_CORTEX
  int16_t out;
  int16_t in[2] = {re, im};
  arm_cmplx_mag_q15(in, &out, 1);
  // function above returns 2.14, so we shift it back to 1.15
  // but first we check we do not lose data.
  // TODO: make sure it saturates
  out = out >> 1;
  return out;
#else
  float fre = Q15_TO_FLOAT(re);
  float fim = Q15_TO_FLOAT(im);
  return FLOAT_TO_Q15(sqrtf(fre*fre+fim*fim) + 0.5);
#endif
}

int16_t ComplexShort::getPhase(){
  return FLOAT_TO_Q15(atan2(Q15_TO_FLOAT(im), Q15_TO_FLOAT(re)));
}

void ComplexShort::setPolar(int16_t magnitude, int16_t phase){
  float fm = Q15_TO_FLOAT(magnitude);
  float fp = Q15_TO_FLOAT(phase);
  re = FLOAT_TO_Q15(fm*cosf(fp) + 0.5);
  im = FLOAT_TO_Q15(fm*sinf(fp) + 0.5);
}

int16_t ComplexShortArray::mag(const int i){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_mag_q15((int16_t*)&(data[i]), &result,1);
#else
  result = FLOAT_TO_Q15(sqrtf(Q15_TO_FLOAT(mag2(i))));
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
  for(size_t i=0; i<size; i++){
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
  float re=Q15_TO_FLOAT(data[i].re);
  float im=Q15_TO_FLOAT(data[i].im);
  result=FLOAT_TO_Q15(re*re+im*im);
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
  for(size_t i=0; i<size; i++){
    destination[i]=mag2(i);
  }
#endif  
}

#if 0
void ComplexShortArray::complexDotProduct(ComplexShortArray operand2, ComplexShort& result){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_cmplx_dot_prod_q15 ( (int16_t*)data, (int16_t*)operand2, size, &(result.re), &(result.im) );
#else
  int16_t *pSrcA=(int16_t*)data;
  int16_t *pSrcB=(int16_t*)operand2;
  int16_t realResult=0;    
  int16_t imagResult=0;    
  for(size_t n=0; n<size; n++) {    
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
  int16_t* pSrcA = (int16_t*)data;
  int16_t* pSrcB = (int16_t*)operand2.getData();
  int16_t* pDst = (int16_t*)result.getData();
  for(size_t n=0; n<size; n++) {
    pDst[(2*n)+0] = Q15_MUL_Q15(pSrcA[(2*n)+0], pSrcB[(2*n)+0]) -
                    Q15_MUL_Q15(pSrcA[(2*n)+1], pSrcB[(2*n)+1]);
    pDst[(2*n)+1] = Q15_MUL_Q15(pSrcA[(2*n)+0], pSrcB[(2*n)+1]) +
                    Q15_MUL_Q15(pSrcA[(2*n)+1], pSrcB[(2*n)+0]);
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
  arm_fill_q15(value, (int16_t*)data, size *2 ); //note the *2 multiplier which accounts for real and imaginary parts
#else
  ComplexShort val;
  val.re=value;
  val.im=value;
  setAll(val);
#endif /* ARM_CORTEX */
}

void ComplexShortArray::setAll(ComplexShort value){
  for(size_t n=0; n<size; n++){
    data[n].re=value.re;
    data[n].im=value.im;
  }
}

void ComplexShortArray::setAll(int16_t valueRe, int16_t valueIm){
  ComplexShort value = {valueRe, valueIm};
  setAll(value);
}


void ComplexShortArray::add(ComplexShortArray operand2, ComplexShortArray destination){
  //ASSERT(operand2.size == size && destination.size >= size, "Arrays size mismatch");
#ifdef ARM_CORTEX
  arm_add_q15((int16_t*)data, (int16_t*)operand2.data, (int16_t*)destination.data, size*2);
#else
  for(size_t n=0; n<size; n++){
    destination[n].re = data[n].re + operand2[n].re;
    destination[n].im = data[n].im + operand2[n].im;
  }
#endif /* ARM_CORTEX */  
}

void ComplexShortArray::copyFrom(ComplexShortArray operand2){
#ifdef ARM_CORTEX
  arm_copy_q15((int16_t*)operand2.getData(), (int16_t*)data, size * 2);
#else
  for(size_t n = 0; n < size; ++n){
    data[n].re = operand2[n].re;
    data[n].im = operand2[n].im ;
  }
#endif
}
  
void ComplexShortArray::copyTo(ComplexShortArray operand2){
#ifdef ARM_CORTEX
  arm_copy_q15((int16_t*)data, (int16_t*)operand2.getData(), size * 2);
#else
  for(size_t n = 0; n < size; ++n){
    data[n].re = (int16_t)operand2[n].re;
    data[n].im = (int16_t)operand2[n].im ;
  }
#endif
}

