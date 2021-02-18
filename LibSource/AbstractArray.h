#ifndef __AbstractArray_h__
#define __AbstractArray_h__

#include <cstddef>

template<typename T>
class AbstractArray {
protected:
  T* data;
  size_t size;
public:
  AbstractArray() : data(NULL), size(0){}
  AbstractArray(T* data, size_t size) : data(data), size(size){}
  virtual ~AbstractArray(){}
  
  size_t getSize() const{
    return size;
  }

  size_t getSize(){
    return size;
  }

  /**
   * Clear the array.
   * Set all the values in the array to 0.
  */
  void clear(){
    setAll(0);
  }
  
  /**
   * Get the minimum value in the array and its index
   * @param[out] value will be set to the minimum value after the call
   * @param[out] index will be set to the index of the minimum value after the call
   * 
   */
  void getMin(T* value, int* index);
  
  /**
   * Get the maximum value in the array and its index
   * @param[out] value will be set to the maximum value after the call
   * @param[out] index will be set to the index of the maximum value after the call
  */
  void getMax(T* value, int* index);
  
  /**
   * Get the minimum value in the array
   * @return the minimum value contained in the array
  */
  T getMinValue();
  
  /**
   * Get the maximum value in the array
   * @return the maximum value contained in the array
   */
  T getMaxValue();
  
  /**
   * Get the index of the minimum value in the array
   * @return the mimimum value contained in the array
   */
  int getMinIndex();
  
  /**
   * Get the index of the maximum value in the array
   * @return the maximum value contained in the array
   */
  int getMaxIndex();

  /**
   * Absolute value of the array.
   * Stores the absolute value of the elements in the array into destination.
   * @param[out] destination the destination array.
  */
  void rectify(AbstractArray<T>& destination);
  
  /**
   * Absolute value of the array.
   * Sets each element in the array to its absolute value.
  */
  void rectify(); //in place
  
  /**
   * Reverse the array
   * Copies the elements of the array in reversed order into destination.
   * @param[out] destination the destination array.
  */
  void reverse(AbstractArray<T>& destination);
  
  /**
   * Reverse the array.
   * Reverses the order of the elements in the array.
  */
  void reverse(); //in place
  
  /**
   * Mean of the array.
   * Gets the mean (or average) of the values in the array.
  */
  T getMean();
  
  /**
   * Clips the elements in the array in the range [-**range**, **range**].
   * @param range clipping value.
  */
  void clip(T range);
  
  /**
   * Clips the elements in the array in the range [**min**, **max**].
   * @param min minimum value
   * @param max maximum value
  */
  void clip(T min, T max);
  
  /**
   * Element-wise sum between arrays.
   * Sets each element in **destination** to the sum of the corresponding element of the array and **operand2**
   * @param[in] operand2 second operand for the sum
   * @param[out] destination the destination array
  */
  void add(AbstractArray<T> operand2, AbstractArray<T> destination);
  
  /**
   * Element-wise sum between arrays.
   * Adds each element of **operand2** to the corresponding element in the array.
   * @param operand2 second operand for the sum
  */
  void add(AbstractArray<T> operand2); //in-place
  
  /**
   * Array-scalar sum.
   * Adds **scalar** to the values in the array.
   * @param scalar value to be added to the array
  */
  void add(T scalar);
  
  /**
   * Element-wise difference between arrays.
   * Sets each element in **destination** to the difference between the corresponding element of the array and **operand2**
   * @param[in] operand2 second operand for the subtraction
   * @param[out] destination the destination array
  */
  void subtract(AbstractArray<T> operand2, AbstractArray<T> destination);
  
  
  /**
   * Element-wise difference between arrays.
   * Subtracts from each element of the array the corresponding element in **operand2**.
   * @param[in] operand2 second operand for the subtraction
  */
  void subtract(AbstractArray<T> operand2); //in-place
  
  /**
   * Array-scalar subtraction.
   * Subtracts **scalar** from the values in the array.
   * @param scalar to be subtracted from the array
  */
  void subtract(T scalar);
  
/**
   * Element-wise multiplication between arrays.
   * Sets each element in **destination** to the product of the corresponding element of the array and **operand2**
   * @param[in] operand2 second operand for the product
   * @param[out] destination the destination array
  */
  void multiply(AbstractArray<T> operand2, AbstractArray<T> destination);
  
