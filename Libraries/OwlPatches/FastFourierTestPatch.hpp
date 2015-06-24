#ifndef __FastFourierTestPatch_hpp__
#define __FastFourierTestPatch_hpp__

#include "StompBox.h"

class FastFourierTestPatch : public Patch {
private:
  FastFourierTransform transform;
  ComplexFloatArray ca;
  int fftSize;
  Window window;
  Window iWindow;
public:
  FastFourierTestPatch(){
    registerParameter(PARAMETER_A, "Gain");
    transform.init(getBlockSize());
    fftSize = getBlockSize();
    ca = ComplexFloatArray::create(fftSize);
    window = Window::create(Window::HammingWindow, fftSize);
    iWindow = Window::create(Window::HammingWindow, fftSize);
    iWindow.invert();
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A)*2;
    float* buf = buffer.getSamples(0);
    int size = buffer.getSize();
    window.apply(buf);
    transform.fft(buf, ca);
    for(int i=0; i<fftSize; i++){
      ca[i].re *= gain;
      ca[i].im *= gain;
    }
    transform.ifft(ca, buf);
    iWindow.apply(buf);
  }
};

#endif // __FastFourierTestPatch_hpp__
