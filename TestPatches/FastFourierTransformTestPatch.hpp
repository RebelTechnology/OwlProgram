#ifndef __FastFourierTransformTestPatch_hpp__
#define __FastFourierTransformTestPatch_hpp__

#include "StompBox.h"

class FastFourierTransformTestPatch : public Patch {
private:
  bool success;
  int passed;
  int failed;
  int fftsize;
  FastFourierTransform fft;
  ComplexFloatArray fd;
public:
  FastFourierTransformTestPatch(){
    fftsize=getBlockSize();
    fft.init(fftsize);
    fd=ComplexFloatArray::create(fftsize);
    
  };
  void processAudio(AudioBuffer &buffer){
    FloatArray td=buffer.getSamples(0);
    fft.fft(td,fd);
    fft.ifft(fd,td);
  }
};

#endif // __FastFourierTransformTestPatch_hpp__
