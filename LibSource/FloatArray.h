#ifndef __FloatArray_h__
#define __FloatArray_h__

#include <cstddef>

/**
 * This class contains useful methods for manipulating arrays of floats.
 * It also provides a convenient handle to the array pointer and the size of the array.
 * FloatArray objects can be passed by value without copying the contents of the array.
 */
class FloatArray {
private:
  float* data;
  int size;
public:
 FloatArray() :
   data(NULL), size(0) {}
 FloatArray(float* d, int s) :
   data(d), size(s) {}
  int getSize() const{
    return size;
  }
  int getSize(){
    return size;
  }

  void clear(){
    setAll(0);
  }
  /**
   * Get the minimum value and its index
   * @param value will be set to the minimum value after the call
   * @param index will be set to the index of the minimum value after the call
   */
  void getMin(float* value, int* index);
  void getMax(float* value, int* index);
  /**
   * Get the minimum value in the array
   * @return the minimum value contained in this array
   */
  float getMinValue();
  float getMaxValue();
  int getMinIndex();
  int getMaxIndex();
  float getDb();
  void rectify(FloatArray& destination);
  void rectify(); //in place
  void reverse(FloatArray& destination);
  void reverse(); //in place
  void reciprocal(FloatArray& destination);
  void reciprocal(); //in place
  void negate(FloatArray& destination);
  void negate(); //in place
  void noise();
  void noise(float min, float max);
  float getRms();
  float getMean();
  float getPower();
  float getStandardDeviation();
  float getVariance();
  void scale(float factor, FloatArray destination);
  void scale(float factor);
  void clip();
  void clip(float max);
  void clip(float max, float newMax);
  void add(FloatArray operand2, FloatArray destination);
  void add(FloatArray operand2); //in-place
  void add(float scalar);
  void subtract(FloatArray operand2, FloatArray destination);
  void subtract(FloatArray operand2); //in-place
  void subtract(float scalar);
  void multiply(FloatArray operand2, FloatArray destination);
  void multiply(FloatArray operand2); //in-place
  void multiply(float scalar);
  void convolve(FloatArray operand2, FloatArray destination);
  void convolve(FloatArray operand2, FloatArray destination, int offset, int samples);
  void correlate(FloatArray operand2, FloatArray destination);
  void correlateInitialized(FloatArray operand2, FloatArray destination);
  void setAll(float value);
  FloatArray subArray(int offset, int length);
  void copyTo(FloatArray destination);
  void copyFrom(FloatArray source);
  void copyTo(float* destination, int length);
  void copyFrom(float* source, int length);
  void insert(FloatArray source, int offset, int samples);
  void insert(FloatArray source, int sourceOffset, int destinationOffset, int samples);
  void move(int fromIndex, int toIndex, int length);
  float& operator [](const int index){
    return data[index];
  }
  float& operator [](const int index) const{
    return data[index];
  }
  bool equals(const FloatArray& other) const{
    if(size!=other.getSize()){
      return false;
    }
    for(int n=0; n<size; n++){
      if(data[n]!=other[n]){
        return false;
      }
    }
    return true;
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
