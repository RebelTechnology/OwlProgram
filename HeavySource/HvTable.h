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
