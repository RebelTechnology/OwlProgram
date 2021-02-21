#ifndef __FirFilter_h__
#define __FirFilter_h__

#include "FloatArray.h"
#include "SignalProcessor.h"

class FirFilter : public SignalProcessor {
private:
  FloatArray coefficients;
  FloatArray states;
  size_t blockSize;
#ifdef ARM_CORTEX
  arm_fir_instance_f32 instance;
#else
  size_t index = 0;
#endif /* ARM_CORTEX */

  void processBlock(float* source, float* destination, size_t size){
#ifdef ARM_CORTEX
    arm_fir_f32(&instance, source, destination, size);
#else
    for(size_t n = 0; n < size; n++){
      states[index] = source[n];
      size_t tempIndex = index;
      float y = 0;
      for(size_t k = 0; k < coefficients.getSize(); k++){
        y += coefficients[k] * states[tempIndex];
        tempIndex = (tempIndex == states.getSize()) ? 0 : tempIndex+1;
      }
      destination[n] = y;
      //  destination[n] = 0;
      if(++index == states.getSize())
	index = 0;
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
    index = 0;
#endif /* ARM_CORTEX */
  }
  
  ~FirFilter(){
  }

  float process(float input){
    float output;
    processBlock(&input, &output, 1);
    return output;
  }

  void process(FloatArray buffer){
    ASSERT(buffer.getSize()<=blockSize, "Too large"); //TODO: check that in-place actually works properly
    processBlock(buffer.getData(), buffer.getData(), buffer.getSize());
  }

  void process(FloatArray source, FloatArray destination){
    ASSERT(source.getSize()<=blockSize, "Too large");
    ASSERT(source.getSize()==destination.getSize(), "Sizes don't match");
    processBlock(source.getData(), destination.getData(), destination.getSize());
  }

  [[deprecated("use process() instead.")]]  
  void processBlock(FloatArray buffer){
    process(buffer);
  }
  [[deprecated("use process() instead.")]]  
  void processBlock(FloatArray source, FloatArray destination){
    process(source, destination);
  }

  FloatArray getCoefficients(){
    return coefficients;
  };

  /**
   * Sets coefficients to point to a different set of values
   */
  void setCoefficients(FloatArray newCoefficients){
    coefficients = newCoefficients;
  }
  
  /**
   * Copies coefficient values from an array.
   */
  void copyCoefficients(FloatArray newCoefficients){
    ASSERT(coefficients.getSize()==newCoefficients.getSize(), "wrong size");
    coefficients.copyFrom(newCoefficients);
  }

  FloatArray getState(){
    return states;
  };

  /**
   * Sets state to point to a different set of values
   */
  void setState(FloatArray newState){
    states = newState;
  }
  
  /**
   * Copies state values from an array.
   */
  void copyState(FloatArray newState){
    ASSERT(states.getSize()==newState.getSize(), "wrong size");
    states.copyFrom(newState);
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
