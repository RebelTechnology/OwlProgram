/* Copyright 2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_TABLE_H_
#define _HEAVY_TABLE_H_

#include "HvMessage.h"
#include "Utils.h"

typedef struct HvTable {
  float *buffer;
  hv_uint32_t length; // the number of values that the table is supposed to have

  // the number of values that the table really has (size >= length)
  hv_uint32_t size; // size is always a multiple of blocksize (HV_N_SIMD)

  hv_uint32_t head; // the most recently written point
} HvTable;

hv_size_t hTable_init(HvTable *o, int length);

hv_size_t hTable_initWithData(HvTable *o, int length, const float *const data);

hv_size_t hTable_initWithFinalData(HvTable *o, int length, float *data);

void hTable_free(HvTable *o);

int hTable_resize(HvTable *o, hv_uint32_t newLength);

void hTable_onMessage(HvTable *o, int letIn, const HvMessage *const m);

// writes a number of samples to the buffer with optional wrap-around
// returns the number of samples *not* written
//int hTable_write(HvTable *o, float *b, hv_size_t len, bool wrap);

static inline float *hTable_getBuffer(HvTable *o) {
  return o->buffer;
}

static inline hv_uint32_t hTable_getLength(HvTable *o) {
  return o->length;
}

static inline hv_uint32_t hTable_getSize(HvTable *o) {
  return o->size;
}

static inline hv_uint32_t hTable_getHead(HvTable *o) {
  return o->head;
}

static inline void hTable_setHead(HvTable *o, hv_uint32_t head) {
  o->head = head;
}

#endif // _HEAVY_TABLE_H_
