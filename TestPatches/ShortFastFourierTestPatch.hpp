#ifndef __ShortFastFourierTestPatch_hpp__
#define __ShortFastFourierTestPatch_hpp__

#include "Patch.h"
#include "ShortFastFourierTransform.h"

class ShortFastFourierTestPatch : public Patch {
private:
  ShortFastFourierTransform transform;
  ComplexShortArray ca;
  ShortArray sb;
public:
  ShortFastFourierTestPatch(){
    int fftSize = getBlockSize();
    transform.init(fftSize);
    ca = ComplexShortArray::create(fftSize);
    sb = ShortArray::create(getBlockSize());
  }
  ~ShortFastFourierTestPatch(){
    ComplexShortArray::destroy(ca);
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray buf = buffer.getSamples(0);
    for(int n = 0; n < sb.getSize(); ++n){
      sb[n] = buf[n] * 32768;    
    }
    transform.fft(sb, ca);
    transform.ifft(ca, sb);
    for(int n = 0; n < sb.getSize(); ++n){
      buf[n] = sb[n] / 32768.f;    
    }
  }
};

#endif // __ShortFastFourierTestPatch_hpp__
