#ifndef __PatchParameterTestPatch_hpp__
#define __PatchParameterTestPatch_hpp__

#include "StompBox.h"

class PatchParameterTestPatch : public Patch {
private:
  FloatParameter p1;
  FloatParameter p2;
  IntParameter p3;
  IntParameter pf;

public:
  void processAudio(AudioBuffer &buffer){
    p1 = getParameter("float", 0.0, 1.0);
    p3 = getParameter("int", -20, 20, 15);
  }
};

#endif // __PatchParameterTestPatch_hpp__
