#include "FloatMatrix.h"
#include "basicmaths.h"

#ifdef ARM_CORTEX
FloatMatrix::FloatMatrix(){
  instance.numRows = 0;
  instance.numCols = 0;
  instance.pData = NULL;
}
FloatMatrix::FloatMatrix(float* dt, size_t rs, size_t cs){
  arm_mat_init_f32(&instance, rs, cs, dt);
}
#else
FloatMatrix::FloatMatrix()
  : data(NULL), rows(0), columns(0) {}
FloatMatrix::FloatMatrix(float* dt, size_t rs, size_t cs)
  : data(dt), rows(rs), columns(cs) {}
#endif

void FloatMatrix::add(FloatMatrix operand2, FloatMatrix destination){
#ifdef ARM_CORTEX
  arm_mat_add_f32(&instance, &operand2.instance, &destination.instance);
#else
#warning todo
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
#warning todo
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

 FloatMatrix FloatMatrix::create(size_t m, size_t n){
  return FloatMatrix(new float[m*n], m, n);
}
  
 void FloatMatrix::destroy(FloatMatrix array){
#ifdef ARM_CORTEX
  delete array.instance.pData;
#else
  delete array.data;
#endif
};

