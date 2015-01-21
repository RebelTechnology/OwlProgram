/* Copyright 2014 Section6. All Rights Reserved. */

#include "HvTable.h"

hv_size_t hTable_init(HvTable *o, int length) {
  o->length = length;
  o->size = ((length & HV_N_SIMD_MASK) == 0) ? length : (length + HV_N_SIMD) & ~HV_N_SIMD_MASK;
  o->head = 0;
  hv_size_t numBytes = o->size * sizeof(float);
  o->buffer = (float *) hv_malloc(numBytes);
  hv_clear_buffer(o->buffer, o->size);
  return numBytes;
}

hv_size_t hTable_initWithData(HvTable *o, int length, const float *const data) {
  o->length = length;
  o->size = ((length & HV_N_SIMD_MASK) == 0) ? length : (length + HV_N_SIMD) & ~HV_N_SIMD_MASK;
  o->head = 0;
  hv_size_t numBytes = o->size * sizeof(float);
  o->buffer = (float *) hv_malloc(numBytes);
  hv_memset(o->buffer, numBytes);
  hv_memcpy(o->buffer, data, length*sizeof(float));
  return numBytes;
}

hv_size_t hTable_initWithFinalData(HvTable *o, int length, float *data) {
  o->length = length;
  o->size = length;
  o->buffer = data;
  o->head = 0;
  return 0;
}

void hTable_free(HvTable *o) {
  hv_free(o->buffer);
}

int hTable_resize(HvTable *o, hv_uint32_t newLength) {
  // TODO(mhroth): update context with memory allocated by table
  const hv_uint32_t oldBytes = (int) (o->size * sizeof(float));
  const hv_uint32_t newSize = ((newLength & HV_N_SIMD_MASK) == 0) ? newLength : (newLength + HV_N_SIMD) & ~HV_N_SIMD_MASK;
  const hv_uint32_t newBytes = (int) (newSize * sizeof(float));
  float *b = (float *) hv_realloc(o->buffer, newBytes);
  if (newSize > o->size) hv_clear_buffer(b+o->size, newSize-o->size); // clear new parts of the buffer
  if (b != o->buffer) {
    // the buffer has been reallocated, ensure that it is on a 32-byte boundary
    if ((((unsigned long long) b) & 0x10L) == 0) {
      o->buffer = b;
    } else {
      float *c = (float *) hv_malloc(newBytes);
      hv_clear_buffer(c, newLength);
      const hv_size_t min = hv_min_ui(o->length, newLength);
      hv_memcpy(c, b, min * sizeof(float));
      hv_free(b);
      o->buffer = c;
    }
  }
  o->length = newLength;
  o->size = newSize;
  return (int) (newBytes - oldBytes);
}

void hTable_onMessage(HvTable *o, int letIn, const HvMessage *const m) {
  if (msg_compareSymbol(m,0,"resize") && msg_isFloat(m,1) && msg_getFloat(m,1) >= 0.0f) {
    hTable_resize(o, (int) msg_getFloat(m,1));
  }
}
