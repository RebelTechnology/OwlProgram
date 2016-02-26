#ifndef __FastFourierTestPatch_hpp__
#define __FastFourierTestPatch_hpp__

#include "StompBox.h"
#include "FastFourierTransform.h"

class FastFourierTestPatch : public Patch {
private:
  FastFourierTransform transform;
  ComplexFloatArray ca;
public:
  FastFourierTestPatch(){
    int fftSize = getBlockSize();
    transform.init(fftSize);
    ca = ComplexFloatArray::create(fftSize);
  }
  ~FastFourierTestPatch(){
    ComplexFloatArray::destroy(ca);
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray buf = buffer.getSamples(0);
    transform.fft(buf, ca);
    transform.ifft(ca, buf);
  }
};

#endif // __FastFourierTestPatch_hpp__
