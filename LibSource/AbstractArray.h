#ifndef __AbstractArray_h__
#define __AbstractArray_h__

#include <cstddef>

/**
 * AbstractArray holds a pointer to an array and the array size, and is designed to 
 * be passed by value. It does not define any virtual methods to prevent overheads 
 * in its subclasses.
 */
template<typename T>
class AbstractArray {
protected:
  T* data;
  size_t size;
public:
  AbstractArray() : data(NULL), size(0){}
  AbstractArray(T* data, size_t size) : data(data), size(size){}
  /* virtual ~AbstractArray(){} No virtual destructor to prevent adding a vtable to the object size */
  
  /**
   * Get the data stored in the Array.
   * @return a T* pointer to the data stored in the Array
  */
  T* getData(){
    return data;
  }
    
  size_t getSize() const {
    return size;
  }
    
  bool isEmpty() const {
    return size == 0;
  }

  /**
   * Get a single value stored in the array.
   * @return the value stored at index @param index
  */
  T getElement(size_t index){
    return data[index];
  }

  /**
   * Set a single value in the array.
  */
  void setElement(size_t index, T value){
    data[index] = value;
  }
    
  /**
   * Compares two arrays.
   * Performs an element-wise comparison of the values contained in the arrays.
   * @param other the array to compare against.
   * @return **true** if the arrays have the same size and the value of each of the elements of the one 
   * match the value of the corresponding element of the other, or **false** otherwise.
  */
  bool equals(const AbstractArray<T>& other) const {
    if(size != other.size)
      return false;
    for(size_t n=0; n<size; n++){
      if(data[n] != other.data[n])
        return false;
    }
    return true;
  }

  /**
   * Casting operator to T*
   * @return a T* pointer to the data stored in the Array
  */
  operator T*(){
    return data;
  }
    
  // /**
  //  * Allows to index the array using array-style brackets.
  //  * @param index the index of the element
  //  * @return the value of the **index** element of the array
  // */
  // T& operator [](size_t index){
  //   return data[index];
  // }
  
  // /**
  //  * Allows to index the array using array-style brackets.
  //  * **const** version of operator[]
  // */
  // const T& operator [](size_t index) const {
  //   return data[index];
  // }

};


#endif // __AbstractArray_h__
