/* Copyright 2014 Section6. All Rights Reserved. */

#include "SignalVar.h"

// __var~f

static void sVarf_update(SignalVarf *o, float k, float step, bool reverse) {
#if __AVX__
  if (reverse) o->v = _mm256_setr_ps(k+7.0f*step, k+6.0f*step, k+5.0f*step, k+4.0f*step, k+3.0f*step, k+2.0f*step, k+step, k);
  else o->v = _mm256_set_ps(k+7.0f*step, k+6.0f*step, k+5.0f*step, k+4.0f*step, k+3.0f*step, k+2.0f*step, k+step, k);
#elif __SSE__
  if (reverse) o->v = _mm_setr_ps(k+3.0f*step, k+2.0f*step, k+step, k);
  else o->v = _mm_set_ps(k+3.0f*step, k+2.0f*step, k+step, k);
#else
#error
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
#if __AVX__
  if (reverse) o->v = _mm256_setr_epi32(k+7*step, k+6*step, k+5*step, k+4*step, k+3*step, k+2*step, k+step, k);
  else o->v = _mm256_set_epi32(k+7*step, k+6*step, k+5*step, k+4*step, k+3*step, k+2*step, k+step, k);
#elif __SSE__
  if (reverse) o->v = _mm_setr_epi32(k+3*step, k+2*step, k+step, k);
  else o->v = _mm_set_epi32(k+3*step, k+2*step, k+step, k);
#else
#error
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
