/** Copyright 2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_MATH_H_
#define _HEAVY_MATH_H_

#include <math.h>
#include "Utils.h"

// https://software.intel.com/sites/landingpage/IntrinsicsGuide/

// http://stackoverflow.com/questions/11228855/header-files-for-simd-intrinsics
#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
/* GCC-compatible compiler, targeting ARM with WMMX */
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
/* GCC-compatible compiler, targeting PowerPC with SPE */
#include <spe.h>
#endif

static inline void sZero_process(hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_setzero_ps();
#elif __SSE__
  *bOut = _mm_setzero_ps();
#else
  memset(bOut, 0, HV_N_SIMD*sizeof(float));
#endif
}

static inline void sLog_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = (bIn[0] > 0.0f) ? logf(bIn[0]) : 0.0f;
    bOut[1] = (bIn[1] > 0.0f) ? logf(bIn[1]) : 0.0f;
    bOut[2] = (bIn[2] > 0.0f) ? logf(bIn[2]) : 0.0f;
    bOut[3] = (bIn[3] > 0.0f) ? logf(bIn[3]) : 0.0f;
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sLog10_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = (bIn[0] > 0.0f) ? log10f(bIn[0]) : 0.0f;
    bOut[1] = (bIn[1] > 0.0f) ? log10f(bIn[1]) : 0.0f;
    bOut[2] = (bIn[2] > 0.0f) ? log10f(bIn[2]) : 0.0f;
    bOut[3] = (bIn[3] > 0.0f) ? log10f(bIn[3]) : 0.0f;
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sLog2_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = (bIn[0] > 0.0f) ? log2f(bIn[0]) : 0.0f;
    bOut[1] = (bIn[1] > 0.0f) ? log2f(bIn[1]) : 0.0f;
    bOut[2] = (bIn[2] > 0.0f) ? log2f(bIn[2]) : 0.0f;
    bOut[3] = (bIn[3] > 0.0f) ? log2f(bIn[3]) : 0.0f;
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sCos_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = cosf(bIn[0]);
    bOut[1] = cosf(bIn[1]);
    bOut[2] = cosf(bIn[2]);
    bOut[3] = cosf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sAcos_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = acosf(bIn[0]);
    bOut[1] = acosf(bIn[1]);
    bOut[2] = acosf(bIn[2]);
    bOut[3] = acosf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sCosh_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = coshf(bIn[0]);
    bOut[1] = coshf(bIn[1]);
    bOut[2] = coshf(bIn[2]);
    bOut[3] = coshf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sAcosh_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = acoshf(bIn[0]);
    bOut[1] = acoshf(bIn[1]);
    bOut[2] = acoshf(bIn[2]);
    bOut[3] = acoshf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sSin_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = sinf(bIn[0]);
    bOut[1] = sinf(bIn[1]);
    bOut[2] = sinf(bIn[2]);
    bOut[3] = sinf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sAsin_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = asinf(bIn[0]);
    bOut[1] = asinf(bIn[1]);
    bOut[2] = asinf(bIn[2]);
    bOut[3] = asinf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sSinh_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = sinhf(bIn[0]);
    bOut[1] = sinhf(bIn[1]);
    bOut[2] = sinhf(bIn[2]);
    bOut[3] = sinhf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sAsinh_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = asinhf(bIn[0]);
    bOut[1] = asinhf(bIn[1]);
    bOut[2] = asinhf(bIn[2]);
    bOut[3] = asinhf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sTan_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = tanf(bIn[0]);
    bOut[1] = tanf(bIn[1]);
    bOut[2] = tanf(bIn[2]);
    bOut[3] = tanf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sAtan_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = atanf(bIn[0]);
    bOut[1] = atanf(bIn[1]);
    bOut[2] = atanf(bIn[2]);
    bOut[3] = atanf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sAtan2_process(float *bIn0, float *bIn1, float *bOut, int n4) {
  while (n4) {
    bOut[0] = atan2f(bIn0[0], bIn1[0]);
    bOut[1] = atan2f(bIn0[1], bIn1[1]);
    bOut[2] = atan2f(bIn0[2], bIn1[2]);
    bOut[3] = atan2f(bIn0[3], bIn1[3]);
    n4 -= 4; bIn0 += 4; bIn1 += 4; bOut += 4;
  }
}

