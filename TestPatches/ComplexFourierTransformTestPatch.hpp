#ifndef __ComplexFourierTransformTestPatch_hpp__
#define __ComplexFourierTransformTestPatch_hpp__

#include "StompBox.h"
#include "ComplexFourierTransform.h"
class ComplexFourierTransformTestPatch : public Patch {
private:
  bool success;
  int passed;
  int failed;
  int fftsize;
  ComplexFourierTransform fftArm;
  // ComplexFourierTransform fftGeneric;
  bool assertt(float first, float second, const char *message){ //assert with tolerance
    bool cond=false;
    float tol=0.0000001;
    if(abs(second-first)<tol){
      cond=true;
    }
    return assert(cond, message);
  }
  bool assert(bool condition,const char * message){
    return assert(condition, message, -1);
  }
  bool assert(bool condition,const char * message, int n){
    if(condition==false && success==true){ //only hit this the first time, so only the first failed assertion gets displayed
      success=false;
      if(n>=0)
        debugMessage(message, n);
      else
        debugMessage(message);
      ASSERT(condition, message); //this actually causes the program to exit and the LED to turn red
      //but if you comment out the line above you can get nice acoustic feedback instead (sinewave for success, noise for error)
      failed++;
    } else {
      passed++;
    }
    return condition;
  }
  ComplexFloatArray inout;
public:
  ComplexFourierTransformTestPatch(){
    fftsize=getBlockSize();
    fftArm.init(fftsize);
    inout=ComplexFloatArray::create(fftsize);
    
  };
  void processAudio(AudioBuffer &buffer){
    FloatArray samples=buffer.getSamples(0);
    inout.copyFrom(samples); //copies only real values 
    fftArm.fft(inout);
    fftArm.ifft(inout);
    inout.copyTo(samples);
    samples.multiply(0.2);

  }
};

#endif // __ComplexFourierTransformTestPatch_hpp__
