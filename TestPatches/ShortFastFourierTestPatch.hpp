#ifndef __ShortFastFourierTestPatch_hpp__
#define __ShortFastFourierTestPatch_hpp__

#include <stdlib.h>
#include <stdio.h>
#include "Patch.h"
#include "ShortFastFourierTransform.h"

void prettyPrint(ComplexShortArray X){
    printf("[");
    for(int n = 0; n < X.getSize(); ++n){
      printf("%d + %di; ", X[n].re, X[n].im);
    }
    printf("];\n\n");
}
void prettyPrint(ShortArray x){
    printf("[");
    for(int n = 0; n < x.getSize(); ++n){
      printf("%d; ", x[n]);
    }
    printf("];\n\n");
}
class ShortFastFourierTestPatch : public TestPatch {
private:
  ShortFastFourierTransform transform;
  ComplexShortArray X;
  ShortArray x;
public:
  ShortFastFourierTestPatch(){
    int fftSize = 512;
    transform.init(fftSize);
    X = ComplexShortArray::create(fftSize);
    x = ShortArray::create(fftSize);
// matlab code:
/*
fftSize = 512;
x = mod( (0:fftSize-1)' * 4096, 32768) - 16384;
X = fft(x)/fftSize;
*/
    for(int n = 0; n < x.getSize(); ++n){
      x[n] = ((n * 4096) % 32768 ) - 16384;
    }
    transform.fft(x, X);
    ShortArray data((int16_t*)X.getData(), X.getSize() * 2);
    printf("Xc=");
// At this point the values of X are pretty similar to the ones in matlab.
// But their magnitude is pretty small (~2048).
// You could try to left shift them to increase the magnitude again and
// reduce the rounding error that will be caused by the ifft:
//    data.shift(16 - log2(data.getMaxValue()));
//  but this would most likely cause saturation when you do ifft ! 
    prettyPrint(X);
    transform.ifft(X, x);
// shift back to 1.15 
    x.shift(log2(fftSize));
// At this point, with infinite precision, x should be back to being exactly
// the same as the one we started with, instead there are some large errors
// (~1024)
    printf("xc=");
    prettyPrint(x);
  }
  ~ShortFastFourierTestPatch(){
    ComplexShortArray::destroy(X);
  }
};

#endif // __ShortFastFourierTestPatch_hpp__
