#ifndef __FloatArray_h__
#define __FloatArray_h__

#include <cstddef>

class FloatArray {
private:
  float* data;
  int size;
public:
 FloatArray() :
   data(NULL), size(0) {}
 FloatArray(float* d, int s) :
   data(d), size(s) {}
  int getSize(){
    return size;
  }
  void getMin(float* value, long unsigned int* index);
  void getMax(float* value, long unsigned int* index);
  float getMinValue();
  float getMaxValue();
  int getMinIndex();
  int getMaxIndex();
  float getDb();
  void rectify(FloatArray& destination);
  void rectify(); //in place
  void reverse(FloatArray& destination);
  void reverse(); //in place
  float getRms();
  float getMean();
  float getPower();
  float getStandardDeviation();
  float getVariance();
  void negate();
  void scale(float factor);
  void add(FloatArray other);
  void subtract(FloatArray other);
  void multiply(FloatArray other);
  void convolve(FloatArray other, FloatArray destination);
  void convolve(FloatArray other, FloatArray destination, int offset, int samples);
  void correlation(FloatArray other, FloatArray destination);
  void setAll(float value);
  FloatArray subarray(int offset, int length);
  void copyTo(FloatArray other);
  void copyFrom(FloatArray other);
  float& operator [](const int index){
    return data[index];
  }
  operator float*(){
    return data;
  }
  float* getData(){
    return data;
  }
  static FloatArray create(int size);
  static void destroy(FloatArray array);
};

#endif // __FloatArray_h__
