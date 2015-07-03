#ifndef __FastFourierTransformTestPatch_hpp__
#define __FastFourierTransformTestPatch_hpp__

#include "StompBox.h"

class FastFourierTransformTestPatch : public Patch {
private:
  bool success;
  int passed;
  int failed;
  int fftsize;
  FastFourierTransform fftArm;
  FastFourierTransform fftGeneric;
  bool assertt(float first, float second, const char *message){ //assert with tolerance
    bool cond=false;
    float tol=0.0000001;
    if(abs(second-first)<tol){
      cond=true;
    }
    assert(cond, message);
  }
  bool assert(bool condition,const char * message){
    assert(condition, message, -1);
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
  ComplexFloatArray fd;
public:
  FastFourierTransformTestPatch(){
     registerParameter(PARAMETER_A, "Toggle FFT method");
    // success=true;
    // passed=0;
    // failed=0;
    fftsize=getBlockSize();
    fftArm.init(fftsize);
    fftGeneric.init(fftsize);
    fd=ComplexFloatArray::create(fftsize);
    // FastFourierTransform fftArm;
    // FastFourierTransformGeneric fftGeneric;
    // fftArm.init(fftsize);
    // fftGeneric.init(fftsize);
    
  };
  void processAudio(AudioBuffer &buffer){
    FloatArray td=buffer.getSamples(0);
    if(getParameterValue(PARAMETER_A)>0.5){
      debugMessage("generic");
      fftGeneric.fft(td,fd);
      fftGeneric.ifft(fd,td);
    } else {
      debugMessage("arm");
      fftArm.fft(td,fd);
      fftArm.ifft(fd,td);
    }
    td.scale(0.2);

    /*
    float *sig=buffer.getSamples(0);
    if(success==false){
      for(int n=0; n<getBlockSize(); n++){
        sig[n]+=0.05*rand()/(float)RAND_MAX;
      }
    } else {
      static float phase=0;
      float inc=2*M_PI/200.0f;
      for(int n=0; n<getBlockSize(); n++){
        sig[n]+=0.2*sinf(phase);
        phase+=inc;
        phase= phase>2*M_PI ? phase-2*M_PI : phase;
      }
    }*/
  }
};

#endif // __FastFourierTransformTestPatch_hpp__
