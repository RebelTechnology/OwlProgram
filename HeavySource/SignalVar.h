/* Copyright 2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_SIGNAL_VAR_H_
#define _HEAVY_SIGNAL_VAR_H_

#include "HvBase.h"

// __var~f
// __varset~f

typedef struct SignalVarf {
  hv_bufferf_t v;
} SignalVarf;

hv_size_t sVarf_init(SignalVarf *o, float k, float step, bool reverse);

static inline void sVarf_process(SignalVarf *o, hv_bOutf_t bOut) {
#if __AVX__
  *bOut = o->v;
#elif __SSE__
  *bOut = o->v;
#else
  *bOut = o->v; // TODO(mhroth): does this work?
#endif
}

static inline void __hv_var_k_f(hv_bOutf_t bOut, const float k) {
#if __AVX__
  *bOut = _mm256_set1_ps(k);
#elif __SSE__
  *bOut = _mm_set1_ps(k);
#else
#error // TODO(mhroth): implement this
#endif
}

static inline void sVarsetf_process(SignalVarf *o, hv_bInf_t bIn) {
#if __AVX__
  o->v = bIn;
#elif __SSE__
  o->v = bIn;
#else
  o->v = *bIn; // TODO(mhroth): does this work?
#endif
}

void sVarf_onMessage(HvBase *_c, SignalVarf *o, const HvMessage *m);



// __var~i
// __varset~i

typedef struct SignalVari {
  hv_bufferi_t v;
} SignalVari;

hv_size_t sVari_init(SignalVari *o, int k, int step, bool reverse);

static inline void sVari_process(SignalVari *o, hv_bOuti_t bOut) {
#if __AVX__
  *bOut = o->v;
#elif __SSE__
  *bOut = o->v;
#else
  *bOut = o->v;
#endif
}

#if __AVX__
// forwards direction _r==0, backwards (reverse) direction _r==1
#define __hv_var_ks_i_0(_b,_k,_s) _mm256_set_epi32(7*_s+_k, 6*_s+_k, 5*_s+_k, 4*_s+_k, 3*_s+_k, 2*_s+_k, _s+_k, _k)
#define __hv_var_ks_i_1(_b,_k,_s) _mm256_setr_epi32(7*_s+_k, 6*_s+_k, 5*_s+_k, 4*_s+_k, 3*_s+_k, 2*_s+_k, _s+_k, _k)
#elif __SSE__
#define __hv_var_ks_i_0(_b,_k,_s) _mm_set_epi32(3*_s+_k, 2*_s+_k, _s+_k, _k)
#define __hv_var_ks_i_1(_b,_k,_s) _mm_setr_epi32(3*_s+_k, 2*_s+_k, _s+_k, _k)
#endif
#define __hv_var_ks_i(_b,_k,_s,_r) *_b=__hv_var_ks_i_##_r(_b,_k,_s)

#if __AVX__
#define __hv_var_ks_f_0(_b,_k,_s) _mm256_set_ps(7.0f*_s+_k, 6.0f*_s+_k, 5.0f*_s+_k, 4.0f*_s+_k, 3.0f*_s+_k, 2.0f*_s+_k, _s+_k, _k)
#define __hv_var_ks_f_1(_b,_k,_s) _mm256_setr_ps(7.0f*_s+_k, 6.0f*_s+_k, 5.0f*_s+_k, 4.0f*_s+_k, 3.0f*_s+_k, 2.0f*_s+_k, _s+_k, _k)
#elif __SSE__
#define __hv_var_ks_f_0(_b,_k,_s) _mm_set_ps(3.0f*_s+_k, 2.0f*_s+_k, _s+_k, _k)
#define __hv_var_ks_f_1(_b,_k,_s) _mm_setr_ps(3.0f*_s+_k, 2.0f*_s+_k, _s+_k, _k)
#endif
#define __hv_var_ks_f(_b,_k,_s,_r) *_b=__hv_var_ks_f_##_r(_b,_k,_s)

static inline void __hv_var_k_i(hv_bOuti_t bOut, const int k) {
#if __AVX__
  *bOut = _mm256_set1_epi32(k);
#elif __SSE__
  *bOut = _mm_set1_epi32(k);
#else
#error // TODO(mhroth): implement this
#endif
}

static inline void sVarseti_process(SignalVari *o, hv_bIni_t bIn) {
#if __AVX__
  o->v = bIn;
#elif __SSE__
  o->v = bIn;
#else
  o->v = *bIn;
#endif
}

void sVari_onMessage(HvBase *_c, SignalVari *o, const HvMessage *m);

#endif // _HEAVY_SIGNAL_VAR_H_
