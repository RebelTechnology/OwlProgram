#ifndef __ShortFastFourierTestPatch_hpp__
#define __ShortFastFourierTestPatch_hpp__

#include <stdlib.h>
#include <stdio.h>
#include "Patch.h"
#include "ShortFastFourierTransform.h"

void prettyPrint(ComplexShortArray ca){
    printf("[");
    for(int n = 0; n < ca.getSize(); ++n){
      printf("%d + %di, ", ca[n].re, ca[n].im);
    }
    printf("\n];\n");
}
void prettyPrint(ShortArray sa){
    printf("[");
    for(int n = 0; n < sa.getSize(); ++n){
      printf("%d, ", sa[n]);
    }
    printf("\n];\n");
}
class ShortFastFourierTestPatch : public TestPatch {
private:
  ShortFastFourierTransform transform;
  ComplexShortArray ca;
  ShortArray sa;
public:
  ShortFastFourierTestPatch(){
    int fftSize = 512;
    transform.init(fftSize);
    ca = ComplexShortArray::create(fftSize);
    sa = ShortArray::create(fftSize);
    for(int n = 0; n < sa.getSize(); ++n){
      // x = mod( (0:fftSize-1) * 4096, 32768) - 16384;
      sa[n] = ((n * 4096) % 32768 ) -16384;
    }
    transform.fft(sa, ca);
    prettyPrint(ca);
    transform.ifft(ca, sa);
  }
  ~ShortFastFourierTestPatch(){
    ComplexShortArray::destroy(ca);
  }
};

#endif // __ShortFastFourierTestPatch_hpp__
