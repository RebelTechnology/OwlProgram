#ifndef __IntArray_h__
#define __IntArray_h__

#include <stdint.h>
#include "AbstractArray.h"
#include "FloatArray.h"

#ifdef USE_TEMPLATE
class IntArray : public AbstractArray<int32_t> {
public:
  IntArray(){}
  IntArray(int32_t* data, size_t size) :
    AbstractArray(data, size) {}

#else
class IntArray {
private:
  int32_t* data;
  size_t size;
public:
  IntArray() :
    data(NULL), size(0) {}
  IntArray(int32_t* data, size_t size) :
    data(data), size(size) {}

  size_t getSize() const{
    return size;
  }

  size_t getSize(){
    return size;
  }

  /**
   * Casting operator to int32_t*
   * @return a int32_t* pointer to the data stored in the IntArray
   */
  operator int32_t*(){
    return data;
  }

  /**
   * Get the data stored in the IntArray.
   * @return a int32_t* pointer to the data stored in the IntArray
   */
  int32_t* getData(){
    return data;
  }

  /**
   * Get a single int32_t stored in the IntArray.
   * @return the int32_t stored at index @param index
   */
  int32_t getElement(int index){
    return data[index];
  }

  /**
   * Set a single int32_t in the IntArray.
   */
  void setElement(int index, int32_t value){
    data[index] = value;
  }
  
  /**
   * Allows to index the array using array-style brackets.
   * @param index the index of the element
   * @return the value of the **index** element of the array
   * Example usage:
   * @code
   * int size=1000;
   * int32_t content[size]; 
   * IntArray int32_tArray(content, size);
   * for(int n=0; n<size; n++)
   *   content[n]==int32_tArray[n]; //now the IntArray can be indexed as if it was an array
   * @endcode
  */
  int32_t& operator [](const int index){
    return data[index];
  }
  
  /**
   * Allows to index the array using array-style brackets.
   * **const** version of operator[]
  */
  const int32_t& operator [](const int index) const {
    return data[index];
  }
  
  /**
   * Compares two arrays.
   * Performs an element-wise comparison of the values contained in the arrays.
   * @param other the array to compare against.
   * @return **true** if the arrays have the same size and the value of each of the elements of the one 
   * match the value of the corresponding element of the other, or **false** otherwise.
  */
  bool equals(const IntArray& other) const{
    if(size!=other.getSize()){
      return false;
    }
    for(size_t n=0; n<size; n++){
      if(data[n]!=other[n]){
        return false;
      }
    }
    return true;
  }
#endif

  void setAll(int32_t value){
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  #ifdef ARM_CORTEX
    arm_fill_q31(value, data, size);
  #else
    for(int n=0; n<size; n++){
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
    for(int n=0; n<size; n++){
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
    #warning TODO
    
    //ASSERT(false, "TODO");
#endif
  }
};

#endif // __IntArray_h__
