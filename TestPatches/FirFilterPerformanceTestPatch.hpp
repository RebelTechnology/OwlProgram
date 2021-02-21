#ifndef __FirFilterPerformanceTestPatch_hpp__
#define __FirFilterPerformanceTestPatch_hpp__

#include "StompBox.h"
#include "FirFilter.h"
class FirFilterPerformanceTestPatch : public Patch {
public:
  FirFilter *filter;
  const static int numTaps = 600;
  FloatArray coefficients;
  FirFilterPerformanceTestPatch(){
    filter = FirFilter::create(numTaps, getBlockSize());
    coefficients = filter->getCoefficients();
    #include "FirInit.h" //initializes coefficients
  }
  ~FirFilterPerformanceTestPatch(){
    FirFilter::destroy(filter);
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    filter->process(left, left);
  }
};

#endif // __FirFilterPerformanceTestPatch_hpp__
