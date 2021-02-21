#ifndef __IntArray_h__
#define __IntArray_h__

#include <stdint.h>
#include "SimpleArray.h"
#include "FloatArray.h"

class IntArray : public SimpleArray<int32_t> {
public:
  IntArray(){}
  IntArray(int32_t* data, size_t size) :
    SimpleArray(data, size) {}

  void setAll(int32_t value){
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  #ifdef ARM_CORTEX
    arm_fill_q31(value, data, size);
  #else
    for(size_t n=0; n<size; n++){
      data[n]=value;
    }
  #endif /* ARM_CORTEX */
  }

  /**
   * Clear the array.
   * Set all the values in the array to 0.
  */
  void clear(){
    setAll(0);
  }
  
  /**
   * Element-wise sum between arrays.
   * Sets each element in **destination** to the sum of the corresponding element of the array and **operand2**
   * @param[in] operand2 second operand for the sum
   * @param[out] destination the destination array
  */
  void add(IntArray operand2, IntArray destination){ //allows in-place
    //ASSERT(operand2.size >= size &&  destination.size<=size, "Arrays must be matching size");
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  #ifdef ARM_CORTEX
    arm_add_q31(data, operand2.data, destination.data, size);
  #else
    for(size_t n=0; n<size; n++){
      destination[n]=data[n]+operand2[n];
    }
  #endif /* ARM_CORTEX */
}
  
  /**
   * Element-wise sum between arrays.
   * Adds each element of **operand2** to the corresponding element in the array.
   * @param operand2 second operand for the sum
  */
  void add(IntArray operand2){ //in-place
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
    add(operand2, *this);
  } //in-place

  /**
   * Creates a new IntArray.
   * Allocates size*sizeof(int32_t) bytes of memory and returns a IntArray that points to it.
   * @param size the size of the new IntArray.
   * @return a IntArray which **data** point to the newly allocated memory and **size** is initialized to the proper value.
   * @remarks a IntArray created with this method has to be destroyed invoking the IntArray::destroy() method.
  */
  static IntArray create(int size){
    IntArray fa(new int32_t[size], size);
    fa.clear();
    return fa;
  }
  
  /**
   * Destroys a IntArray created with the create() method.
   * @param array the IntArray to be destroyed.
   * @remarks the IntArray object passed as an argument should not be used again after invoking this method.
   * @remarks a IntArray object that has not been created by the IntArray::create() method might cause an exception if passed as an argument to this method.
  */
  static void destroy(IntArray array){
    delete array.data;
  }

  /**
   * Bitshift the array values, saturating.
   *
   * @param shiftValue number of positions to shift. A positive value will shift left, a negative value will shift right.
   */
  void shift(int shiftValue){
#ifdef ARM_CORTEX
    arm_shift_q31(data, shiftValue, data, size);
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
};

#endif // __IntArray_h__
