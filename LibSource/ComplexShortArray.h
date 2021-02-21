#ifndef __ComplexShortArray_h__
#define __ComplexShortArray_h__

#include "ShortArray.h"

/**
* A structure defining a fixed point complex number as two members of type int16_t.
*/
struct ComplexShort {
  /**
  The real part of the complex number.
  */
  int16_t re;
  
  /**
  The imaginary part of the complex number.
  */
  int16_t im;
  
  /**
  Get the magnitude of the complex number.
  Computes and returns the magnitude of the complex number.
  @return The magnitude of the complex number.
  */
  int16_t getMagnitude();
  
  /**
  Get the phase of the complex number.
  Computes and returns the phase of the complex number.
  @return The phase of the complex number.
  */  
  int16_t getPhase();
  
  /**
  Set the phase of the complex number.
  Set the phase of the complex number, keeping the magnitude unaltered.
  @param phase The new phase of the complex number
  */
  void setPhase(int16_t phase){
    int16_t magnitude = getMagnitude();
    setPolar(magnitude, phase);
  }
  
  /**
  Set the magnitude of the complex number.
  Set the magnitude of the complex number, keeping the phase unaltered.
  @param magnitude The new magnitude of the complex number
  */
  void setMagnitude(int16_t magnitude){
    int16_t phase = getPhase();
    setPolar(magnitude, phase);
  }
  
  /**
  Set magnitude and phase of the complex number.
  @param magnitude The new magnitude of the complex number
  @param phase The new phase of the complex number
  */
  void setPolar(int16_t magnitude, int16_t phase);

  bool operator==(const ComplexShort& other) const {
    return re == other.re && im == other.im;
  }
  
  bool operator!=(const ComplexShort& other) const {
    return re != other.re || im != other.im;
  }
};

class ComplexShortArray : public SimpleArray<ComplexShort> {
public:
  ComplexShortArray(){}
  ComplexShortArray(ComplexShort* data, size_t size) :
    SimpleArray(data, size) {}

  /** 
    The real part of an element of the array.
      
    @param i The index of the element
    @return The real part of the element
  */
  int16_t re(const int i){
    return data[i].re;
  }
  
  /**
    The imaginary part of an element of the array.
    @param i The index of the element
    @return The imaginary part of the element
  */
  int16_t im(const int i){
    return data[i].im;
  }
  
  void clear(){
    setAll(0);
  }

  /**
    The magnitude of an element of the array.
    @param i The index of the element
    @return The magnitude of the element
  */  
  int16_t mag(const int i);
  
  /**
    The magnitudes of the elements of the array.
    @param[out] destination The array where the magnitude values will be stored.
  */  
  void getMagnitudeValues(ShortArray destination);
  
  /**
    The magnitude squared of an element of the array.
    @param i The index of the element
    @return The magnitude squared of the element
  */  
  int16_t mag2(const int i);
  
  /**
    The squared magnitudes of the elements of the array.
    @param[out] destination The array where the magnitude squared values will be stored.
  */  
  void getMagnitudeSquaredValues(ShortArray destination);
  
   /**
    The complex conjugate values of the element of the array.
    @param[out] destination The array where the complex conjugate values will be stored.
  */  
  void getComplexConjugateValues(ComplexShortArray destination);
  
   /**
    Complex dot product between arrays.
    @param[in] operand2 The second operand of the dot product
    @param[out] result The array where the result of the dot product is stored 
  */  
  void complexDotProduct(ComplexShortArray operand2, ComplexShort& result);

   /**
    Complex by complex multiplication between arrays.
    @param[in] operand2 The second operand of the multiplication
    @param[out] result The array where the result of the multiplication is stored 
  */  
  void complexByComplexMultiplication(ComplexShortArray operand2, ComplexShortArray result);
  
   /**
    Complex by real multiplication between arrays.
    @param[in] operand2 The second operand of the multiplication
    @param[out] result The array where the result of the multiplication is stored 
  */ 
  void complexByRealMultiplication(ShortArray operand2, ComplexShortArray result);

