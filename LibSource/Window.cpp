#include "Window.h"

void Window::applyWindow(float *signalIn, float *window, float *signalOut, int size){
#ifdef ARM_CORTEX
  /* despite not explicitely documented in the CMSIS documentation,
      this has been tested to behave properly even when pSrcA==pDst
      void 	arm_mult_f32 (float32_t *pSrcA, float32_t *pSrcB, float32_t *pDst, uint32_t blockSize)
  */
  arm_mult_f32(signalIn, window, signalOut, size);
#else
  for(int n=0; n<size; n++)
    signalOut[n] = window[n]*signalIn[n];
#endif /* ARM_CORTEX */
}
