#ifndef __AmpegPatch_hpp__
#define __AmpegPatch_hpp__

#include "StompBox.h"
#include "FirFilter.h"

class TohocAmpeg810Patch : public Patch
{
public:
  FirFilter *filter;
  const static int numTaps = 962;
  FloatArray coefficients;
  TohocAmpeg810Patch(){
    filter = FirFilter::create(numTaps, getBlockSize());
    coefficients = filter->getCoefficients();
    #include "Ampeg8x10SM57.h" //initializes coefficients
  }
  ~TohocAmpeg810Patch(){
    FirFilter::destroy(filter);
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    filter->processBlock(left);
  }
};

#endif // __AmpegPatch_hpp__
