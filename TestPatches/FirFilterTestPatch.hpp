#ifndef __FirFilterTestPatch_hpp__
#define __FirFilterTestPatch_hpp__

#include "StompBox.h"
#include "FirFilter.h"
class FirFilterTestPatch : public Patch {
public:
  FirFilter *filter;
  int numTaps=64;
  FloatArray temp;
  FloatArray coefficients;
  FirFilterTestPatch(){
    filter=FirFilter::create(numTaps, getBlockSize());
    coefficients = filter->getCoefficients();
    #include "FirInit.h" //initializes coefficients
    temp=FloatArray::create(getBlockSize());
  }
  ~FirFilterTestPatch(){
    FloatArray::destroy(temp);
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray fa=buffer.getSamples(0);
    // fa.noise();
    filter->process(fa, temp);
    // fa.copyFrom(temp);
    // debugMessage("max value",fa.getMaxValue());
  }
};

#endif // __FirFilterTestPatch_hpp__