static inline void sTanh_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = tanhf(bIn[0]);
    bOut[1] = tanhf(bIn[1]);
    bOut[2] = tanhf(bIn[2]);
    bOut[3] = tanhf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sAtanh_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = atanhf(bIn[0]);
    bOut[1] = atanhf(bIn[1]);
    bOut[2] = atanhf(bIn[2]);
    bOut[3] = atanhf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void sSqrt_process(float *bIn, float *bOut, int n4) {
#if __SSE__
  static const __m128 zero = (__m128) {0.0f, 0.0f, 0.0f, 0.0f};
#endif
  while (n4) {
#if __SSE__
    __m128 b = _mm_load_ps(bIn);
    __m128 mask = _mm_cmpgt_ps(b, zero); // b > 0, ensure that all values are positive
    _mm_store_ps(bOut, _mm_sqrt_ps(_mm_and_ps(mask, b)));
#else
    bOut[0] = (bIn[0] > 0.0f) ? sqrtf(bIn[0]) : 0.0f;
    bOut[1] = (bIn[1] > 0.0f) ? sqrtf(bIn[1]) : 0.0f;
    bOut[2] = (bIn[2] > 0.0f) ? sqrtf(bIn[2]) : 0.0f;
    bOut[3] = (bIn[3] > 0.0f) ? sqrtf(bIn[3]) : 0.0f;
#endif
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void __hv_abs_f(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if __AVX__
  static const __m256 mask = (__m256) {-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f};
  *bOut = _mm256_andnot_ps(mask, bIn);
#elif __SSE__
  static const __m128 mask = (__m128) {-0.0f, -0.0f, -0.0f, -0.0f}; // == 1 << 31
  *bOut = _mm_andnot_ps(mask, bIn);
#else
  bOut[0] = fabsf(bIn[0]);
  bOut[1] = fabsf(bIn[1]);
  bOut[2] = fabsf(bIn[2]);
  bOut[3] = fabsf(bIn[3]);
#endif
}

static inline void sExp_process(float *bIn, float *bOut, int n4) {
  while (n4) {
    bOut[0] = expf(bIn[0]);
    bOut[1] = expf(bIn[1]);
    bOut[2] = expf(bIn[2]);
    bOut[3] = expf(bIn[3]);
    n4 -= 4; bIn += 4; bOut += 4;
  }
}

static inline void __hv_ceil_f(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_ceil_ps(bIn);
#elif __SSE4_2__
  *bOut = _mm_ceil_ps(bIn);
#else
  bOut[0] = ceilf(bIn[0]);
  bOut[1] = ceilf(bIn[1]);
  bOut[2] = ceilf(bIn[2]);
  bOut[3] = ceilf(bIn[3]);
#endif
}

static inline void __hv_floor_f(hv_bInf_t bIn, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_floor_ps(bIn);
#elif __SSE4_2__
  *bOut = _mm_floor_ps(bIn);
#else
  bOut[0] = floorf(bIn[0]);
  bOut[1] = floorf(bIn[1]);
  bOut[2] = floorf(bIn[2]);
  bOut[3] = floorf(bIn[3]);
#endif
}

// __add~f
static inline void __hv_add_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __ARM_NEON__
  *bOut = vaddq_f32(bIn0, bIn1);
#elif __AVX__
  *bOut = _mm256_add_ps(bIn0, bIn1);
#elif __SSE__
  *bOut = _mm_add_ps(bIn0, bIn1);
#else
  bOut[0] = bIn0[0] + bIn1[0];
  bOut[1] = bIn0[1] + bIn1[1];
  bOut[2] = bIn0[2] + bIn1[2];
  bOut[3] = bIn0[3] + bIn1[3];
#endif
}

// __add~i
static inline void __hv_add_i(hv_bIni_t bIn0, hv_bIni_t bIn1, hv_bOuti_t bOut) {
#if __AVX__
  __m128i x = _mm_add_epi32(_mm256_castsi256_si128(bIn0), _mm256_castsi256_si128(bIn1));
  __m128i y = _mm_add_epi32(_mm256_extractf128_si256(bIn0, 1), _mm256_extractf128_si256(bIn1, 1));
  *bOut = _mm256_insertf128_si256(_mm256_castsi128_si256(x), y, 1);
#elif __SSE__
  *bOut = _mm_add_epi32(bIn0, bIn1);
#else
  bOut[0] = bIn0[0] * bIn1[0];
  bOut[1] = bIn0[1] * bIn1[1];
  bOut[2] = bIn0[2] * bIn1[2];
  bOut[3] = bIn0[3] * bIn1[3];
#endif
}

static inline void __hv_sub_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_sub_ps(bIn0, bIn1);
#elif __SSE__
  *bOut = _mm_sub_ps(bIn0, bIn1);
#else
  bOut[0] = bIn0[0] - bIn1[0];
  bOut[1] = bIn0[1] - bIn1[1];
  bOut[2] = bIn0[2] - bIn1[2];
  bOut[3] = bIn0[3] - bIn1[3];
#endif
}

