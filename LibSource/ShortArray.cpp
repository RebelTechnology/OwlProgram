#include "ShortArray.h"
#include "basicmaths.h"
#include "message.h"
#include <string.h>
#include <limits.h>

#ifndef ARM_CORTEX
static int16_t saturateTo16(int64_t value){
  if(value > SHRT_MAX)
    value = SHRT_MAX;
  else if(value < SHRT_MIN)
    value = SHRT_MIN;
  return value;
}
#endif

void ShortArray::getMin(int16_t* value, int* index){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  uint32_t idx;
  arm_min_q15(data, size, value, &idx);
  *index = (int)idx;
#else
  *value=data[0];
  *index=0;
  for(size_t n=1; n<size; n++){
    int16_t currentValue=data[n];
    if(currentValue<*value){
      *value=currentValue;
      *index=n;
    }
  }
#endif
}

int16_t ShortArray::getMinValue(){
  int16_t value;
  int index;
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  getMin(&value, &index);
  return value;
}

int ShortArray::getMinIndex(){
  int16_t value;
  int index;
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  getMin(&value, &index);
  return index;
}

void ShortArray::getMax(int16_t* value, int* index){
  ASSERT(size>0, "Wrong size");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX 
  uint32_t idx;
  arm_max_q15(data, size, value, &idx);
  *index = (int)idx;
#else
  *value=data[0];
  *index=0;
  for(size_t n=1; n<size; n++){
    int16_t currentValue=data[n];
    if(currentValue>*value){
      *value=currentValue;
      *index=n;
    }
  }
#endif
}

int16_t ShortArray::getMaxValue(){
  int16_t value;
  int index;
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  getMax(&value, &index);
  return value;
}

int ShortArray::getMaxIndex(){
  int16_t value;
  int index;
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  getMax(&value, &index);
  return index;
}

void ShortArray::rectify(ShortArray& destination){ //this is actually "copy data with rectifify"
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX   
  arm_abs_q15(data, destination.getData(), size);
#else
  size_t minSize= min(size,destination.getSize()); //TODO: shall we take this out and allow it to segfault?
  for(size_t n=0; n<minSize; n++){
    destination[n] = fabs(data[n]);
  }
#endif  
}

void ShortArray::rectify(){//in place
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  rectify(*this);
}

void ShortArray::reverse(ShortArray& destination){ //this is actually "copy data with reverse"
  if(destination==*this){ //make sure it is not called "in-place"
    reverse();
    return;
  }
  for(size_t n=0; n<size; n++){
    destination[n]=data[size-n-1];
  }
}

void ShortArray::reverse(){//in place
  for(size_t n=0; n<size/2; n++){
    int16_t temp=data[n];
    data[n]=data[size-n-1];
    data[size-n-1]=temp;
  }
}

void ShortArray::reciprocal(ShortArray& destination){
  int16_t* data = getData();
  for(size_t n=0; n<getSize(); n++)
    destination[n] = (int16_t)(0.5 + 1.0f/data[n]);
}

void ShortArray::reciprocal(){//in place
  reciprocal(*this);
}

int16_t ShortArray::getRms(){
  int16_t result;
#ifdef ARM_CORTEX  
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  arm_rms_q15 (data, size, &result);
#else
  int64_t value = 0;
  for(size_t n=0; n < size; ++n){
    value += data[n] * data[n];
  }
  value = sqrtf(value / size);
  result = saturateTo16(value);
#endif
  return result;
}

int16_t ShortArray::getMean(){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX  
  arm_mean_q15 (data, size, &result);
#else
  int32_t value = 0;
  for(size_t n=0; n < size; n++){
    value += data[n];
  }
  value = value / size;
  result = saturateTo16(value);
#endif
  return result;
}

int64_t ShortArray::getPower(){
  int64_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX  
  arm_power_q15 (data, size, &result);
#else
  result=0;
  int16_t *pSrc = data;
  for(size_t n=0; n < size; n++){
    int32_t value = (int32_t)pSrc[n]*pSrc[n];
    result += saturateTo16(value >> 15);
  }
#endif
  return result;
}

int16_t ShortArray::getStandardDeviation(){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX  
  arm_std_q15 (data, size, &result);
#else
  result=sqrtf(getVariance());
#endif
  return result;
}

int16_t ShortArray::getVariance(){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX  
  arm_var_q15(data, size, &result);
#else
  int16_t sumOfSquares=getPower();
  int16_t sum=0;
  for(size_t n=0; n<size; n++){
    sum+=data[n];
  }
  result=(sumOfSquares - sum*sum/size) / (size - 1);
#endif
  return result;
}

