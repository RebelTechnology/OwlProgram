#ifndef __fastmath_h__
#define __fastmath_h__

#include "basicmaths.h"

// This file extends functions from basicmaths.h with all function names
// used by Faust fast maths calls.

// For reference, see
// https://github.com/grame-cncm/faust/blob/master-dev/compiler/global.cpp#L173-L215

// Float versions

#define fast_fabsf(x) abs(x) 
#define fast_acosf(x) acosf(x)
#define fast_asinf(x) asinf(x)
#define fast_atanf(x) atanf(x)
#define fast_atan2f(x, y) atan2f(x, y) // Optimized for ARM
#define fast_ceilf(x) ceilf(x)
#define fast_cosf(x) cosf(x) // Optimized for ARM
// fast_expf is defined in basicmath.h
// fast_exp2f is defined in basicmath.h
// fast_exp10f is defined in basicmath.h
#define fast_floorf(x) floorf(x)
#define fast_fmodf(x, y) fmodf(x, y)
// fast_logf is defined in basicmath.h
// fast_log2f is defined in basicmath.h
// fast_log10f is defined in basicmath.h
// fast_powf is defined in basicmath.h
#define fast_remainderf(x, y) remainderf(x, y)
#define fast_rintf(x) rintf(x)
#define fast_roundf(x) roundf(x)
#define fast_sinf(x) sinf(x) // Optimized for ARM
#define fast_sqrtf(x) sqrtf(x) // Optimized for ARM
#define fast_tanf(x) tanf(x)

// Double versions

// Actually, we'll end up calling float versions and casting them to doubles.
#define fast_fabs(x) abs(x)
#define fast_acos(x) fast_acosf(x)
#define fast_asin(x) fast_asinf(x)
#define fast_atan(x) fast_atanf(x)
#define fast_atan2(x, y) fast_atan2f(x, y)
#define fast_ceil(x) fast_ceilf(x)
#define fast_cos(x) fast_cosf(x)
#define fast_exp(x) fast_expf(x)
#define fast_exp2(x, y) fast_exp2f(x, y)
#define fast_exp10(x) fast_exp10f(x)
#define fast_floor(x) fast_floorf(x)
#define fast_fmod(x, y) fast_fmodf(x, y)
#define fast_log(x) fast_logf(x)
#define fast_log2(x) fast_log2f(x)
#define fast_log10(x) fast_log10f(x)
#define fast_pow(x, y) fast_powf(x, y)
#define fast_remainder(x, y) fast_remainderf(x, y)
#define fast_rint(x) fast_rintf(x)
#define fast_round(x) fast_roundf(x)
#define fast_sin(x) fast_sinf(x)
#define fast_sqrt(x) fast_sqrtf(x)
#define fast_tan(x) fast_tanf(x)

#endif