// __*~f
static inline void __hv_mul_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_mul_ps(bIn0, bIn1);
#elif __SSE__
  *bOut = _mm_mul_ps(bIn0, bIn1);
#else
  bOut[0] = bIn0[0] * bIn1[0];
  bOut[1] = bIn0[1] * bIn1[1];
  bOut[2] = bIn0[2] * bIn1[2];
  bOut[3] = bIn0[3] * bIn1[3];
#endif
}

// __*~i
static inline void __hv_mul_i(hv_bIni_t bIn0, hv_bIni_t bIn1, hv_bOuti_t bOut) {
#if __AVX__
  __m128i x = _mm_mullo_epi32(_mm256_castsi256_si128(bIn0), _mm256_castsi256_si128(bIn1));
  __m128i y = _mm_mullo_epi32(_mm256_extractf128_si256(bIn0, 1), _mm256_extractf128_si256(bIn1, 1));
  *bOut = _mm256_insertf128_si256(_mm256_castsi128_si256(x), y, 1);
#elif __SSE__
  *bOut = _mm_mullo_epi32(bIn0, bIn1);
#else
  bOut[0] = bIn0[0] * bIn1[0];
  bOut[1] = bIn0[1] * bIn1[1];
  bOut[2] = bIn0[2] * bIn1[2];
  bOut[3] = bIn0[3] * bIn1[3];
#endif
}

// __cast~if
static inline void __hv_cast_if(hv_bIni_t bIn, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_cvtepi32_ps(bIn);
#elif __SSE__
  *bOut = _mm_cvtepi32_ps(bIn);
#else
  bOut[0] = (float) bIn[0];
  bOut[1] = (float) bIn[1];
  bOut[2] = (float) bIn[2];
  bOut[3] = (float) bIn[3];
#endif
}

// __cast~fi
static inline void __hv_cast_fi(hv_bInf_t bIn, hv_bOuti_t bOut) {
#if __AVX__
  *bOut = _mm256_cvtps_epi32(bIn);
#elif __SSE__
  *bOut = _mm_cvtps_epi32(bIn);
#else
  bOut[0] = (int) bIn[0];
  bOut[1] = (int) bIn[1];
  bOut[2] = (int) bIn[2];
  bOut[3] = (int) bIn[3];
#endif
}

