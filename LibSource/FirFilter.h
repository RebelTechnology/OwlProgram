#ifndef __FirFilter_h__
#define __FirFilter_h__

#include "FloatArray.h"

class FirFilter {
private:
  FloatArray coefficients;
  FloatArray states;
  size_t blockSize;
#ifdef ARM_CORTEX
  arm_fir_instance_f32 instance;
#else
  size_t pointer = 0;
#endif /* ARM_CORTEX */

  void processBlock(float* source, float* destination, int size){
#ifdef ARM_CORTEX
    arm_fir_f32(&instance, source, destination, size);
#else
    for(int n = 0; n < size; n++){
      states[pointer] = source[n];
      int tempPointer = pointer;
      float y = 0;
      for(int k = 0; k < coefficients.getSize(); k++){
        y += coefficients[k] * states[tempPointer];
        tempPointer = (tempPointer == states.getSize()) ? 0 : tempPointer+1;
      }
      destination[n] = y;
      //  destination[n] = 0;
      pointer = (pointer == states.getSize()) ? 0 : pointer+1;
    }
#endif /* ARM_CORTEX */
  }
  
public:
  FirFilter() {}
  
  FirFilter(FloatArray cfs, FloatArray ste, size_t bsize)
    : coefficients(cfs), states(ste), blockSize(bsize) {
#ifdef ARM_CORTEX
    arm_fir_init_f32(&instance, coefficients.getSize(), coefficients.getData(), states.getData(), blockSize);
#else
    pointer = 0;
#endif /* ARM_CORTEX */
  }
  
  ~FirFilter(){
  }
  
  void processBlock(FloatArray buffer){
    ASSERT(buffer.getSize()<=blockSize, "Too large"); //TODO: check that in-place actually works properly
    processBlock(buffer.getData(), buffer.getData(), buffer.getSize());
  }
  
  void processBlock(FloatArray source, FloatArray destination){
    ASSERT(source.getSize()<=blockSize, "Too large");
    ASSERT(source.getSize()==destination.getSize(), "Sizes don't match");
    processBlock(source.getData(), destination.getData(), destination.getSize());
  }
  
  FloatArray getCoefficients(){
    return coefficients;
  };
  
  /**
    Copies coefficients value from an array.
  */
  void setCoefficients(FloatArray newCoefficients){
    ASSERT(coefficients.getSize()==newCoefficients.getSize(), "wrong size");
    coefficients.copyFrom(newCoefficients);
  }
  
  static FirFilter* create(size_t taps, size_t blocksize){
    FloatArray coefficients = FloatArray::create(taps);
    FloatArray states = FloatArray::create(taps + blocksize - 1);
    states.clear();
    return new FirFilter(coefficients, states, blocksize);
  }

  static void destroy(FirFilter* filter){
    FloatArray::destroy(filter->coefficients);
    FloatArray::destroy(filter->states);
    delete filter;
  }
};
#endif // __FirFilter_h__
