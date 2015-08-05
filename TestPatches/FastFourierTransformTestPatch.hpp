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
  SuperSaw ss;
public:
  FastFourierTransformTestPatch(): ss(getSampleRate()){
    
    fftsize=getBlockSize();
    fft.init(fftsize);
    fd=ComplexFloatArray::create(fftsize);
    
  };
  void processAudio(AudioBuffer &buffer){
    ss.setFrequency(300);
    FloatArray td=buffer.getSamples(0);
    ss.getSamples(td);
    fft.fft(td,fd);
    fft.ifft(fd,td);
    td.scale(0.1);
  }
};

#endif // __FastFourierTransformTestPatch_hpp__
