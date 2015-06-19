#ifndef __FastFourierTestPatch_hpp__
#define __FastFourierTestPatch_hpp__

#include "StompBox.h"
#include "FastFourierTranform.hpp"

class FastFourierTestPatch : public Patch {
private:
  FastFourierTransform transform;
  ComplexFloatArray ca;
  int fftSize;
  float *window;
  int count;
public:
  FastFourierTestPatch() : 
    ca(NULL,0)
  {
    count=0;
    registerParameter(PARAMETER_A, "Gain");
    transform.init(getBlockSize());
    fftSize=getBlockSize();
    float *values=createMemoryBuffer(1, fftSize*2)->getSamples(0);
    ca.setData((ComplexFloat *)values);
    ca.setSize(fftSize);
    window=createMemoryBuffer(1, fftSize)->getSamples(0);
    Window::window(Window::kHammingWindow, window, fftSize);
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A)*2;
    float* buf = buffer.getSamples(0);
    int size = buffer.getSize();
/*
  lame example of how to apply your window destroying the signal: requires overlap and add
*/

    Window::applyWindow(buf, window, buf, fftSize);
    transform.fft(buf, ca);
    for(int i=0; i<fftSize; i++){
      ca[i].re *= gain;
      ca[i].im *= gain;
    }
    transform.ifft(ca, buf);
    Window::applyWindow(buf, window, fftSize);
    count++;
  }
};

#endif // __FastFourierTestPatch_hpp__
