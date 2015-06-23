#ifndef __FloatArray_h__
#define __FloatArray_h__

#include <cstddef>

class FloatArray {
private:
  float* data;
  int sz;
public:
 FloatArray() :
   data(NULL), sz(0) {}
 FloatArray(float* d, int s) :
   data(d), sz(s) {}
  int getSize(){
    return sz;
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
  operator float*() {
    return data;
  }
  static FloatArray createFloatArray(int size);
};

#endif // __FloatArray_h__