  /**
   * Element-wise sum between complex arrays.
   * Sets each element in **destination** to the complex sum of the corresponding element of the array and **operand2**
   * @param[in] operand2 second operand for the sum
   * @param[out] destination the destination array
  */
  void add(ComplexShortArray operand2, ComplexShortArray destination);

  /**
   * In-place element-wise sum between complex arrays.
   * Adds each element of **operand2** to the corresponding element in the array.
   * @param operand2 second operand for the sum
  */
  void add(ComplexShortArray operand2);

  /**
   * Element-wise difference between complex arrays.
   * Sets each element in **destination** to the complex difference between the corresponding element of the array and **operand2**
   * @param[in] operand2 second operand for the subtraction
   * @param[out] destination the destination array
  */
  void subtract(ComplexShortArray operand2, ComplexShortArray destination);

  /**
   * In-place element-wise difference between complex arrays.
   * Sutracts each element of **operand2** from the corresponding element in the array.
   * @param operand2 second operand for the sum
  */
  void subtract(ComplexShortArray operand2);
  
  /**
    The value of the element with the maximum magnitude in the array.
    @return The maximum magnitude value in the array.
  */ 
  int16_t getMaxMagnitudeValue();
  
  /**
    The index of the element with the maximum magnitude in the array.
    @return The index of the element with the maximum magnitude in the array.
  */ 
  unsigned int getMaxMagnitudeIndex();
  
  /**
   * A subset of the array.
   * Returns an array that points to subset of the memory used by the original array.
   * @param[in] offset the first element of the subset.
   * @param[in] length the number of elments in the new ComplexShortArray.
   * @return the newly created ComplexShortArray.
   * @remarks no memory is allocated by this method. The memory is still shared with the original array.
   * The memory should not be de-allocated elsewhere (e.g.: by calling ComplexShortArray::destroy() on the original ComplexShortArray) 
   * as long as the ComplexShortArray returned by this method is still in use.
   * @remarks Calling ComplexShortArray::destroy() on a ComplexShortArray instance created with this method might cause an exception.
  */
  ComplexShortArray subArray(unsigned int offset, unsigned int length);
  
  /** Get the real part of the elements of the array.
   * @param[out] buf The array where the real part will be stored.
   */   
  void getRealValues(ShortArray buf);
  
  /** Get the imaginary part of the elements of the array.
   * @param[out] buf The array where the imaginary part will be stored.
  */   
  void getImaginaryValues(ShortArray buf);
  
  /** Array by scalar multiplication.
   * @param factor The value by which all the elements of the array are multiplied.
   */
  void scale(int16_t factor);
  
  /**
   * Creates a new ComplexShortArray.
   * Allocates size*sizeof(int16_t) bytes of memory and returns a ComplexShortArray that points to it.
   * @param size The size of the new ComplexShortArray.
   * @return A ComplexShortArray which **data** point to the newly allocated memory and <code>size</code> is initialized to the proper value.
   * @remarks A ComplexShortArray created with this method has to be destroyed invoking the ComplexShortArray::destroy() method.
  */
  static ComplexShortArray create(unsigned int size);

  /**
   * Destroys a ComplexShortArray created with the create() method.
   * @param array The ComplexShortArray to be destroyed.
   * @remarks The ComplexShortArray object passed as an argument should not be used again after invoking this method.
   * @remarks A ComplexShortArray object that has not been created by the ComplexShortArray::create() method might cause an exception if passed as an argument to this method.
  */
  static void destroy(ComplexShortArray);

  /**
   * Copies the content of an array into another array.
   * @param[in] source The source array
  */
  void copyFrom(ShortArray source);

  /**
   * Copies the content of a ShortArray into the ComplexShortArray.
   * @param[in] source The source array
   * @remarks The size of the ShortArray must be twice the size of the ComplexShortArray.
  */
  void copyFrom(ComplexShortArray source);
  