static inline void sDivide_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_div_ps(bIn0, bIn1);
#elif __SSE__
  *bOut = _mm_div_ps(bIn0, bIn1);
#else
  bOut[0] = (bIn1[0] != 0.0f) ? (bIn0[0] / bIn1[0]) : 0.0f;
  bOut[1] = (bIn1[1] != 0.0f) ? (bIn0[1] / bIn1[1]) : 0.0f;
  bOut[2] = (bIn1[2] != 0.0f) ? (bIn0[2] / bIn1[2]) : 0.0f;
  bOut[3] = (bIn1[3] != 0.0f) ? (bIn0[3] / bIn1[3]) : 0.0f;
#endif
}

static inline void __hv_min_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_min_ps(bIn0, bIn1);
#elif __SSE__
  *bOut = _mm_min_ps(bIn0, bIn1);
#else
  bOut[0] = hv_min_f(bIn0[0], bIn1[0]);
  bOut[1] = hv_min_f(bIn0[1], bIn1[1]);
  bOut[2] = hv_min_f(bIn0[2], bIn1[2]);
  bOut[3] = hv_min_f(bIn0[3], bIn1[3]);
#endif
}

static inline void __hv_max_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_max_ps(bIn0, bIn1);
#elif __SSE__
  *bOut = _mm_max_ps(bIn0, bIn1);
#else
  bOut[0] = hv_max_f(bIn0[0], bIn1[0]);
  bOut[1] = hv_max_f(bIn0[1], bIn1[1]);
  bOut[2] = hv_max_f(bIn0[2], bIn1[2]);
  bOut[3] = hv_max_f(bIn0[3], bIn1[3]);
#endif
}

static inline void sPow_process(float *bIn0, float *bIn1, float *bOut, int n4) {
  while (n4) {
    bOut[0] = powf(bIn0[0], bIn1[0]);
    bOut[1] = powf(bIn0[1], bIn1[1]);
    bOut[2] = powf(bIn0[2], bIn1[2]);
    bOut[3] = powf(bIn0[3], bIn1[3]);
    n4 -= 4; bIn0 += 4; bIn1 += 4; bOut += 4;
  }
}

static inline void __hv_gt_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_GT_OQ);
#elif __SSE__
  *bOut = _mm_cmpgt_ps(bIn0, bIn1);
#else
  bOut[0] = (bIn0[0] > bIn1[0]) ? 1.0f : 0.0f;
  bOut[1] = (bIn0[1] > bIn1[1]) ? 1.0f : 0.0f;
  bOut[2] = (bIn0[2] > bIn1[2]) ? 1.0f : 0.0f;
  bOut[3] = (bIn0[3] > bIn1[3]) ? 1.0f : 0.0f;
#endif
}

static inline void __hv_gte_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_GE_OQ);
#elif __SSE__
  *bOut = _mm_cmpge_ps(bIn0, bIn1);
#else
  bOut[0] = (bIn0[0] >= bIn1[0]) ? 1.0f : 0.0f;
  bOut[1] = (bIn0[1] >= bIn1[1]) ? 1.0f : 0.0f;
  bOut[2] = (bIn0[2] >= bIn1[2]) ? 1.0f : 0.0f;
  bOut[3] = (bIn0[3] >= bIn1[3]) ? 1.0f : 0.0f;
#endif
}

static inline void __hv_lt_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_LT_OQ);
#elif __SSE__
  *bOut = _mm_cmplt_ps(bIn0, bIn1);
#else
  bOut[0] = (bIn0[0] < bIn1[0]) ? 1.0f : 0.0f;
  bOut[1] = (bIn0[1] < bIn1[1]) ? 1.0f : 0.0f;
  bOut[2] = (bIn0[2] < bIn1[2]) ? 1.0f : 0.0f;
  bOut[3] = (bIn0[3] < bIn1[3]) ? 1.0f : 0.0f;
#endif
}

