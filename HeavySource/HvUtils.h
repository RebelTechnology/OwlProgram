/**
 * Copyright (c) 2014,2015,2016 Enzien Audio Ltd.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _HEAVY_UTILS_H_
#define _HEAVY_UTILS_H_

// platform definitions
#if _WIN32 || _WIN64
  #define HV_WIN 1
#ifdef _MSC_VER
  #define HV_MSVC 1
#endif
#elif __APPLE__
  #define HV_APPLE 1
#elif __unix__ || __unix
  #define HV_UNIX 1
#else
  #warning Could not detect platform. Assuming Unix-like.
#endif

// basic includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// type definitions
#include <stdint.h>
#include <stdbool.h>
#define hv_size_t size_t
#define hv_uint32_t uint32_t
#define hv_int32_t int32_t
#define hv_uint16_t uint16_t

// SIMD-specific includes
#ifndef HV_SIMD_NONE
  #define HV_SIMD_NEON __ARM_NEON__
  #define HV_SIMD_SSE (__SSE__ && __SSE2__ && __SSE3__ && __SSSE3__ && __SSE4_1__)
  #define HV_SIMD_AVX (__AVX__ && HV_SIMD_SSE)
  #define HV_SIMD_FMA __FMA__
#endif

#if HV_SIMD_AVX || HV_SIMD_SSE
  #include <immintrin.h>
#elif HV_SIMD_NEON
  #include <arm_neon.h>
#endif

#if HV_SIMD_NEON // NEON
  #define HV_N_SIMD 4
  #define hv_bufferf_t float32x4_t
  #define hv_bufferi_t int32x4_t
  #define hv_bInf_t float32x4_t
  #define hv_bOutf_t float32x4_t*
  #define hv_bIni_t int32x4_t
  #define hv_bOuti_t int32x4_t*
  #define VIf(_x) (_x)
  #define VOf(_x) (&_x)
  #define VIi(_x) (_x)
  #define VOi(_x) (&_x)
#elif HV_SIMD_AVX // AVX
  #define HV_N_SIMD 8
  #define hv_bufferf_t __m256
  #define hv_bufferi_t __m256i
  #define hv_bInf_t __m256
  #define hv_bOutf_t __m256*
  #define hv_bIni_t __m256i
  #define hv_bOuti_t __m256i*
  #define VIf(_x) (_x)
  #define VOf(_x) (&_x)
  #define VIi(_x) (_x)
  #define VOi(_x) (&_x)
#elif HV_SIMD_SSE // SSE
  #define HV_N_SIMD 4
  #define hv_bufferf_t __m128
  #define hv_bufferi_t __m128i
  #define hv_bInf_t __m128
  #define hv_bOutf_t __m128*
  #define hv_bIni_t __m128i
  #define hv_bOuti_t __m128i*
  #define VIf(_x) (_x)
  #define VOf(_x) (&_x)
  #define VIi(_x) (_x)
  #define VOi(_x) (&_x)
#else // DEFAULT
  #define HV_N_SIMD 1
  #undef HV_SIMD_NONE
  #define HV_SIMD_NONE 1
  #define hv_bufferf_t float
  #define hv_bufferi_t int
  #define hv_bInf_t float
  #define hv_bOutf_t float*
  #define hv_bIni_t int
  #define hv_bOuti_t int*
  #define VIf(_x) (_x)
  #define VOf(_x) (&_x)
  #define VIi(_x) (_x)
  #define VOi(_x) (&_x)
#endif

#define HV_N_SIMD_MASK (HV_N_SIMD-1)

// Strings
#include <string.h>
#define hv_strlen(a) strlen(a)
#define hv_strncpy(a, b, c) strncpy(a, b, c)
#define hv_strcmp(a, b) strcmp(a, b)
#define hv_snprintf(a, b, c, ...) snprintf(a, b, c, __VA_ARGS__)

// Memory management
#define hv_realloc(a, b) realloc(a, b)
#define hv_memcpy(a, b, c) memcpy(a, b, c)
#define hv_memclear(a, b) memset(a, 0, b)
#if HV_MSVC
  #include <malloc.h>
  #define hv_alloca(_n) _alloca(_n)
  #if HV_SIMD_AVX
    #define hv_malloc(_n) _aligned_malloc(_n, 32)
    #define hv_free(x) _aligned_free(x)
  #elif HV_SIMD_SSE || HV_SIMD_NEON
    #define hv_malloc(_n) _aligned_malloc(_n, 16)
    #define hv_free(x) _aligned_free(x)
  #else // HV_SIMD_NONE
    #define hv_malloc(_n) malloc(_n)
    #define hv_free(_n) free(_n)
  #endif
#elif HV_APPLE
  #define hv_alloca(_n) alloca(_n)
  #if HV_SIMD_AVX
    #define hv_malloc(_n) _mm_malloc(_n, 32)
    #define hv_free(x) _mm_free(x)
  #elif HV_SIMD_SSE || HV_SIMD_NEON
    #define hv_malloc(_n) _mm_malloc(_n, 16)
    #define hv_free(x) _mm_free(x)
  #else // HV_SIMD_NONE
    #define hv_malloc(_n) malloc(_n)
    #define hv_free(x) free(x)
  #endif
#else
#if defined __GNUC__
  #define hv_alloca(_n)  __builtin_alloca(_n)
#else
  #define hv_alloca(_n)  alloca(_n)
#endif
  #if HV_SIMD_AVX
    #define hv_malloc(_n) aligned_alloc(32, _n)
    #define hv_free(x) free(x)
  #elif HV_SIMD_SSE || HV_SIMD_NEON
    #define hv_malloc(_n) aligned_alloc(16, _n)
    #define hv_free(x) free(x)
  #else // HV_SIMD_NONE
    #define hv_malloc(_n) malloc(_n)
    #define hv_free(_n) free(_n)
  #endif
#endif

// Assert
#include <assert.h>
#define hv_assert(e) assert(e)

// Export and Inline
#if HV_MSVC
#define HV_EXPORT __declspec(dllexport)
#define inline __inline
#define HV_FORCE_INLINE __forceinline
#else
#define HV_EXPORT
#define HV_FORCE_INLINE inline __attribute__((always_inline))
#endif

// Math
#ifdef ARM_CORTEX
#include <basicmaths.h>
#else
#include <math.h>
#endif

static inline hv_size_t __hv_utils_max_ui(hv_size_t x, hv_size_t y) { return (x > y) ? x : y; }
static inline hv_size_t __hv_utils_min_ui(hv_size_t x, hv_size_t y) { return (x < y) ? x : y; }
static inline hv_int32_t __hv_utils_max_i(hv_int32_t x, hv_int32_t y) { return (x > y) ? x : y; }
static inline hv_int32_t __hv_utils_min_i(hv_int32_t x, hv_int32_t y) { return (x < y) ? x : y; }
#define hv_max_ui(a, b) __hv_utils_max_ui(a, b)
#define hv_min_ui(a, b) __hv_utils_min_ui(a, b)
#define hv_max_i(a, b) __hv_utils_max_i(a, b)
#define hv_min_i(a, b) __hv_utils_min_i(a, b)
#define hv_max_f(a, b) fmaxf(a, b)
#define hv_min_f(a, b) fminf(a, b)
#define hv_max_d(a, b) fmax(a, b)
#define hv_min_d(a, b) fmin(a, b)
#ifdef ARM_CORTEX
#define hv_sin_f(a) arm_sin_f32(a)
#define hv_cos_f(a) arm_cos_f32(a)
#define hv_sqrt_f(a) arm_sqrtf(a)
#else
#define hv_sin_f(a) sinf(a)
#define hv_cos_f(a) cosf(a)
#define hv_sqrt_f(a) sqrtf(a)
#endif
#define hv_sinh_f(a) sinhf(a)
#define hv_cosh_f(a) coshf(a)
#define hv_tan_f(a) tanf(a)
#define hv_tanh_f(a) tanhf(a)
#define hv_asin_f(a) asinf(a)
#define hv_asinh_f(a) asinhf(a)
#define hv_acos_f(a) acosf(a)
#define hv_acosh_f(a) acoshf(a)
#define hv_atan_f(a) atanf(a)
#define hv_atanh_f(a) atanhf(a)
#define hv_atan2_f(a, b) atan2f(a, b)
#define hv_exp_f(a) expf(a)
#define hv_abs_f(a) fabsf(a)
#define hv_log_f(a) logf(a)
#define hv_log2_f(a) log2f(a)
#define hv_log10_f(a) log10f(a)
#define hv_ceil_f(a) ceilf(a)
#define hv_floor_f(a) floorf(a)
#define hv_round_f(a) roundf(a)
#define hv_pow_f(a, b) powf(a, b)
#define hv_fma_f(a, b, c) ((a*b)+c) // TODO(joe): use 'fmaf(a, b, c)' once emscripten supports it
#if HV_MSVC
  // finds ceil(log2(x))
  #include <intrin.h>
  static inline hv_uint32_t __hv_utils_min_max_log2(hv_uint32_t x) {
    unsigned long z = 0;
    _BitScanReverse(&z, x);
    return (hv_uint32_t) (z+1);
  }
#else
  static inline hv_uint32_t __hv_utils_min_max_log2(hv_uint32_t x) {
    return (hv_uint32_t) (32 - __builtin_clz(x-1));
  }
#endif
#define hv_min_max_log2(a) __hv_utils_min_max_log2(a)

#endif // _HEAVY_UTILS_H_
