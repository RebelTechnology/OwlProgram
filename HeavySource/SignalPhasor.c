/* Copyright 2014 Section6. All Rights Reserved. */

#include "SignalPhasor.h"
#include <stdio.h>

static void sPhasor_updateFrequency(SignalPhasor *o, float f, double r) {
  o->step.s = (int) (f*(4294967295.0/r));
#if __AVX__
  o->inc = _mm_set1_epi32(4*o->step.s);
#elif __SSE__
  o->inc = _mm_set1_epi32(4*o->step.s);
#else
  o->inc.a = 0;
  o->inc.b = o->step.s;
  o->inc.c = 2*o->step.s;
  o->inc.d = 3*o->step.s;
#endif
}

static void sPhasor_updatePhase(SignalPhasor *o, float phase) {
  while (phase < 0.0f) phase += 1.0f; // wrap phase to [0,1]
  while (phase > 1.0f) phase -= 1.0f;
  int p = (int) (phase * 4294967295.0);
#if __AVX__
  o->phase = _mm_set_epi32(3*o->step.s+p, 2*o->step.s+p, o->step.s+p, p);
#elif __SSE__
  o->phase = _mm_set_epi32(3*o->step.s+p, 2*o->step.s+p, o->step.s+p, p);
#else
  o->inc.a = 0;
  o->inc.b = o->step.s;
  o->inc.c = 2*o->step.s;
  o->inc.d = 3*o->step.s;
#endif
}

hv_size_t sPhasor_init(SignalPhasor *o, double samplerate) {
#if __AVX__
  o->phase = _mm_set1_epi32(0);
  o->inc = _mm_set1_epi32(0);
#elif __SSE__
  o->phase = _mm_set1_epi32(0);
  o->inc = _mm_set1_epi32(0);
#else
#error
#endif
  o->step.f2sc = (float) (4294967295.0/samplerate);
  return 0;
}

void sPhasor_onMessage(HvBase *_c, SignalPhasor *o, int letIn, const HvMessage *m) {
  if (msg_isFloat(m,0)) {
    float phase = msg_getFloat(m,0);
    while (phase < 0.0f) phase += 1.0f; // wrap phase to [0,1]
    while (phase > 1.0f) phase -= 1.0f;
    int p = (int) (phase * 4294967295.0);
#if __AVX__
    o->phase = _mm_set1_epi32(p);
#elif __SSE__
    o->phase = _mm_set1_epi32(p);
#else
#error
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
