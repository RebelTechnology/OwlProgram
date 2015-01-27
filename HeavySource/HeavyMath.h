/** Copyright 2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_MATH_H_
#define _HEAVY_MATH_H_

#include "Utils.h"

static const float HV_MAX_INT_AS_FLOAT = 0.0000001192093f; // ((2^23)-1)^-1

static inline void sZero_process(hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_setzero_ps();
#elif HV_SIMD_SSE
  *bOut = _mm_setzero_ps();
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  hv_memset(bOut, HV_N_SIMD*sizeof(float));
#endif
}

static inline void sLoad_process(float *bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_load_ps(bIn);
#elif HV_SIMD_SSE
  *bOut = _mm_load_ps(bIn);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  hv_memcpy(bOut, (const void *) &bIn, sizeof(hv_bufferf_t));
#endif
}

static inline void sStore_process(float *bOut, hv_bInf_t bIn) {
#if HV_SIMD_AVX
  _mm256_store_ps(bOut, bIn);
#elif HV_SIMD_SSE
  _mm_store_ps(bOut, bIn);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  hv_memcpy(bOut, (const void *) &bIn, sizeof(hv_bufferf_t));
#endif
}

static inline void sLog_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn > 0.0f) ? hv_log_f(bIn) : 0.0f;
#endif
}

static inline void sLog10_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn > 0.0f) ? hv_log10_f(bIn) : 0.0f;
#endif
}

static inline void sLog2_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn > 0.0f) ? hv_log2_f(bIn) : 0.0f;
#endif
}

static inline void sCos_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_cos_f(bIn);
#endif
}

static inline void sAcos_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_acos_f(bIn);
#endif
}

static inline void sCosh_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_cosh_f(bIn);
#endif
}

static inline void sAcosh_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_acosh_f(bIn);
#endif
}

static inline void sSin_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_sin_f(bIn);
#endif
}

static inline void sAsin_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_asin_f(bIn);
#endif
}

static inline void sSinh_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_sinh_f(bIn);
#endif
}

static inline void sAsinh_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_asinh_f(bIn);
#endif
}

static inline void sTan_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_tan_f(bIn);
#endif
}

static inline void sAtan_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_atan_f(bIn);
#endif
}

static inline void sAtan2_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_atan2_f(bIn0, bIn1);
#endif
}

static inline void sTanh_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_tanh_f(bIn);
#endif
}

static inline void sAtanh_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_atanh_f(bIn);
#endif
}

static inline void sSqrt_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
  static const __m128 zero = (__m128) {0.0f, 0.0f, 0.0f, 0.0f};
  __m128 mask = _mm_cmpgt_ps(bIn, zero); // b > 0, ensure that all values are positive
  *bOut = _mm_sqrt_ps(_mm_and_ps(mask, bIn));
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn > 0.0f) ? hv_sqrt_f(bIn) : 0.0f;
#endif
}

static inline void __hv_abs_f(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  static const __m256 mask = (__m256) {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f};
  *bOut = _mm256_andnot_ps(mask, bIn);
#elif HV_SIMD_SSE
  static const __m128 mask = (__m128) {-0.0f, -0.0f, -0.0f, -0.0f}; // == 1 << 31
  *bOut = _mm_andnot_ps(mask, bIn);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_abs_f(bIn);
#endif
}

static inline void sExp_process(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_exp_f(bIn);
#endif
}

static inline void __hv_ceil_f(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_ceil_ps(bIn);
#elif HV_SIMD_SSE
  *bOut = _mm_ceil_ps(bIn);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_ceil_f(bIn);
#endif
}

static inline void __hv_floor_f(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_floor_ps(bIn);
#elif HV_SIMD_SSE
  *bOut = _mm_floor_ps(bIn);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_floor_f(bIn);
#endif
}

// __add~f
static inline void __hv_add_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_add_ps(bIn0, bIn1);
#elif HV_SIMD_SSE
  *bOut = _mm_add_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
  *bOut = vaddq_f32(bIn0, bIn1);
#else // HV_SIMD_NONE
  *bOut = bIn0 + bIn1;
#endif
}

// __add~i
static inline void __hv_add_i(hv_bIni_t bIn0, hv_bIni_t bIn1, hv_bOuti_t bOut) {
#if HV_SIMD_AVX
  __m128i x = _mm_add_epi32(_mm256_castsi256_si128(bIn0), _mm256_castsi256_si128(bIn1));
  __m128i y = _mm_add_epi32(_mm256_extractf128_si256(bIn0, 1), _mm256_extractf128_si256(bIn1, 1));
  *bOut = _mm256_insertf128_si256(_mm256_castsi128_si256(x), y, 1);
#elif HV_SIMD_SSE
  *bOut = _mm_add_epi32(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = bIn0 + bIn1;
#endif
}

static inline void __hv_sub_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_sub_ps(bIn0, bIn1);
#elif HV_SIMD_SSE
  *bOut = _mm_sub_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = bIn0 - bIn1;
#endif
}

// __*~f
static inline void __hv_mul_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_mul_ps(bIn0, bIn1);
#elif HV_SIMD_SSE
  *bOut = _mm_mul_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = bIn0 * bIn1;
#endif
}

// __*~i
static inline void __hv_mul_i(hv_bIni_t bIn0, hv_bIni_t bIn1, hv_bOuti_t bOut) {
#if HV_SIMD_AVX
  __m128i x = _mm_mullo_epi32(_mm256_castsi256_si128(bIn0), _mm256_castsi256_si128(bIn1));
  __m128i y = _mm_mullo_epi32(_mm256_extractf128_si256(bIn0, 1), _mm256_extractf128_si256(bIn1, 1));
  *bOut = _mm256_insertf128_si256(_mm256_castsi128_si256(x), y, 1);
#elif HV_SIMD_SSE
  *bOut = _mm_mullo_epi32(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = bIn0 * bIn1;
#endif
}

// __cast~if
static inline void __hv_cast_if(hv_bIni_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_cvtepi32_ps(bIn);
#elif HV_SIMD_SSE
  *bOut = _mm_cvtepi32_ps(bIn);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (float) bIn;
#endif
}

// __cast~fi
static inline void __hv_cast_fi(hv_bInf_t bIn, hv_bOuti_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_cvtps_epi32(bIn);
#elif HV_SIMD_SSE
  *bOut = _mm_cvtps_epi32(bIn);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (int) bIn;
#endif
}

static inline void sDivide_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_div_ps(bIn0, bIn1);
#elif HV_SIMD_SSE
  *bOut = _mm_div_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn1 != 0.0f) ? (bIn0 / bIn1) : 0.0f;
#endif
}

static inline void __hv_min_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_min_ps(bIn0, bIn1);
#elif HV_SIMD_SSE
  *bOut = _mm_min_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_min_f(bIn0, bIn1);
#endif
}

static inline void __hv_max_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_max_ps(bIn0, bIn1);
#elif HV_SIMD_SSE
  *bOut = _mm_max_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_max_f(bIn0, bIn1);
#endif
}

static inline void sPow_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = hv_pow_f(bIn0, bIn1);
#endif
}

static inline void __hv_gt_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_GT_OQ);
#elif HV_SIMD_SSE
  *bOut = _mm_cmpgt_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn0 > bIn1) ? 1.0f : 0.0f;
#endif
}

static inline void __hv_gte_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_GE_OQ);
#elif HV_SIMD_SSE
  *bOut = _mm_cmpge_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn0 >= bIn1) ? 1.0f : 0.0f;
#endif
}

static inline void __hv_lt_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_LT_OQ);
#elif HV_SIMD_SSE
  *bOut = _mm_cmplt_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn0 < bIn1) ? 1.0f : 0.0f;
#endif
}

static inline void __hv_lte_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_LE_OQ);
#elif HV_SIMD_SSE
  *bOut = _mm_cmple_ps(bIn0, bIn1);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn0 <= bIn1) ? 1.0f : 0.0f;
#endif
}

static inline void sBitwiseXor_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
//#error todo implement me
#elif HV_SIMD_SSE
//#error todo implement me
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (float) (((int) bIn0) ^ ((int) bIn1));
#endif
}

static inline void __hv_and_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  *bOut = _mm256_and_ps(bIn1, bIn0);
#elif HV_SIMD_SSE
  *bOut = _mm_and_ps(bIn1, bIn0);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (float) (((int) bIn0) & ((int) bIn1));
#endif
}

static inline void sIf_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut0, hv_bOutf_t bOut1) {
#if HV_SIMD_AVX
  *bOut1 = _mm256_and_ps(bIn1, bIn0); // != 0, goes out right outlet
  *bOut0 = _mm256_andnot_ps(bIn1, bIn0); // == 0 goes out left inlet
#elif HV_SIMD_SSE
  *bOut1 = _mm_and_ps(bIn1, bIn0); // != 0, goes out right outlet
  *bOut0 = _mm_andnot_ps(bIn1, bIn0); // == 0 goes out left inlet
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut0 = (bIn1 == 0.0f) ? bIn0 : 0.0f; // false (i.e. zero) goes left
  *bOut1 = (bIn1 != 0.0f) ? bIn0 : 0.0f; // true (i.e. non-zero) goes right
#endif
}

static inline void sFma_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bInf_t bIn2, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  #if __FMA__
    *bOut = _mm256_fmadd_ps(bIn0, bIn1, bIn2);
  #else
    *bOut = _mm256_add_ps(_mm256_mul_ps(bIn0, bIn1), bIn2);
  #endif // __FMA__
#elif HV_SIMD_SSE
  #if __FMA__
    *bOut = _mm_fmadd_ps(bIn0, bIn1, bIn2);
  #else
    *bOut = _mm_add_ps(_mm_mul_ps(bIn0, bIn1), bIn2);
  #endif // __FMA__
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  *bOut = (bIn0 * bIn1) + bIn2;
#endif
}

#endif // _HEAVY_MATH_H_
