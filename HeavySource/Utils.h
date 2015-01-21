/* Copyright 2013,2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_UTILS_H_
#define _HEAVY_UTILS_H_

// Type definitions
#if defined(_WIN32) || defined(_WIN64) || defined(WINAPI_FAMILY)
#define HV_WIN 1
#include <stddef.h>
#if defined (_MSC_VER)
#define HV_MSVC 1
#endif
#define hv_size_t unsigned long
#define hv_uint32_t unsigned int
#define hv_uint16_t unsigned short
#define hv_int32_t int
#elif defined(__APPLE__) && defined(__MACH__)
#define HV_APPLE 1
#include <stddef.h>
#define hv_size_t size_t
#define hv_uint32_t unsigned int
#define hv_uint16_t unsigned short
#define hv_int32_t int
#elif defined (__unix__) || (__unix)
#define HV_UNIX 1
#include <stddef.h>
#define hv_size_t size_t
#define hv_uint32_t unsigned int
#define hv_uint16_t unsigned short
#define hv_int32_t int
#else
#error Unsupported platform
#endif

// Memory management
extern void *hv_alloca(hv_size_t numbytes);
extern void *hv_malloc(hv_size_t numbytes); // allocates memory on 16 byte boundaries and clears it to zero
extern void hv_free(void *ptr); // frees aligned memory
extern void *hv_realloc(void *ptr, hv_size_t numBytes);
extern void *hv_memcpy(void *dest, const void *src, hv_size_t numbytes);
extern void *hv_memset(void *ptr, hv_size_t numbytes); // sets everything to 0

// String handling
extern hv_size_t hv_strlen(const char *str);
extern char *hv_strncat(char *dest, const char *str, hv_size_t n);
extern char *hv_strncpy(char *dest, const char *str, hv_size_t n);
extern int hv_strcmp(const char *str1, const char *str2);
extern int hv_strncmp(const char *str1, const char *str2, hv_size_t n);
extern int hv_snprintf(char *dest, hv_size_t n, const char *format, ...);

// Math
extern int hv_max_i(int x, int y);
extern hv_size_t hv_max_ui(hv_size_t x, hv_size_t y);
extern int hv_min_i(int x, int y);
extern hv_size_t hv_min_ui(hv_size_t x, hv_size_t y);
extern float hv_max_f(float x, float y);
extern float hv_min_f(float x, float y);
extern double hv_max_d(double x, double y);
extern double hv_min_d(double x, double y);
extern float hv_sin_f(float x);
extern float hv_sinh_f(float x);
extern float hv_cos_f(float x);
extern float hv_cosh_f(float x);
extern float hv_tan_f(float x);
extern float hv_tanh_f(float x);
extern float hv_asin_f(float x);
extern float hv_asinh_f(float x);
extern float hv_acos_f(float x);
extern float hv_acosh_f(float x);
extern float hv_atan_f(float x);
extern float hv_atanh_f(float x);
extern float hv_exp_f(float x);
extern float hv_abs_f(float x);
extern float hv_sqrt_f(float x);
extern float hv_log_f(float x);
extern float hv_log2_f(float x);
extern float hv_log10_f(float x);
extern float hv_ceil_f(float x);
extern float hv_floor_f(float x);
extern float hv_round_f(float x);

// Utilities
extern void hv_assert(int e);
extern void hv_clear_buffer(float *b, int n);
extern hv_uint32_t hv_min_max_log2(hv_uint32_t x);

#ifdef HV_WIN
#include "Utils_windows.h"
#elif HV_APPLE
#include "Utils_mac.h"
#elif HV_UNIX
#include "Utils_unix.h"
#else
#error Unsupported platform
#endif

// SIMD
#if __ARM_NEON__
#define hv_bufferf_t float32x4_t
#define hv_bufferi_t int32x4_t
#define hv_bInf_t float32x4_t
#define hv_bOutf_t float32x4_t*
#define hv_bIni_t int32x4_t
#define hv_bOuti_t int32x4_t*
#define VI(_x) (_x)
#define VO(_x) (&_x)
#define HV_N_SIMD 4
#elif __AVX__
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
#define HV_N_SIMD 8
#elif __SSE__
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
#define HV_N_SIMD 4
#else
typedef struct hv_veci128 {
  int a;
  int b;
  int c;
  int d;
} hv_veci128;
typedef struct hv_vecf128 {
  float a;
  float b;
  float c;
  float d;
} hv_vecf128;
#define hv_bufferf_t hv_vecf128
#define hv_bufferi_t hv_veci128
#define hv_bInf_t hv_vecf128
#define hv_bOutf_t hv_vecf128*
#define hv_bIni_t hv_veci128
#define hv_bOuti_t hv_veci128*
#define VIf(_x) ((float *) &_x)
#define VOf(_x) ((float *) &_x)
#define VIi(_x) ((int *) &_x)
#define VOi(_x) ((int *) &_x)
#define HV_N_SIMD 4
#endif
#define HV_N_SIMD_MASK (HV_N_SIMD-1)

#endif // _HEAVY_UTILS_H_
