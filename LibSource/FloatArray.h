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
  void clear(){
    setAll(0);
  }
  void getMin(float* value, int* index);
  void getMax(float* value, int* index);
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
  void correlate(FloatArray other, FloatArray destination);
  void setAll(float value);
  FloatArray subarray(int offset, int length);
  void copyTo(FloatArray destination);
  void copyFrom(FloatArray source);
  void copyTo(float* destination, int length);
  void copyFrom(float* source, int length);
  void insert(FloatArray source, int offset, int length);
  void move(int fromIndex, int toIndex, int length);
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
