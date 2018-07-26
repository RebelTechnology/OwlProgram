#ifndef __basicmaths_h__
#define __basicmaths_h__

#include <stdlib.h>
#include <stdint.h>
#include "heap.h"

#define _USE_MATH_DEFINES
/* Definitions of useful mathematical constants
 * M_E        - e
 * M_LOG2E    - log2(e)
 * M_LOG10E   - log10(e)
 * M_LN2      - ln(2)
 * M_LN10     - ln(10)
 * M_PI       - pi
 * M_PI_2     - pi/2
 * M_PI_4     - pi/4
 * M_1_PI     - 1/pi
 * M_2_PI     - 2/pi
 * M_2_SQRTPI - 2/sqrt(pi)
 * M_SQRT2    - sqrt(2)
 * M_SQRT1_2  - 1/sqrt(2)
 */
#ifdef ARM_CORTEX
#include "arm_math.h" 
#endif //ARM_CORTEX

#ifdef __cplusplus
#include <cmath>
#else
#include <math.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

#ifdef __cplusplus
 extern "C" {
#endif

   float arm_sqrtf(float in);
   void arm_srand32(uint32_t s);
   uint32_t arm_rand32();

   // fast lookup-based exponentials
   float fast_powf(float x, float y);
   float fast_expf(float x);
   float fast_exp2f(float x);
   float fast_exp10f(float x);
   void fast_pow_set_table(const uint32_t* table, int size);

   // fast lookup-based logarithmics
   float fast_logf(float x);
   float fast_log2f(float x);
   float fast_log10f(float x);
   void fast_log_set_table(const float* table, int size);

   uint32_t fast_log2i(uint32_t x);

   // fast approximations
   float fast_atan2f(float a, float b);

   /** generate a random number between 0 and 1 */
   float randf();

   static inline uint32_t log2i(const uint32_t x){
     return (31 - __builtin_clz (x));
   }


#define malloc(x) pvPortMalloc(x)
#define calloc(x, y) pvPortCalloc(x, y)
#define free(x) vPortFree(x)
#define realloc(x, y) pvPortRealloc(x, y);
void *pvPortCalloc(size_t nmemb, size_t size);
void *pvPortRealloc(void *pv, size_t xWantedSize );

#ifdef __cplusplus
}
#endif

#ifdef ARM_CORTEX
#define sin(x) arm_sin_f32(x)
#define sinf(x) arm_sin_f32(x)
#define cos(x) arm_cos_f32(x)
#define cosf(x) arm_cos_f32(x)
#define sqrt(x) arm_sqrtf(x)
#define sqrtf(x) arm_sqrtf(x)
#define rand() arm_rand32()
#define tanh(x) tanhf(x)
#define sinh(x) sinhf(x)
#define cosh(x) coshf(x)

#undef __FAST_MATH__
#define exp10f(x) fast_exp10f(x)
#ifdef __FAST_MATH__ /* set by gcc option -ffast-math */

// fast lookup-based exponentials
#define pow(x, y) fast_powf(x, y)
#define powf(x, y) fast_powf(x, y)
#define exp(x) fast_expf(x)
#define expf(x) fast_expf(x)
#define exp2(x) fast_exp2f(x)
#define exp2f(x) fast_exp2f(x)
#define exp10(x) fast_exp10f(x)
#define exp10f(x) fast_exp10f(x)

// fast lookup-based logarithmics
#ifdef log2
#undef log2 /* defined in math.h */
#endif
#define log(x) fast_logf(x)
#define logf(x) fast_logf(x)
#define log2(x) fast_log2f(x)
#define log2f(x) fast_log2f(x)
#define log10(x) fast_log10f(x)
#define log10f(x) fast_log10f(x)

// fast approximate math functions
#define atan2(x, y) fast_atan2f(x, y)
#define atan2f(x, y) fast_atan2f(x, y)

#endif

#undef RAND_MAX
#define RAND_MAX UINT32_MAX
#endif //ARM_CORTEX

#endif // __basicmaths_h__
