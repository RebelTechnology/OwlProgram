#include "FloatMatrix.h"
#include "basicmaths.h"
#include "message.h"

#ifdef ARM_CORTEX
FloatMatrix::FloatMatrix(){
  instance.numRows = 0;
  instance.numCols = 0;
  instance.pData = NULL;
}
FloatMatrix::FloatMatrix(float* dt, size_t rs, size_t cs){
  arm_mat_init_f32(&instance, rs, cs, dt);
}
FloatMatrix::FloatMatrix(const float* dt, size_t rs, size_t cs){
  arm_mat_init_f32(&instance, rs, cs, (float*)dt);
}
#else
FloatMatrix::FloatMatrix()
  : data(NULL), rows(0), columns(0) {}
FloatMatrix::FloatMatrix(float* dt, size_t rs, size_t cs)
  : data(dt), rows(rs), columns(cs) {}
FloatMatrix::FloatMatrix(const float* dt, size_t rs, size_t cs)
  : data((float*)dt), rows(rs), columns(cs) {}
#endif

void FloatMatrix::add(FloatMatrix operand2, FloatMatrix destination){
#ifdef ARM_CORTEX
  arm_mat_add_f32(&instance, &operand2.instance, &destination.instance);
#else
  // ASSERT(getColumns() == operand2.getColumns(), "Incompatible column size");
  // ASSERT(getRows() == operand2.getRows(), "Incompatible row size");
  // for(size_t i=0; i<getRows(); i++)
  //   for(size_t j=0; j<operand2.getColumns(); j++)
  //     destination.setElement(i, j, getElement(i, j)+operand2.getElement(i, j));
  // element wise addition
  ASSERT(getSize() == operand2.getSize(), "Incompatible matrix sizes");
  for(size_t i=0; i<getSize(); i++)
    destination.data[i] = data[i] + operand2.data[i];;
#endif
}
  
void FloatMatrix::add(FloatMatrix operand2){
  add(operand2, *this);
}
  
void FloatMatrix::add(float scalar){
  size_t size = getSize();
  float* data = getData();
  for(size_t i=0; i<size; i++)
      data[i] += scalar;
}

void FloatMatrix::multiply(FloatMatrix operand2, FloatMatrix destination){
#ifdef ARM_CORTEX
  arm_mat_mult_f32(&instance, &operand2.instance, &destination.instance);
#else
  ASSERT(getColumns() == operand2.getRows(), "Incompatible matrix sizes");
  ASSERT(destination.rows >= rows, "Insufficient rows in destination");
  ASSERT(destination.columns >= operand2.columns, "Insufficient columns in destination");

  // FloatMatrix operand1 = *this;
  // for(size_t i=0; i<rows; i++){
  //   for(size_t j=0; j<operand2.columns; j++){
  //     destination[i][j] = 0;
  //     for(size_t k=0; k<operand2.rows; k++){
  // 	destination[i][j] += operand1[i][k] * operand2[k][j];
  //     }
  //   }
  // }
  for(size_t i=0; i<getRows(); i++){
    for(size_t j=0; j<operand2.getColumns(); j++){
      destination.setElement(i, j, 0);
      for(size_t k=0; k<operand2.getRows(); k++){
	destination.setElement(i, j, destination.getElement(i, j) + getElement(i, k) * operand2.getElement(k, j));
      }
    }
  }
#endif
}
  
void FloatMatrix::multiply(FloatMatrix operand2){
  multiply(operand2, *this);
}
  
void FloatMatrix::multiply(float scalar){
  size_t size = getSize();
  float* data = getData();
  for(size_t i=0; i<size; i++)
      data[i] *= scalar;
}

void FloatMatrix::setAll(float value){
  size_t size = getSize();
  float* data = getData();
  for(size_t i=0; i<size; i++)
    data[i] = value;
}

void FloatMatrix::softmax(FloatMatrix destination){
  ASSERT(destination.getSize() >= getSize(), "Destination size too small");
  float* src = getData();
  float* dest = destination.getData();
  size_t size = getSize();
  float m = -INFINITY;
  for(size_t i = 0; i < size; i++) {
      if(src[i] > m)
	m = src[i];
  }
  float sum = 0.0;
  for(size_t i = 0; i < size; i++)
    sum += expf(src[i] - m);
  float offset = m + logf(sum);
  for(size_t i = 0; i < size; i++)
    dest[i] = expf(src[i] - offset);
}


void FloatMatrix::sigmoid(FloatMatrix destination){
  ASSERT(destination.getSize() >= getSize(), "Destination size too small");
  float* src = getData();
  float* dest = destination.getData();
  size_t size = getSize();
  for(size_t i = 0; i < size; i++)
    dest[i] = 1.0f / (1 + expf(-src[i]));
}

FloatMatrix FloatMatrix::create(size_t m, size_t n){
  FloatMatrix fm(new float[m*n], m, n);
  fm.clear();
  return fm;
}
  
 void FloatMatrix::destroy(FloatMatrix array){
#ifdef ARM_CORTEX
  delete[] array.instance.pData;
#else
  delete[] array.data;
#endif
};