void ShortArray::clip(int16_t max){
  for(size_t n=0; n<size; n++){
    if(data[n]>max)
      data[n]=max;
    else if(data[n]<-max)
      data[n]=-max;
  }
}
void ShortArray::clip(int16_t min, int16_t max){
  for(size_t n=0; n<size; n++){
    if(data[n]>max)
      data[n]=max;
    else if(data[n]<min)
      data[n]=min;
  }
}
ShortArray ShortArray::subArray(int offset, size_t length){
  ASSERT(size >= offset+length, "Array too small");
  return ShortArray(data+offset, length);
}

void ShortArray::copyTo(ShortArray destination){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  copyTo(destination, min(size, destination.getSize()));
}

void ShortArray::copyFrom(ShortArray source){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  copyFrom(source, min(size, source.getSize()));
}

void ShortArray::copyTo(int16_t* other, size_t length){
  ASSERT(size >= length, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_q15(data, other, length);
#else
  memcpy((void *)other, (void *)getData(), length*sizeof(int16_t));
#endif /* ARM_CORTEX */
}

void ShortArray::copyFrom(int16_t* other, size_t length){
  ASSERT(size >= length, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_q15(other, data, length);
#else
  memcpy((void *)getData(), (void *)other, length*sizeof(int16_t));
#endif /* ARM_CORTEX */
}

void ShortArray::insert(ShortArray source, int sourceOffset, int destinationOffset, size_t samples){
  ASSERT(size >= destinationOffset+samples, "Array too small");
  ASSERT(source.size >= sourceOffset+samples, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_q15(source.data+sourceOffset, data+destinationOffset, samples);  
#else
  memcpy((void*)(getData()+destinationOffset), (void*)(source.getData()+sourceOffset), samples*sizeof(int16_t));
#endif /* ARM_CORTEX */
}

void ShortArray::insert(ShortArray source, int destinationOffset, size_t samples){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  insert(source, 0, destinationOffset, samples);
}

void ShortArray::move(int fromIndex, int toIndex, size_t samples){
  ASSERT(size >= toIndex+samples, "Array too small");
  memmove(data+toIndex, data+fromIndex, samples*sizeof(int16_t)); //TODO: evaluate if it is appropriate to use arm_copy_q15 for this method
}

void ShortArray::setAll(int16_t value){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_fill_q15(value, data, size);
#else
  for(size_t n=0; n<size; n++){
    data[n]=value;
  }
#endif /* ARM_CORTEX */
}

void ShortArray::add(ShortArray operand2, ShortArray destination){ //allows in-place
  ASSERT(operand2.size >= size &&  destination.size<=size, "Arrays must be matching size");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  /* despite not explicitely documented in the CMSIS documentation,
      this has been tested to behave properly even when pSrcA==pDst
      void 	arm_add_q15 (int16_t32_t *pSrcA, int16_t32_t *pSrcB, int16_t32_t *pDst, uint32_t blockSize)
  */
  arm_add_q15(data, operand2.data, destination.data, size);
#else
  for(size_t n=0; n<size; n++){
    int32_t value = data[n] + operand2[n];
    destination[n] = saturateTo16(value);
  }
#endif /* ARM_CORTEX */
}

void ShortArray::add(ShortArray operand2){ //in-place
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  add(operand2, *this);
}

void ShortArray::add(int16_t scalar){
#ifdef ARM_CORTEX
  int16_t doubleScalar[2] = {scalar, scalar};
  q15_t * pSrcA = data;
  q15_t * pSrcB = (int16_t*)&doubleScalar;
  q15_t * pDst = data;
  uint32_t blockSize = size;
  
  uint32_t blkCnt;                               /* loop counter */
/* Run the below code for Cortex-M4 and Cortex-M3 */
  q31_t inA1, inA2, inB1, inB2;

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.    
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = A + B */
    /* Add and then store the results in the destination buffer. */
    inA1 = *__SIMD32(pSrcA)++;
    inA2 = *__SIMD32(pSrcA)++;
    inB1 = *__SIMD32(pSrcB); //HERE: do not increment pSrcB
    inB2 = *__SIMD32(pSrcB); //HERE: do not increment pSrcB

    *__SIMD32(pDst)++ = __QADD16(inA1, inB1);
    *__SIMD32(pDst)++ = __QADD16(inA2, inB2);

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.    
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* C = A + B */
    /* Add and then store the results in the destination buffer. */
    *pDst++ = (q15_t) __QADD16(*pSrcA++, *pSrcB); //HERE: do not increment pSrcB

    /* Decrement the loop counter */
    blkCnt--;
  }
#else
  for(size_t n=0; n < size; ++n){
    int32_t value = data[n] + scalar;
    data[n] = saturateTo16(value);
  } 
#endif
}

void ShortArray::subtract(ShortArray operand2, ShortArray destination){ //allows in-place
  ASSERT(operand2.size == size && destination.size >= size, "Arrays size mismatch");
#ifdef ARM_CORTEX
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  /* despite not explicitely documented in the CMSIS documentation,
      this has been tested to behave properly even when pSrcA==pDst
      void 	arm_sub_q15 (int16_t32_t *pSrcA, int16_t32_t *pSrcB, int16_t32_t *pDst, uint32_t blockSize)
  */
  arm_sub_q15(data, operand2.data, destination.data, size);
#else
  for(size_t n=0; n < size; ++n){
    int32_t value = data[n] - operand2[n];
    destination[n] = saturateTo16(value);
  }
#endif /* ARM_CORTEX */
}

void ShortArray::subtract(ShortArray operand2){ //in-place
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  subtract(operand2, *this);
}

void ShortArray::subtract(int16_t scalar) 
{
#ifdef ARM_CORTEX
  // this method is modelled on arm_sub_q15 
  // from <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  // just avoid incrementing the pSrcB pointer(see below)
  int16_t *pSrcA = data;
  int16_t scalarDouble[2] = {scalar, scalar};
  int16_t *pSrcB = scalarDouble; 
  int16_t *pDst = data;
  uint32_t blockSize = size;

  uint32_t blkCnt;                               /* loop counter */
/* Run the below code for Cortex-M4 and Cortex-M3 */
  q31_t inA1, inA2;
  q31_t inB1, inB2;

  /*loop Unrolling */
  blkCnt = blockSize >> 2u;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.    
   ** a second loop below computes the remaining 1 to 3 samples. */
  while(blkCnt > 0u)
  {
    /* C = A - B */
    /* Subtract and then store the results in the destination buffer two samples at a time. */
    inA1 = *__SIMD32(pSrcA)++;
    inA2 = *__SIMD32(pSrcA)++;
    inB1 = *__SIMD32(pSrcB); //HERE: not incrementing pSrcB
    inB2 = *__SIMD32(pSrcB); //HERE: not incrementing pSrcB

    *__SIMD32(pDst)++ = __QSUB16(inA1, inB1);
    *__SIMD32(pDst)++ = __QSUB16(inA2, inB2);

    /* Decrement the loop counter */
    blkCnt--;
  }

  /* If the blockSize is not a multiple of 4, compute any remaining output samples here.    
   ** No loop unrolling is used. */
  blkCnt = blockSize % 0x4u;

  while(blkCnt > 0u)
  {
    /* C = A - B */
    /* Subtract and then store the result in the destination buffer. */
    *pDst++ = (q15_t) __QSUB16(*pSrcA++, *pSrcB); //HERE: not incrementing pSrcB

    /* Decrement the loop counter */
    blkCnt--;
  }
#else
  for(size_t n = 0; n < size; ++n){
    int32_t value = data[n] - scalar;
    data[n] = saturateTo16(value);
  } 
#endif
}

void ShortArray::multiply(ShortArray operand2, ShortArray destination){ //allows in-place
  ASSERT(operand2.size == size &&  destination.size==size, "Arrays must be same size");
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  /* despite not explicitely documented in the CMSIS documentation,
      this has been tested to behave properly even when pSrcA==pDst
      void 	arm_mult_q15 (int16_t32_t *pSrcA, int16_t32_t *pSrcB, int16_t32_t *pDst, uint32_t blockSize)
  */
    arm_mult_q15(data, operand2.data, destination, size);
#else
  for(size_t n=0; n<size; n++){
    int32_t value = data[n] * operand2[n];
    destination[n] = saturateTo16(value >> 15);
  }

#endif /* ARM_CORTEX */
}

void ShortArray::multiply(ShortArray operand2){ //in-place
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  multiply(operand2, *this);
}

void ShortArray::multiply(int16_t scalar){
  // this method is modelled on  arm_mult_q15 
  // from <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  // just avoid incrementing the pSrcB pointer(see below)
#ifdef ARM_CORTEX
  arm_scale_q15(data, scalar, 0, data, size);
#else 
  for(size_t n = 0; n < size; ++n){
    int32_t value = data[n] * scalar;
    data[n] = saturateTo16(value >> 15);
  }
#endif
}

void ShortArray::negate(ShortArray& destination){//allows in-place
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_negate_q15(data, destination.getData(), size); 
#else
  for(size_t n=0; n<size; n++){
    destination[n]=-data[n];
  }
#endif /* ARM_CORTEX */
}
void ShortArray::negate(){
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  negate(*this);
}

void ShortArray::noise(){
  noise(-32768, 32767);
}

void ShortArray::noise(int16_t min, int16_t max){
  uint16_t amplitude = abs((int32_t)max-(int32_t)min);
  int16_t offset = min;
  //debugMessage("amp off", amplitude,offset);
  for(size_t n=0; n<size; n++){
    data[n]=(rand()/((float)RAND_MAX)) * amplitude + offset;
  }
}


void ShortArray::convolve(ShortArray operand2, ShortArray destination){
  ASSERT(destination.size >= size + operand2.size -1, "Destination array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_conv_q15(data, size, operand2.data, operand2.size, destination);
#else
  size_t size2=operand2.getSize();
  for (size_t n=0; n<size+size2-1; n++){
    size_t n1=n;
    destination[n] =0;
    for(size_t k=0; k<size2; k++){
      if(n1>=0 && n1<size)
        destination[n]+=data[n1]*operand2[k];
      n1--;
    }
  }
#endif /* ARM_CORTEX */
}

void ShortArray::convolve(ShortArray operand2, ShortArray destination, int offset, size_t samples){
  ASSERT(destination.size >= size + operand2.size -1, "Destination array too small"); //TODO: change this condition to the actual size being written(will be samples+ tail)
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  //TODO: I suspect a bug in arm_conv_partial_q15
  //it seems that destination[n] is left unchanged for n<offset
  //and the result is actually stored from destination[offset] onwards
  //that is, in the same position where they would be if a full convolution was performed.
  //This requires (destination.size >= size + operand2.size -1). Ideally you would want destination to be smaller
  arm_conv_partial_q15(data, size, operand2.data, operand2.size, destination.getData(), offset, samples);
#else
  //this implementations reproduces the (buggy?) behaviour of arm_conv_partial (see comment above and inline comments below)
  /*
  This implementation is just a copy/paste/edit from the overloaded method
  */
  size_t size2=operand2.getSize();
  for (size_t n=offset; n<offset+samples; n++){
    size_t n1=n;
    destination[n] =0; //this should be [n-offset]
    for(size_t k=0; k<size2; k++){
      if(n1>=0 && n1<size)
        destination[n]+=data[n1]*operand2[k];//this should be destination[n-offset]
      n1--;
    }
  }
#endif /* ARM_CORTEX */
}

void ShortArray::correlate(ShortArray operand2, ShortArray destination){ 
  destination.setAll(0);
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  correlateInitialized(operand2, destination);
}
void ShortArray::correlateInitialized(ShortArray operand2, ShortArray destination){
  ASSERT(destination.size >= size+operand2.size-1, "Destination array too small"); //TODO: change CMSIS docs, which state a different size
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_correlate_q15(data, size, operand2.data, operand2.size, destination);
#else
  //correlation is the same as a convolution where one of the signals is flipped in time
  //so we flip in time operand2 
  operand2.reverse();
  //and convolve it with fa to obtain the correlation
  convolve(operand2, destination);
  //and we flip back operand2, so that the input is not modified
  operand2.reverse();
#endif /* ARM_CORTEX */  
}

void ShortArray::shift(int shiftValue){
#ifdef ARM_CORTEX
    arm_shift_q15(data, shiftValue, data, size);
#else
    if(shiftValue >= 0){
      for(size_t n=0; n<size; n++)
	data[n] = data[n] << shiftValue;
    }else{
      shiftValue = -shiftValue;
      for(size_t n=0; n<size; n++)
	data[n] = data[n] >> shiftValue;
    }
#endif
  }

ShortArray ShortArray::create(int size){
  ShortArray fa(new int16_t[size], size);
  fa.clear();
  return fa;
}

void ShortArray::destroy(ShortArray array){
  delete array.data;
}

void ShortArray::setFloatValue(uint32_t n, float value){
  data[n] = value * -SHRT_MIN;
}

float ShortArray::getFloatValue(uint32_t n){
  return data[n] / (float)-SHRT_MIN;
}

void ShortArray::copyFrom(FloatArray source){
  ASSERT(source.getSize() == size, "Size does not match");
#ifdef ARM_CORTEX
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  arm_float_to_q15((float*)source, data, size);
#else
  for(size_t n = 0; n < size; ++n){
    setFloatValue(n, source[n]);
  }
#endif
}

void ShortArray::copyTo(FloatArray destination){
  ASSERT(destination.getSize() == size, "Size does not match");
#ifdef ARM_CORTEX
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  arm_q15_to_float(data, (float*)destination, size);
#else
  for(size_t n = 0; n < size; ++n){
    destination[n] = getFloatValue(n);
  }
#endif
}
