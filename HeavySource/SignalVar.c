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

#include "SignalVar.h"

// __var~f

static void sVarf_update(SignalVarf *o, float k, float step, bool reverse) {
#if HV_SIMD_AVX
  if (reverse) o->v = _mm256_setr_ps(k+7.0f*step, k+6.0f*step, k+5.0f*step, k+4.0f*step, k+3.0f*step, k+2.0f*step, k+step, k);
  else o->v = _mm256_set_ps(k+7.0f*step, k+6.0f*step, k+5.0f*step, k+4.0f*step, k+3.0f*step, k+2.0f*step, k+step, k);
#elif HV_SIMD_SSE
  if (reverse) o->v = _mm_setr_ps(k+3.0f*step, k+2.0f*step, k+step, k);
  else o->v = _mm_set_ps(k+3.0f*step, k+2.0f*step, k+step, k);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NONE
  o->v = k;
#endif
}

hv_size_t sVarf_init(SignalVarf *o, float k, float step, bool reverse) {
  sVarf_update(o, k, step, reverse);
  return 0;
}

void sVarf_onMessage(HvBase *_c, SignalVarf *o, const HvMessage *m) {
  if (msg_isFloat(m,0)) {
    sVarf_update(o, msg_getFloat(m,0), msg_isFloat(m,1) ? msg_getFloat(m,1) : 0.0f, msg_getNumElements(m) == 3);
  }
}



// __var~i

static void sVari_update(SignalVari *o, int k, int step, bool reverse) {
#if HV_SIMD_AVX
  if (reverse) o->v = _mm256_setr_epi32(k+7*step, k+6*step, k+5*step, k+4*step, k+3*step, k+2*step, k+step, k);
  else o->v = _mm256_set_epi32(k+7*step, k+6*step, k+5*step, k+4*step, k+3*step, k+2*step, k+step, k);
#elif HV_SIMD_SSE
  if (reverse) o->v = _mm_setr_epi32(k+3*step, k+2*step, k+step, k);
  else o->v = _mm_set_epi32(k+3*step, k+2*step, k+step, k);
#elif HV_SIMD_NEON
#error todo implement me
#else // HV_SIMD_NEON
  o->v = k;
#endif
}

hv_size_t sVari_init(SignalVari *o, int k, int step, bool reverse) {
  sVari_update(o, k, step, reverse);
  return 0;
}

void sVari_onMessage(HvBase *_c, SignalVari *o, const HvMessage *m) {
  if (msg_isFloat(m,0)) {
    sVari_update(o, (int) msg_getFloat(m,0), msg_isFloat(m,1) ? (int) msg_getFloat(m,1) : 0, msg_getNumElements(m) == 3);
  }
}