  /**
   * Copies an array of ComplexShort into the array.
   * @param[in] source A pointer to the beginning of the portion of memory to read from.
   * @param[in] length Number of samples to copy.
  */
  void copyFrom(ComplexShort* source, unsigned int length);
  
  /**
   * Copies the content of the ComplexShortArray into a ShortArray.
   * @param[out] destination The destination array
   * @remarks The size of the ShortArray must be twice the size of the ComplexShortArray.
  */
  void copyTo(ShortArray destination);

  /**
   * Copies the content of the array to another array.
   * @param[out] destination The destination array
  */
  void copyTo(ComplexShortArray destination);
  
  /**
   * Copies an array of ComplexShort into the array.
   * @param[in] destination A pointer to the beginning of the portion of memory to write to.
   * @param[in] length Number of samples to copy.
  */
  void copyTo(ComplexShort* destination, int length);


  /**
   * Set all the elements in the array.
   * @param[in] value All the elements are set to this value.
  */
  void setAll(ComplexShort value);
  
  /**
   * Set all the elements in the array.
   * @param[in] value The real and imaginary parts of all the elements in the array are set to this value.
  */
  void setAll(int16_t value);
  
  /**
   * Set all the elements in the array.
   * @param[in] valueRe The real part of every element of the the array will be set to this value.
   * @param[in] valueIm The imaginary pary of rvery element of the the array will be set to this value.
  */
  void setAll(int16_t valueRe, int16_t valueIm);

  /**
   * Set all the elements in the array using polar coordinates.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] phase An array containing the phases.
  */
  void setPolar(ShortArray magnitude, ShortArray phase);
  
  /**
   * Set a range of elements in the array using polar coordinates.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] phase An array containing the phases.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
  */  
  void setPolar(ShortArray magnitude, ShortArray phase, int offset, int count);

  /**
   * Set the phase of the elements of the array, leaving the magnitude unchanged.
   * @param[in] phase An array containing the phases.
  */
  void setPhase(ShortArray phase);

  /**
   * Set the phase of a range of elements of the array, leaving the magnitude unchanged.
   * @param[in] phase An array containing the phases.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
  */
  void setPhase(ShortArray phase, int offset, int count);
  
  /**
   * Set the phase of the elements of an array, using the magnitude from the current array.
   * @param[in] phase An array containing the phases.
   * @param[out] destination The destination array.
  */
  void setPhase(ShortArray phase, ComplexShortArray destination);
  
  /**
   * Set the phase of a range of the elements of an array, using the magnitude from the current array.
   * 
   * The <code>offset</code> and <code>count</code> specified apply to both the source and <code>destination</code> arrays. 
   * Elements of the <code>destination</code> array outside this range will not be affected.
   * @param[in] phase An array containing the phases.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
   * @param[out] destination The destination array.
  */
  void setPhase(ShortArray phase, int offset, int count, ComplexShortArray destination);


  /**
   * Set the magnitude of the elements of the array, leaving the phase unchanged.
   * @param[in] magnitude An array containing the magnitudes.
  */
  void setMagnitude(ShortArray magnitude);

/**
   * Set the magnitude of a range of elements of the array, leaving the phase unchanged.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
  */
  void setMagnitude(ShortArray magnitude, int offset, int count);
  
  /**
   * Set the magnitude of the elements of an array, using the phase from the current array.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[out] destination The destination array.
  */
  void setMagnitude(ShortArray magnitude, ComplexShortArray destination);
  
  /**
   * Set the magnitude of a range of the elements of an array, using the phases from the current array.
   * 
   * The <code>offset</code> and <code>count</code> specified apply to both the source and <code>destination</code> arrays. 
   * Elements of the <code>destination</code> array outside this range will not be affected.
   * @param[in] magnitude An array containing the magnitudes.
   * @param[in] offset First element to set
   * @param[in] count Number of elements to set
   * @param[out] destination The destination array.
  */
  void setMagnitude(ShortArray magnitude, int offset, int count, ComplexShortArray destination);
};
#endif // __ComplexShortArray_h__
