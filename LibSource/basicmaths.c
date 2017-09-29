#include "basicmaths.h"
#include <stdint.h>
#include "fastpow.h"
#include "FastPowTable.h"
#include "fastlog.h"
#include "FastLogTable.h"

// todo: see
// http://www.hxa.name/articles/content/fast-pow-adjustable_hxa7241_2007.html
// http://www.finesse.demon.co.uk/steven/sqrt.html
// http://www.keil.com/forum/7934/
// http://processors.wiki.ti.com/index.php/ARM_compiler_optimizations

static uint32_t r32seed = 33641;

void arm_srand32(uint32_t s){
  r32seed = s;
}

/**
 * generate an unsigned 32bit pseudo-random number using xorshifter algorithm.
 * "Anyone who considers arithmetical methods of producing random digits is, of course, in a state of sin." 
 * -- John von Neumann.
*/
uint32_t arm_rand32(){
  r32seed ^= r32seed << 13;
  r32seed ^= r32seed >> 17;
  r32seed ^= r32seed << 5;
  return r32seed;
}

float randf(){
  return arm_rand32()/4294967296.0f;
}

float arm_sqrtf(float in){
  float out;
#ifdef ARM_CORTEX
  arm_sqrt_f32(in, &out);
#else
  out=sqrtf(in);
#endif
  return out;
}

/* http://stackoverflow.com/questions/6475373/optimizations-for-pow-with-const-non-integer-exponent */
/* http://www.hxa.name/articles/content/fast-pow-adjustable_hxa7241_2007.html */
/* https://hackage.haskell.org/package/approximate-0.2.2.3/src/cbits/fast.c */

/* ----------------------------------------------------------------------
** Fast approximation to the log2() function.  It uses a two step
** process.  First, it decomposes the floating-point number into
** a fractional component F and an exponent E.  The fraction component
** is used in a polynomial approximation and then the exponent added
** to the result.  A 3rd order polynomial is used and the result
** when computing db20() is accurate to 7.984884e-003 dB.
** http://community.arm.com/thread/6741
** ------------------------------------------------------------------- */
const float log2f_approx_coeff[4] = {1.23149591368684f, -4.11852516267426f, 6.02197014179219f, -3.13396450166353f};
float fastlog2f(float X){
  const float *C = &log2f_approx_coeff[0];
  float Y;
  float F;
  int E;
  // This is the approximation to log2()
  F = frexpf(fabsf(X), &E);
  //  Y = C[0]*F*F*F + C[1]*F*F + C[2]*F + C[3] + E;
  Y = *C++;
  Y *= F;
  Y += (*C++);
  Y *= F;
  Y += (*C++);
  Y *= F;
  Y += (*C++);
  Y += E;
  return(Y);
}

/* Fast arctan2
 * from http://dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization
 */
float fastatan2f(float y, float x){
  const float coeff_1 = M_PI/4;
  const float coeff_2 = 3*M_PI/4;
  float abs_y = fabs(y)+1e-10; // kludge to prevent 0/0 condition
  float r, angle;
  if (x>=0){
    r = (x - abs_y) / (x + abs_y);
    angle = coeff_1 - coeff_1 * r;
  }else{
    r = (x + abs_y) / (abs_y - x);
    angle = coeff_2 - coeff_1 * r;
  }
  if(y < 0)
    return(-angle); // negate if in quad III or IV
  else
    return(angle);
}

float fast_powf(float x, float y){
  return powFastLookup(y, logf(x)*1.44269504088896f, fast_pow_table, fast_pow_precision);
}
    
float fast_expf(float x){
  return powFastLookup(x, 1.44269504088896f, fast_pow_table, fast_pow_precision);
}

float fast_exp2f(float x){
  return powFastLookup(x, 1.0f, fast_pow_table, fast_pow_precision);
}

float fast_exp10f(float x){
  return powFastLookup(x, 3.32192809488736f, fast_pow_table, fast_pow_precision);
}

float fast_logf(float x){
  return logFastLookup(x, fast_log_table, fast_log_precision);
}

float fast_log2f(float x){
  /* log10 (x) equals log (x) / log (10). */
  const float loge10 = logf(10);
  return logFastLookup(x, fast_log_table, fast_log_precision) / loge10;
}
