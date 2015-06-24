#include "FloatArray.h"
#include "basicmaths.h"

void FloatArray::getMin(float* value, long unsigned int* index){
  arm_min_f32((float *)data, size, value, index);
}

float FloatArray::getMinValue(){
  float value;
  long unsigned int index;
  getMin(&value, &index);
  return value;
}

int FloatArray::getMinIndex(){
  float value;
  long unsigned int index;
  getMin(&value, &index);
  return index;
}

void FloatArray::getMax(float* value, long unsigned int* index){
  arm_max_f32((float *)data, size, value, index);
}

float FloatArray::getMaxValue(){
  float value;
  long unsigned int index;
  getMax(&value, &index);
  return value;
}

int FloatArray::getMaxIndex(){
  float value;
  long unsigned int index;
  getMax(&value, &index);
  return index;
}

void FloatArray::rectify(FloatArray& destination){ //this is actually "copy data with rectifify"
  int minSize= min(size,destination.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_abs_f32( (float*)data, (float*)destination, size);
}

void FloatArray::rectify(){//in place //TODO: rewrite this so that it calls the overloaded method
  rectify(*this);
}

void FloatArray::reverse(FloatArray& destination){ //this is actually "copy data with reverse"
  for(int n=0; n<size; n++){
    destination[n]=data[size-n-1];
  }
}

void FloatArray::reverse(){//in place //TODO: rewrite this so that it calls the overloaded method
  reverse(*this);
}

float FloatArray::getRms(){
  float result;
  arm_rms_f32 ((float *)data, size, &result);
  return result;
}

float FloatArray::getMean(){
  float result;
  arm_mean_f32 ((float *)data, size, &result);
  return result;
}

float FloatArray::getPower(){
  float result;
  arm_power_f32 ((float *)data, size, &result);
  return result;
}

float FloatArray::getStandardDeviation(){
  float result;
  arm_std_f32 ((float *)data, size, &result);
  return result;
}

float FloatArray::getVariance(){
  float result;
  arm_var_f32 ((float *)data, size, &result);
  return result;
}

void FloatArray::scale(float factor){
  arm_scale_f32 ( (float*)data, factor, (float*)data, size);
}

FloatArray FloatArray::create(int size){
  return FloatArray(new float[size], size);
}
