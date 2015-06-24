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
  void scale(float factor);
  FloatArray subarray(int offset, int length);
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
};

#endif // __FloatArray_h__
