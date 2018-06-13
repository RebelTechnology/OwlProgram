#ifndef __FloatMatrix_h__
#define __FloatMatrix_h__

#include <cstddef>
#include "basicmaths.h"

/**
 * This class contains useful methods for manipulating NxN dimensioned matrices of floats.
 */
class FloatMatrix {
private:
#ifdef ARM_CORTEX
  arm_matrix_instance_f32 instance;
#else
  float* data;
  size_t rows;
  size_t columns;
#endif
public:
  FloatMatrix();
  FloatMatrix(float* data, size_t rows, size_t columns);

  /** Get the number of elements in this matrix */
  int getSize() const{
    return getRows()*getColumns();
  }

  size_t getRows() const{
#ifdef ARM_CORTEX
    return instance.numRows;
#else
    return rows;
#endif
  }

  size_t getColumns() const{
#ifdef ARM_CORTEX
    return instance.numCols;
#else
    return columns;
#endif
  }

  /**
   * Clear the array.
   * Set all the values in the array to 0.
  */
  void clear(){
    setAll(0);
  }  
  
  /**
   * Element-wise sum between matrices.
   * @param[in] operand2 second operand for the sum
   * @param[out] destination the destination array
  */
  void add(FloatMatrix operand2, FloatMatrix destination);
  
  /**
   * Element-wise sum between arrays.
   * Adds each element of **operand2** to the corresponding element in the array.
   * @param operand2 second operand for the sum
  */
  void add(FloatMatrix operand2); //in-place

  /* /\** */
  /*  * Element-wise sum between matrix and array. */
  /*  * Sets each element in **destination** to the sum of the corresponding element of the matrix and **operand2** */
  /*  * @param[in] operand2 second operand for the sum */
  /*  * @param[out] destination the destination array */
  /* *\/ */
  /* void add(FloatArray operand2, FloatMatrix destination); */
  
  /* /\** */
  /*  * Element-wise sum between matrix and array. */
  /*  * Adds each element of **operand2** to the corresponding element of the matrix. */
  /*  * @param operand2 second operand for the sum */
  /* *\/ */
  /* void add(FloatMatrix operand2); //in-place */

  /**
   * Matrix-scalar sum.
   * Adds **scalar** to the values in the array.
   * @param scalar value to be added to the array
  */
  void add(float scalar);
    
/**
   * Element-wise multiplication between arrays.
   * Sets each element in **destination** to the product of the corresponding element of the array and **operand2**
   * Multiplying an M x N matrix with an N x P matrix results in an M x P matrix.
   * @param[in] operand2 second operand for the product
   * @param[out] destination the destination array
  */
  void multiply(FloatMatrix operand2, FloatMatrix destination);
  
   /**
   * Element-wise multiplication between arrays.
   * Multiplies each element in the array by the corresponding element in **operand2**.
   * @param operand2 second operand for the sum
  */
  void multiply(FloatMatrix operand2); //in-place
  
  /**
   * Matrix scalar multiplication.
   * Multiplies each value in the matrix by **scalar**.
   * @param scalar to be subtracted from the array
  */
  void multiply(float scalar);

  /**
   * Set all the values in the array.
   * Sets all the elements of the array to **value**.
   * @param[in] value all the elements are set to this value.
  */
  void setAll(float value);  
  
  /**
   * Casting operator to float*
   * @return a float* pointer to the data stored in the FloatMatrix
  */
  operator float*(){
    return getData();
  }
  
  /**
   * Get the data stored in the FloatMatrix.
   * @return a float* pointer to the data stored in the FloatMatrix
  */
  float* getData(){
#ifdef ARM_CORTEX
    return instance.pData;
#else
    return data;
#endif
  }
  
  /**
   * Creates a new FloatMatrix.
   * Allocates rows*columns*sizeof(float) bytes of memory and returns a FloatMatrix that points to it.
   * @param rows the number of rows of the new FloatMatrix.
   * @param columns the number of columns of the new FloatMatrix.
   * @return a FloatMatrix which **data** point to the newly allocated memory and **rows** and **columns** initialized to the proper values.
   * @remarks a FloatMatrix created with this method should be destroyed invoking the FloatMatrix::destroy() method.
  */
  static FloatMatrix create(size_t rows, size_t columns);
  
  /**
   * Destroys a FloatMatrix created with the create() method.
   * @param array the FloatMatrix to be destroyed.
   * @remarks the FloatMatrix object passed as an argument should not be used again after invoking this method.
   * @remarks a FloatMatrix object that has not been created by the FloatMatrix::create() method might cause an exception if passed as an argument to this method.
  */
  static void destroy(FloatMatrix array);
};

#endif // __FloatMatrix_h__
