#ifndef __CppTestPatch_hpp__
#define __CppTestPatch_hpp__

#include "OpenWareLibrary.h"

class CppTestPatch : public Patch {
public:
  CppTestPatch(){
    registerParameter(PARAMETER_A, "Gain");
    setParameterValue(PARAMETER_A, 0.5);
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A)*2;
    for(size_t ch=0; ch<getNumberOfChannels(); ++ch)
      buffer.getSamples(ch).multiply(gain);
  }
};

#endif // __CppTestPatch_hpp__
