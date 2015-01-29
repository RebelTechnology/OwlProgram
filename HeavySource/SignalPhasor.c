/* Copyright 2014 Section6. All Rights Reserved. */

#include "SignalPhasor.h"
#include <stdio.h>

static void sPhasor_updateFrequency(SignalPhasor *o, float f, double r) {
  o->step.s = (int) (f*(4294967295.0/r));
#if HV_SIMD_AVX || HV_SIMD_SSE
  o->inc = _mm_set1_epi32(4*o->step.s);
#elif HV_SIMD_NEON
#error // TODO(mhroth): implement this!
#else // HV_SIMD_NONE
  o->inc = o->step.s;
#endif
}

static void sPhasor_updatePhase(SignalPhasor *o, float phase) {
  while (phase < 0.0f) phase += 1.0f; // wrap phase to [0,1]
  while (phase > 1.0f) phase -= 1.0f;
  hv_uint32_t p = (hv_uint32_t) (phase * 4294967295.0);
#if HV_SIMD_AVX || HV_SIMD_SSE
  o->phase = _mm_set_epi32(3*o->step.s+p, 2*o->step.s+p, o->step.s+p, p);
#elif HV_SIMD_NEON
#error // TODO(mhroth): implement this!
#else // HV_SIMD_NONE
  o->phase = p;
#endif
}

hv_size_t sPhasor_init(SignalPhasor *o, double samplerate) {
#if HV_SIMD_AVX || HV_SIMD_SSE
  o->phase = _mm_set1_epi32(0);
  o->inc = _mm_set1_epi32(0);
#elif HV_SIMD_NEON
#error // TODO(mhroth): implement this!
#else // HV_SIMD_NONE
  o->phase = 0;
  o->inc = 0;
#endif
  o->step.f2sc = (float) (4294967295.0/samplerate);
  return 0;
}

void sPhasor_onMessage(HvBase *_c, SignalPhasor *o, int letIn, const HvMessage *m) {
  if (msg_isFloat(m,0)) {
    float phase = msg_getFloat(m,0);
    while (phase < 0.0f) phase += 1.0f; // wrap phase to [0,1]
    while (phase > 1.0f) phase -= 1.0f;
    hv_uint32_t p = (hv_uint32_t) (phase * 4294967295.0);
#if HV_SIMD_AVX || HV_SIMD_SSE
    o->phase = _mm_set1_epi32(p);
#elif HV_SIMD_NEON
#error // TODO(mhroth): implement this!
#else // HV_SIMD_NONE
    o->phase = p;
#endif
  }
}

hv_size_t sPhasor_k_init(SignalPhasor *o, float frequency, double samplerate) {
  sPhasor_updateFrequency(o, frequency, samplerate);
  sPhasor_updatePhase(o, 0);
  return 0;
}

void sPhasor_k_onMessage(HvBase *_c, SignalPhasor *o, int letIn, const HvMessage *m) {
  if (msg_isFloat(m,0)) {
    switch (letIn) {
      case 0: sPhasor_updateFrequency(o, msg_getFloat(m,0), ctx_getSampleRate(_c)); break;
      case 1: sPhasor_updatePhase(o, msg_getFloat(m,0)); break;
      default: break;
    }
  }
}
