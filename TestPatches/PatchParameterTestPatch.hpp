#ifndef __PatchParameterTestPatch_hpp__
#define __PatchParameterTestPatch_hpp__

#include "StompBox.h"

class PatchParameterTestPatch : public Patch {
private:
  FloatParameter p1;
  FloatParameter p2;
  IntParameter p3;
  IntParameter p4;

public:
  PatchParameterTestPatch(){
    p1 = getParameter("float", 0.0, 1.0);
    p2 = getParameter("smoothfloat", 0.0, 1.0, 0.5, LIN, 0.99);
    p3 = getParameter("int", -20, 20, 15);
    p4 = getParameter("stiffint", 0, 4095, 50, LIN, 0.0, 0.1);
  }
  void processAudio(AudioBuffer &buffer){
    //    debugMessage("p1/p2/p3", (float)p1, (float)p2, (float)p3);
    debugMessage("p2/p3/p4", (float)p2, (float)p3, (float)p4);
  }
};

#endif // __PatchParameterTestPatch_hpp__
