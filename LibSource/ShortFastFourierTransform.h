#ifndef __ShortFastFourierTransform_h__
#define __ShortFastFourierTransform_h__

#include "basicmaths.h"
#include "ShortArray.h"
#include "ComplexShortArray.h"

#ifndef ARM_CORTEX
#define FIXED_POINT 16
#include "kiss_fft.h"
#endif /* ARM_CORTEX */

/**
 * This class performs direct and inverse ShortFast Fourier Transform.
 * Note that on ARM, this links in static coefficients for all supported FFT sizes!
 */ 
class ShortFastFourierTransform {
private:
#ifdef ARM_CORTEX
  unsigned int len;
  arm_rfft_instance_q15 instance;
#else /* ARM_CORTEX */
  kiss_fft_cfg cfgfft;
  kiss_fft_cfg cfgifft;
  ComplexShortArray temp;
#endif /* ARM_CORTEX */

public:
  /**
   * Default constructor.
   * Does **not** initialize the instance.
   * @remarks You need to call init(int size) before calling any other method
  */
  ShortFastFourierTransform();

  /**
   * Construct and initialize the instance.
   * @param[in] aSize The size of the FFT
   * @remarks Only sizes of 32, 64, 128, 256, 512, 1024, 2048, 4096 are supported, due to limitations of the CMSIS library.
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  */
  ShortFastFourierTransform(int aSize);

  ~ShortFastFourierTransform();

  /**
   * Initialize the instance.
   * @param aSize The size of the FFT
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  */
  void init(int aSize);

  /**
   * Perform the direct FFT.
   * @param[in] input The real-valued input array
   * @param[out] output The complex-valued output array
   * @remarks Calling this method will mess up the content of the **input** array.
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
  */
  void fft(ShortArray input, ComplexShortArray output);
  
  /**
   * Perform the inverse FFT.
   * The output is rescaled by 1/fftSize.
   * @param[in] input The complex-valued input array
   * @param[out] output The real-valued output array
   * @remarks Calling this method will mess up the content of the **input** array.
   * @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
   * 
  */
  void ifft(ComplexShortArray input, ShortArray output);
    
  /**
   * Get the size of the FFT
   * @return The size of the FFT
  */
  size_t getSize();
};

#endif // __ShortFastFourierTransform_h__
