#include "FloatArray.h"
#include "basicmaths.h"
#include "message.h"

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
  ASSERT(destination.size >= size, "Destination array too small");
  arm_abs_f32( (float*)data, (float*)destination, size);
}

void FloatArray::rectify(){//in place //TODO: rewrite this so that it calls the overloaded method
  rectify(*this);
}

void FloatArray::reverse(FloatArray& destination){ //this is actually "copy data with reverse"
  ASSERT(destination.size >= size, "Destination array too small");
  for(int n=0; n<size; n++)
    destination[n]=data[size-n-1];
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

void FloatArray::destroy(FloatArray array){
  delete array.data;
}

void FloatArray::copyTo(FloatArray other){
  ASSERT(other.size >= size, "Destination array too small");
  arm_copy_f32(data, other.data, size);
  // memcpy(other.data, data, size);
}

void FloatArray::copyFrom(FloatArray other){
  ASSERT(other.size >= size, "Source array too small");
  arm_copy_f32(other.data, data, size);
  // memcpy(data, other.data, size);
}

void FloatArray::setAll(float value){
  arm_fill_f32(value, data, size);
  // for(int i=0; i<size; i++)
  //   data[i] = value;
}

void FloatArray::add(FloatArray other){
  ASSERT(other.size == size, "Arrays must be same size");
  arm_add_f32(data, other.data, data, size);
}

void FloatArray::subtract(FloatArray other){
  ASSERT(other.size == size, "Arrays must be same size");
  arm_sub_f32(data, other.data, data, size);
}

void FloatArray::multiply(FloatArray other){
  ASSERT(other.size == size, "Arrays must be same size");
  arm_mult_f32(data, other.data, data, size);
}

void FloatArray::negate(){
  arm_negate_f32(data, data, size);
}

/**
 * Perform the convolution of this FloatArray with @other, putting the result in @destination.
 * @destination must have a minimum size of this+other-1.
 */
void FloatArray::convolve(FloatArray other, FloatArray destination){
  ASSERT(destination.size >= size + other.size -1, "Destination array too small");
  arm_conv_f32(data, size, other.data, other.size, destination);
}

/**
 * Perform partial convolution: start at @offset and compute @samples points
 */
void FloatArray::convolve(FloatArray other, FloatArray destination, int offset, int samples){
  ASSERT(destination.size >= samples, "Destination array too small");
  arm_conv_partial_f32(data, size, other.data, other.size, destination, offset, samples);
}

/*
 * Length 2 * max(srcALen, srcBLen) - 1.
 */
void FloatArray::correlation(FloatArray other, FloatArray destination){
  ASSERT(destination.size >= 2 * max(size, other.size)-1, "Destination array too small");
  arm_correlate_f32(data, size, other.data, other.size, destination);
}