static inline void __hv_lte_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_cmp_ps(bIn0, bIn1, _CMP_LE_OQ);
#elif __SSE__
  *bOut = _mm_cmple_ps(bIn0, bIn1);
#else
  bOut[0] = (bIn0[0] <= bIn1[0]) ? 1.0f : 0.0f;
  bOut[1] = (bIn0[1] <= bIn1[1]) ? 1.0f : 0.0f;
  bOut[2] = (bIn0[2] <= bIn1[2]) ? 1.0f : 0.0f;
  bOut[3] = (bIn0[3] <= bIn1[3]) ? 1.0f : 0.0f;
#endif
}

static inline void sBitwiseXor_process(float *bIn0, float *bIn1, float *bOut, int n4) {
  while (n4) {
    bOut[0] = (float) (((int) bIn0[0]) ^ ((int) bIn1[0]));
    bOut[1] = (float) (((int) bIn0[1]) ^ ((int) bIn1[1]));
    bOut[2] = (float) (((int) bIn0[2]) ^ ((int) bIn1[2]));
    bOut[3] = (float) (((int) bIn0[3]) ^ ((int) bIn1[3]));
    n4 -= 4; bIn0 += 4; bIn1 += 4; bOut += 4;
  }
}

static inline void __hv_and_f(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_and_ps(bIn1, bIn0);
#elif __SSE__
  *bOut = _mm_and_ps(bIn1, bIn0);
#else
#error
#endif
}

static inline void sIf_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bOutf_t bOut0, hv_bOutf_t bOut1) {
#if __AVX__
  *bOut1 = _mm256_and_ps(bIn1, bIn0); // != 0, goes out right outlet
  *bOut0 = _mm256_andnot_ps(bIn1, bIn0); // == 0 goes out left inlet
#elif __SSE__
  *bOut1 = _mm_and_ps(bIn1, bIn0); // != 0, goes out right outlet
  *bOut0 = _mm_andnot_ps(bIn1, bIn0); // == 0 goes out left inlet
#else
  for (int i = 0; i < n4; ++i, bIn0+=4, bIn1+=4, bOut0+=4, bOut1+=4) {
    if (bIn1[i] == 0.0f) {
      // false (i.e. zero) goes left
      bOut0[i] = bIn0[i];
      bOut1[i] = 0.0f;
    } else {
      // true (i.e. non-zero) goes right
      bOut0[i] = 0.0f;
      bOut1[i] = bIn0[i];
    }
  }
#endif
}

// bOut = (bIn0 * bIn1) + bIn2
static inline void sFma_process(hv_bInf_t bIn0, hv_bInf_t bIn1, hv_bInf_t bIn2, hv_bOutf_t bOut) {
#if __AVX__
#if __FMA__
  *bOut = _mm256_fmadd_ps(bIn0, bIn1, bIn2);
#else
  *bOut = _mm256_add_ps(_mm256_mul_ps(bIn0, bIn1), bIn2);
#endif // __FMA__
#elif __SSE__
#if __FMA__
  *bOut = _mm_fmadd_ps(bIn0, bIn1, bIn2);
#else
  *bOut = _mm_add_ps(_mm_mul_ps(bIn0, bIn1), bIn2);
#endif // __FMA__
#else
  // TODO(mhroth):
#endif
}

static inline void sLoad_process(float *bIn, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = _mm256_load_ps(bIn);
#elif __SSE__
  *bOut = _mm_load_ps(bIn);
#else
  memcpy(bOut, bIn, sizeof(hv_buffer_t));
#endif
}

static inline void sStore_process(float *bOut, hv_bInf_t bIn) {
#if __AVX__
  _mm256_store_ps(bOut, bIn);
#elif __SSE__
  _mm_store_ps(bOut, bIn);
#else
  memcpy(bOut, bIn, sizeof(hv_buffer_t));
#endif
}

#endif // _HEAVY_MATH_H_
