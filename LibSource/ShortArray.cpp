#include "ShortArray.h"
#include "basicmaths.h"
#include "message.h"
#include <string.h>

 ShortArray::ShortArray() :
   data(NULL), size(0) {}

 ShortArray::ShortArray(int16_t* d, int s) :
   data(d), size(s) {}

void ShortArray::getMin(int16_t* value, int* index){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  uint32_t idx;
  arm_min_q15(data, size, value, &idx);
  *index = (int)idx;
#else
  *value=data[0];
  *index=0;
  for(int n=1; n<size; n++){
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
  for(int n=1; n<size; n++){
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
  int minSize= min(size,destination.getSize()); //TODO: shall we take this out and allow it to segfault?
  for(int n=0; n<minSize; n++){
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
  for(int n=0; n<size; n++){
    destination[n]=data[size-n-1];
  }
}

void ShortArray::reverse(){//in place
  for(int n=0; n<size/2; n++){
    int16_t temp=data[n];
    data[n]=data[size-n-1];
    data[size-n-1]=temp;
  }
}

void ShortArray::reciprocal(ShortArray& destination){
  int16_t* data = getData();
  for(int n=0; n<getSize(); n++)
    destination[n] = (int16_t)(0.5 + 1.0f/data[n]);
}

void ShortArray::reciprocal(){//in place
  reciprocal(*this);
}

int16_t ShortArray::getRms(){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX  
  arm_rms_q15 (data, size, &result);
#else
  ASSERT(false, "TODO");
  result=0;
  int16_t *pSrc= data;
  for(int n=0; n<size; n++){
    result += pSrc[n]*pSrc[n];
  }
  result=sqrtf(result/size);
#endif
  return result;
}

int16_t ShortArray::getMean(){
  int16_t result;
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX  
  arm_mean_q15 (data, size, &result);
#else
  result=0;
  for(int n=0; n < size; n++){
    result+=data[n];
  }
  result=result/size;
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
  for(int n=0; n < size; n++){
    result += (int32_t)pSrc[n]*pSrc[n];
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
  for(int n=0; n<size; n++){
    sum+=data[n];
  }
  result=(sumOfSquares - sum*sum/size) / (size - 1);
#endif
  return result;
}


void ShortArray::scale(int16_t factor, int8_t shift, ShortArray destination){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  arm_scale_q15(data, factor, shift, destination, size);
}

void ShortArray::scale(int16_t factor, int8_t shift){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  scale(factor, shift, *this);
}

void ShortArray::clip(int16_t max){
  for(int n=0; n<size; n++){
    if(data[n]>max)
      data[n]=max;
    else if(data[n]<-max)
      data[n]=-max;
  }
}
void ShortArray::clip(int16_t min, int16_t max){
  for(int n=0; n<size; n++){
    if(data[n]>max)
      data[n]=max;
    else if(data[n]<min)
      data[n]=min;
  }
}
ShortArray ShortArray::subArray(int offset, int length){
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

void ShortArray::copyTo(int16_t* other, int length){
  ASSERT(size >= length, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_q15(data, other, length);
#else
  memcpy((void *)other, (void *)getData(), length*sizeof(int16_t));
#endif /* ARM_CORTEX */
}

void ShortArray::copyFrom(int16_t* other, int length){
  ASSERT(size >= length, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_q15(other, data, length);
#else
  memcpy((void *)getData(), (void *)other, length*sizeof(int16_t));
#endif /* ARM_CORTEX */
}

void ShortArray::insert(ShortArray source, int sourceOffset, int destinationOffset, int samples){
  ASSERT(size >= destinationOffset+samples, "Array too small");
  ASSERT(source.size >= sourceOffset+samples, "Array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_copy_q15(source.data+sourceOffset, data+destinationOffset, samples);  
#else
  memcpy((void*)(getData()+destinationOffset), (void*)(source.getData()+sourceOffset), samples*sizeof(int16_t));
#endif /* ARM_CORTEX */
}

void ShortArray::insert(ShortArray source, int destinationOffset, int samples){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  insert(source, 0, destinationOffset, samples);
}

void ShortArray::move(int fromIndex, int toIndex, int samples){
  ASSERT(size >= toIndex+samples, "Array too small");
  memmove(data+toIndex, data+fromIndex, samples*sizeof(int16_t)); //TODO: evaluate if it is appropriate to use arm_copy_q15 for this method
}

void ShortArray::setAll(int16_t value){
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_fill_q15(value, data, size);
#else
  for(int n=0; n<size; n++){
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
  for(int n=0; n<size; n++){
    destination[n]=data[n]+operand2[n];
  }
#endif /* ARM_CORTEX */
}

void ShortArray::add(ShortArray operand2){ //in-place
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  add(operand2, *this);
}

void ShortArray::add(int16_t scalar){
  for(int n=0; n<size; n++){
    data[n]+=scalar;
  } 
}

void ShortArray::subtract(ShortArray operand2, ShortArray destination){ //allows in-place
  ASSERT(operand2.size == size && destination.size >= size, "Arrays size mismatch");
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  /* despite not explicitely documented in the CMSIS documentation,
      this has been tested to behave properly even when pSrcA==pDst
      void 	arm_sub_q15 (int16_t32_t *pSrcA, int16_t32_t *pSrcB, int16_t32_t *pDst, uint32_t blockSize)
  */
  arm_sub_q15(data, operand2.data, destination.data, size);
  #else
  for(int n=0; n<size; n++){
    destination[n]=data[n]-operand2[n];
  }
  #endif /* ARM_CORTEX */
}

void ShortArray::subtract(ShortArray operand2){ //in-place
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  subtract(operand2, *this);
}

void ShortArray::subtract(int16_t scalar){
  for(int n=0; n<size; n++){
    data[n]-=scalar;
  } 
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
  for(int n=0; n<size; n++){
    destination[n]=data[n]*operand2[n];
  }

#endif /* ARM_CORTEX */
}

void ShortArray::multiply(ShortArray operand2){ //in-place
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  multiply(operand2, *this);
}

void ShortArray::multiply(int16_t scalar){
  for(int n=0; n<size; n++){
   data[n]*=scalar;
  } 
}

void ShortArray::negate(ShortArray& destination){//allows in-place
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_negate_q15(data, destination.getData(), size); 
#else
  for(int n=0; n<size; n++){
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
  for(int n=0; n<size; n++){
    data[n]=(rand()/((float)RAND_MAX)) * amplitude + offset;
  }
}


void ShortArray::convolve(ShortArray operand2, ShortArray destination){
  ASSERT(destination.size >= size + operand2.size -1, "Destination array too small");
/// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
#ifdef ARM_CORTEX
  arm_conv_q15(data, size, operand2.data, operand2.size, destination);
#else
  int size2=operand2.getSize();
  for (int n=0; n<size+size2-1; n++){
    int n1=n;
    destination[n] =0;
    for(int k=0; k<size2; k++){
      if(n1>=0 && n1<size)
        destination[n]+=data[n1]*operand2[k];
      n1--;
    }
  }
#endif /* ARM_CORTEX */
}

void ShortArray::convolve(ShortArray operand2, ShortArray destination, int offset, int samples){
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
  int size2=operand2.getSize();
  for (int n=offset; n<offset+samples; n++){
    int n1=n;
    destination[n] =0; //this should be [n-offset]
    for(int k=0; k<size2; k++){
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
#warning TODO: saturated shift
    if(shiftValue > 0)
      for(int n=0; n<size; n++)
	data[n] <<= shiftValue;
    else
      for(int n=0; n<size; n++)
	data[n] >>= -shiftValue;
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
