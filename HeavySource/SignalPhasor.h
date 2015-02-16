/**
 * Copyright (c) 2014,2015 Enzien Audio, Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, and/or
 * sublicense copies of the Software, strictly on a non-commercial basis,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef _HEAVY_SIGNAL_PHASOR_H_
#define _HEAVY_SIGNAL_PHASOR_H_

#include "HvBase.h"

// The reciprocal of the maximum value represented by a 32-bit floating point
// number’s mantissa - used to scale the wrap around point in the phasor
#define __HV_PHASOR_SCALE 0.0000001192093f // ((2^23)-1)^-1

typedef struct SignalPhasor {
  union {
    float f2sc; // float to step conversion (used for __phasor~f)
    hv_int32_t s; // step value (used for __phasor_k~f)
  } step;
#if HV_SIMD_AVX || HV_SIMD_SSE
  __m128i phase; // current phase
  __m128i inc; // phase increment
#elif HV_SIMD_NEON
#error // TODO(mhroth): implement me!
#else // HV_SIMD_NONE
  hv_uint32_t phase;
  hv_int32_t inc;
#endif
} SignalPhasor;

hv_size_t sPhasor_init(SignalPhasor *o, double samplerate);

hv_size_t sPhasor_k_init(SignalPhasor *o, float frequency, double samplerate);

void sPhasor_k_onMessage(HvBase *_c, SignalPhasor *o, int letIn, const HvMessage *m);

void sPhasor_onMessage(HvBase *_c, SignalPhasor *o, int letIn, const HvMessage *m);

static inline void __hv_phasor_f(SignalPhasor *o, hv_bInf_t bIn, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  static const __m256 c = (__m256) {
      __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE,
      __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE };
  // convert frequency to step
  __m256i s256 = _mm256_cvtps_epi32(_mm256_mul_ps(bIn, _mm256_set1_ps(o->step.f2sc)));

  __m128i s = _mm256_extractf128_si256(s256, 0); // lower vector
  __m128i s1 = _mm_srli_si128(s, 4);
  __m128i s2 = _mm_srli_si128(s, 8);
  __m128i s3 = _mm_srli_si128(s, 12);
  s = _mm_add_epi32(s, s1);
  __m128i _s = _mm_add_epi32(s2, s3);
  __m128i p = _mm_add_epi32(o->phase, s);
  p = _mm_add_epi32(p, _s);

  __m128i q = _mm_set1_epi32((int) p[3]);
  s = _mm256_extractf128_si256(s256, 1); // upper vector
  s1 = _mm_srli_si128(s, 4);
  s2 = _mm_srli_si128(s, 8);
  s3 = _mm_srli_si128(s, 12);
  s = _mm_add_epi32(s, s1);
  _s = _mm_add_epi32(s2, s3);
  q = _mm_add_epi32(q, s);
  q = _mm_add_epi32(q, _s);

  o->phase = _mm_set1_epi32((int) q[3]);

  p = _mm_srli_epi32(p, 9);
  q = _mm_srli_epi32(q, 9);
  *bOut = _mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_insertf128_si256(_mm256_castsi128_si256(p), q, 1)), c);
#elif HV_SIMD_SSE
  static const __m128 c = (__m128) {
      __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE };
  // convert frequency to step
  __m128i s = _mm_cvtps_epi32(_mm_mul_ps(bIn, _mm_set1_ps(o->step.f2sc)));

  __m128i s1 = _mm_srli_si128(s, 4);
  __m128i s2 = _mm_srli_si128(s, 8);
  __m128i s3 = _mm_srli_si128(s, 12);
  s = _mm_add_epi32(s, s1);
  __m128i _s = _mm_add_epi32(s2, s3);
  __m128i p = _mm_add_epi32(o->phase, s);
  p = _mm_add_epi32(p, _s);

  *bOut = _mm_mul_ps(_mm_cvtepi32_ps(_mm_srli_epi32(p, 9)), c);

  o->phase = _mm_set1_epi32((int) p[3]);
#elif HV_SIMD_NEON
#error // TODO(mhroth): implement this!
#else // HV_SIMD_NONE
  int step = (int) (bIn * o->step.f2sc);
  o->phase += step;
  *bOut = ((float) (o->phase >> 9)) * __HV_PHASOR_SCALE;
#endif
}

static inline void __hv_phasor_k_f(SignalPhasor *o, hv_bOutf_t bOut) {
#if HV_SIMD_AVX
  static const __m256 c = (__m256) {
      __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE,
      __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE };
  __m128i inc = o->inc;
  __m128i phase = o->phase;
  __m128i p = _mm_srli_epi32(phase, 9);
  phase = _mm_add_epi32(phase, inc);
  __m128i q = _mm_srli_epi32(phase, 9);
  o->phase = _mm_add_epi32(phase, inc);
  *bOut = _mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_insertf128_si256(_mm256_castsi128_si256(p), q, 1)), c);
#elif HV_SIMD_SSE
  static const __m128 c = (__m128) {
      __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE, __HV_PHASOR_SCALE };
  __m128i phase = o->phase;
  *bOut = _mm_mul_ps(_mm_cvtepi32_ps(_mm_srli_epi32(phase, 9)), c);
  o->phase = _mm_add_epi32(phase, o->inc);
#elif HV_SIMD_NEON
#error // TODO(mhroth): implement this!
#else // HV_SIMD_NONE
  *bOut = ((float) (o->phase >> 9)) * __HV_PHASOR_SCALE;
  o->phase += o->inc;
#endif
}

#endif // _HEAVY_SIGNAL_PHASOR_H_
