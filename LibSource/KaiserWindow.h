/*
 * Adapted from the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2010 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * KaiserWindow.C
 *
 * Implementation of class Loris::KaiserWindow.
 *
 * Kelly Fitz, 14 Dec 1999
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Window.h"

class KaiserWindow : public Window {
public:
  KaiserWindow(): Window(){}
  KaiserWindow(float* data, size_t size): Window(data, size){}
  
  static KaiserWindow create(float shape, size_t size, bool derivative = false){
    KaiserWindow win(new float[size], size);
    if(derivative)
      buildWindow(win, shape);
    else
      buildTimeDerivativeWindow(win, shape);
    return win;
  }

  static void destroy(KaiserWindow obj){
    Window::destroy(obj);
  }

  // ---------------------------------------------------------------------------
  //  buildWindow
  // ---------------------------------------------------------------------------
  //! Build a new Kaiser analysis window having the specified shaping
  //! parameter. See Oppenheim and Schafer:  "Digital Signal Processing" 
  //! (1975), p. 452 for further explanation of the Kaiser window. Also, 
  //! see Kaiser and Schafer, 1980.
  //!
  //! \param      win is the vector that will store the window
  //!             samples. The number of samples computed will be
  //!             equal to the length of this vector. Any previous
  //!             contents will be overwritten.
  //! \param      shape is the Kaiser shaping parameter, controlling
  //!             the sidelobe rejection level.
  // α is a non-negative real number that determines the shape of the window.
  // In the frequency domain, it determines the trade-off between main-lobe width
  // and side lobe level, which is a central decision in window design.
  // Sometimes the Kaiser window is parametrized by β, where β = πα.
  //
  static void buildWindow(FloatArray win, float shape){   
    //  Pre-compute the shared denominator in the Kaiser equation. 
    const float oneOverDenom = 1.0 / zeroethOrderBessel( shape );
    const unsigned int N = win.getSize() - 1;
    const float oneOverN = 1.0 / N;
    float* data = win.getData();
    for(unsigned int n = 0; n <= N; ++n){
      const float K = (2.0 * n * oneOverN) - 1.0;
      const float arg = sqrtf( 1.0 - (K * K) );        
      data[n] = zeroethOrderBessel(shape * arg) * oneOverDenom;
      // consider using std::cyl_bessel_jf(0, shape * arg) (c++17)
    }
  }

 
  // ---------------------------------------------------------------------------
  //  createDerivativeWindow
  // ---------------------------------------------------------------------------
  //! Build a new time-derivative Kaiser analysis window having the
  //! specified shaping parameter, for computing frequency reassignment.
  //! The closed form for the time derivative can be obtained from the
  //! property of modified Bessel functions that the derivative of the
  //! zeroeth order function is equal to the first order function.
  //!
  //! \param      win is the vector that will store the window
  //!             samples. The number of samples computed will be
  //!             equal to the length of this vector. Any previous
  //!             contents will be overwritten.
  //! \param      shape is the Kaiser shaping parameter, controlling
  //!             the sidelobe rejection level.
  //
  static void buildTimeDerivativeWindow(FloatArray win, float shape){   
    //  Pre-compute the common factor that does not depend on n.
    const unsigned int N = win.getSize() - 1;
    const float oneOverN = 1.0 / N;
    const float commonFac = - 2.0 * shape / (N * zeroethOrderBessel(shape) );
      //  w'[0] = w'[N] = 0
    win[0] = win[N] = 0.0;    
    float* data = win.getData();
    for(unsigned int n = 1; n < N; ++n){
      const float K = (2.0 * n * oneOverN) - 1.0;
      const float arg = sqrtf( 1.0 - (K * K) );
      data[n] = commonFac * firstOrderBessel(shape * arg) * K / arg;
      // consider using std::cyl_bessel_jf(1, shape * arg) (c++17)
    }
  }
 
  // ---------------------------------------------------------------------------
  //  zeroethOrderBessel
  // ---------------------------------------------------------------------------
  //  Compute the zeroeth order modified Bessel function of the first kind 
  //  at x using the series expansion, used to compute the Kasier window
  //  function.
  //
  static float zeroethOrderBessel(float x){
    const float eps = 0.000001;
    
    //  initialize the series term for m=0 and the result
    float besselValue = 0;
    float term = 1;
    float m = 0;
    
    //  accumulate terms as long as they are significant
    while(term  > eps * besselValue){
      besselValue += term;
        
      //  update the term
      ++m;
      term *= (x*x) / (4*m*m);
    }
    
    return besselValue;
  }

  // ---------------------------------------------------------------------------
  //  firstOrderBessel
  // ---------------------------------------------------------------------------
  //  Compute the first order modified Bessel function of the first kind 
  //  at x using the series expansion, used to compute the time derivative
  //  of the Kasier window function for computing frequency reassignment.
  //
  static float firstOrderBessel(float x){
    const float eps = 0.000001;
    
    //  initialize the series term for m=0 and the result
    float besselValue = 0;
    float term = .5*x;
    float m = 0;
    
    //  accumulate terms as long as they are significant
    while(term  > eps * besselValue)
      {
        besselValue += term;
        
        //  update the term
        ++m;
        term *= (x*x) / (4*m*(m+1));
      }
    
    return besselValue;
  }

  // ---------------------------------------------------------------------------
  //  computeShape
  // ---------------------------------------------------------------------------
  //  Compute the Kaiser window shaping parameter from the specified attenuation 
  //  of side lobes. This algorithm is given in Kaiser an Schafer,1980 and is 
  //  supposed to give better than 0.36% accuracy (Kaiser and Schafer 1980).
  //
  static float computeShape(float atten){
    ASSERT(atten >= 0, "Kaiser window shape must be > 0dB");
    float alpha;
    if (atten > 60.0){
      alpha = 0.12438 * (atten + 6.3);
    }else if(atten > 13.26){
      alpha = 0.76609L * ( powf((atten - 13.26), 0.4) ) + 
	0.09834L * (atten - 13.26L);
    }else{
      //  can't have less than 13dB.
      alpha = 0.0;
    }    
    return alpha;
  }
 
  // ---------------------------------------------------------------------------
  //  computeLength
  // ---------------------------------------------------------------------------
  //  Compute the length (in samples) of the Kaiser window from the desired 
  //  (approximate) main lobe width and the control parameter. Of course, since 
  //  the window must be an integer number of samples in length, your actual 
  //  lobal mileage may vary. This equation appears in Kaiser and Schafer 1980
  //  (on the use of the I0 window class for spectral analysis) as Equation 9.
  //
  //  The main width of the main lobe must be normalized by the sample rate,
  //  that is, it is a fraction of the sample rate.
  //
  static size_t computeLength(float width, float alpha) {
    //  The last 0.5 is cheap rounding.
    //  But I think I don't need cheap rounding because the equation 
    //  from Kaiser and Schafer has a +1 that appears to be a cheap
    //  ceiling function.
    return long(1.0 + (2. * sqrtf((M_PI*M_PI) + (alpha*alpha)) / (M_PI * width)) /* + 0.5 */);
  }

};