   /**
   * Element-wise multiplication between arrays.
   * Multiplies each element in the array by the corresponding element in **operand2**.
   * @param operand2 second operand for the sum
  */
  void multiply(AbstractArray<T> operand2); //in-place
  
  /**
   * Array-scalar multiplication.
   * Multiplies the values in the array by **scalar**.
   * @param scalar to be multiplied with the array elements
  */
  void multiply(T scalar);

  /**
   * Set all the values in the array.
   * Sets all the elements of the array to **value**.
   * @param[in] value all the elements are set to this value.
  */
  void setAll(T value);
  
  /**
   * A subset of the array.
   * Returns a array that points to subset of the memory used by the original array.
   * @param[in] offset the first element of the subset.
   * @param[in] length the number of elments in the new AbstractArray.
   * @return the newly created AbstractArray.
   * @remarks no memory is allocated by this method. The memory is still shared with the original array.
   * The memory should not be de-allocated elsewhere (e.g.: by calling AbstractArray::destroy() on the original AbstractArray) 
   * as long as the AbstractArray returned by this method is still in use.
   * @remarks Calling AbstractArray::destroy() on a AbstractArray instance created with this method might cause an exception.
  */
  AbstractArray<T> subArray(int offset, size_t length);
  
  /**
   * Copies the content of the array to another array.
   * @param[out] destination the destination array
  */
  void copyTo(AbstractArray<T> destination);

  /**
   * Copies the content of an array into another array.
   * @param[in] source the source array
  */
  void copyFrom(AbstractArray<T> source);

  /**
   * Copies the content of an array into a subset of the array.
   * Copies **samples** elements from **source** to **destinationOffset** in the current array.
   * @param[in] source the source array
   * @param[in] destinationOffset the offset into the destination array 
   * @param[in] samples the number of samples to copy
   *
  */
  void insert(AbstractArray<T> source, int destinationOffset, size_t samples);

  /**
   * Copies the content of an array into a subset of the array.
   * Copies **samples** elements starting from **sourceOffset** of **source** to **destinationOffset** in the current array.
   * @param[in] source the source array
   * @param[in] sourceOffset the offset into the source array
   * @param[in] destinationOffset the offset into the destination array
   * @param[in] samples the number of samples to copy
  */
  void insert(AbstractArray<T> source, int sourceOffset, int destinationOffset, size_t samples);
  
  /**
   * Copies values within an array.
   * Copies **length** values starting from index **fromIndex** to locations starting with index **toIndex**
   * @param[in] fromIndex the first element to copy
   * @param[in] toIndex the destination of the first element
   * @param[in] length the number of elements to copy
   * @remarks this method uses *memmove()* so that the source memory and the destination memory can overlap. As a consequence it might have slow performances.
  */
  void move(int fromIndex, int toIndex, size_t length);
  
  /**
   * Allows to index the array using array-style brackets.
   * @param index the index of the element
   * @return the value of the **index** element of the array
  */
  T& operator [](size_t index){
    return data[index];
  }
  
  /**
   * Allows to index the array using array-style brackets.
   * **const** version of operator[]
  */
  T& operator [](size_t index) const{
    return data[index];
  }
  
  /**
   * Get a single value stored in the array.
   * @return the value stored at index @param index
  */
  T getElement(int index){
    return data[index];
  }

  /**
   * Set a single value in the array.
  */
  void setElement(int index, T value){
    data[index] = value;
  }

  /**
   * Compares two arrays.
   * Performs an element-wise comparison of the values contained in the arrays.
   * @param other the array to compare against.
   * @return **true** if the arrays have the same size and the value of each of the elements of the one 
   * match the value of the corresponding element of the other, or **false** otherwise.
  */
  bool equals(const AbstractArray<T>& other) const{
    if(size != other.getSize())
      return false;
    for(size_t n=0; n<size; n++){
      if(data[n] != other[n])
        return false;
    }
    return true;
  }
  
  /**
   * Casting operator to T*
   * @return a T* pointer to the data stored in the AbstractArray
  */
  operator T*(){
    return data;
  }
  
  /**
   * Get the data stored in the AbstractArray.
   * @return a T* pointer to the data stored in the AbstractArray
  */
  T* getData(){
    return data;
  }
};


#endif // __AbstractArray_h__
