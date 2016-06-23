#ifndef __PatchParameterTestPatch_hpp__
#define __PatchParameterTestPatch_hpp__

#include "StompBox.h"

class PatchParameterTestPatch : public Patch {
private:
  FloatParameter p1;
  FloatParameter p2;
  IntParameter p3;
  FloatParameter p4;

public:
  PatchParameterTestPatch(){
    p1 = getFloatParameter("float", 0.0, 1.0);
    p2 = getFloatParameter("smoothfloat", 0.0, 1.0, 0.5, 0.9);
    p3 = getIntParameter("stiffint", 0, 10, 0, 0, 2);
    p4 = getFloatParameter("expfloat", 0.0, 10.0, 0.0, 0.0, 0.0, 2.0);
  }
  void processAudio(AudioBuffer &buffer){
    //    debugMessage("p1/p2/p3", (float)p1, (float)p2, (float)p3);
    debugMessage("p2/p3/p4", (float)p2, (float)p3, (float)p4);
  }
};

#endif // __PatchParameterTestPatch_hpp__
