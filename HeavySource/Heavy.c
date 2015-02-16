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

#include "HvBase.h"

#if !HV_WIN
#pragma mark - Heavy Table
#endif

void hv_table_resize(HvTable *o, hv_uint32_t newLength) {
  if (newLength > 0) {
    hTable_resize(o, newLength);
  }
}

float *hv_table_getBuffer(HvTable *o) {
  return hTable_getBuffer(o);
}

hv_size_t hv_table_getLength(HvTable *o) {
  return hTable_getLength(o);
}



#if !HV_WIN
#pragma mark - Heavy Message
#endif

hv_size_t hv_msg_getByteSize (hv_uint32_t numElements) {
  return msg_getByteSize(numElements);
}

void hv_msg_init(HvMessage *m, int numElements, hv_uint32_t timestamp) {
  msg_init(m, numElements, timestamp);
}

hv_size_t hv_msg_getNumElements(HvMessage *m) {
  return msg_getNumElements(m);
}

double hv_msg_getTimestamp(HvMessage *m) {
  return msg_getTimestamp(m);
}

void hv_msg_setTimestamp(HvMessage *m, hv_uint32_t timestamp) {
  msg_setTimestamp(m, timestamp);
}

bool hv_msg_isBang(HvMessage *m, int i) {
  return msg_isBang(m,i);
}

void hv_msg_setBang(HvMessage *m, int i) {
  msg_setBang(m,i);
}

bool hv_msg_isFloat(HvMessage *m, int i) {
  return msg_isFloat(m, i);
}

float hv_msg_getFloat(HvMessage *m, int i) {
  return msg_getFloat(m,i);
}

void hv_msg_setFloat(HvMessage *m, int i, float f) {
  msg_setFloat(m,i,f);
}

bool hv_msg_isSymbol(HvMessage *m, int i) {
  return msg_isSymbol(m,i);
}

char *hv_msg_getSymbol(HvMessage *m, int i) {
  return msg_getSymbol(m,i);
}

void hv_msg_setSymbol(HvMessage *m, int i, char *s) {
  msg_setSymbol(m,i,s);
}

bool hv_msg_hasFormat(HvMessage *m, const char *fmt) {
  return msg_hasFormat(m, fmt);
}

char *hv_msg_toString(HvMessage *m) {
  return msg_toString(m);
}

HvMessage *hv_msg_copy(HvMessage *m) {
  return msg_copy(m);
}

void hv_msg_free(HvMessage *m) {
  msg_free(m);
}



#if !HV_WIN
#pragma mark - Heavy Common
#endif

double hv_getSampleRate(HvBase *c) {
  return ctx_getSampleRate(c);
}

int hv_getNumInputChannels(HvBase *c) {
  return ctx_getNumInputChannels(c);
}

int hv_getNumOutputChannels(HvBase *c) {
  return ctx_getNumOutputChannels(c);
}

void hv_setPrintHook(HvBase *c, void (*f)(double, const char *, const char *, void *)) {
  ctx_setPrintHook(c, f);
}

void hv_setSendHook(HvBase *c, void (*f)(const char *, HvMessage *, void *)) {
  ctx_setSendHook(c, f);
}

void hv_vscheduleMessageForReceiver(HvBase *c, const char *receiverName, const double delayMs, const char *format, ...) {
  va_list ap;
  va_start(ap, format);

  const int numElem = (int) hv_strlen(format);
  HvMessage *m = HV_MESSAGE_ON_STACK(numElem);
  msg_init(m, numElem, c->blockStartTimestamp + (hv_uint32_t) (hv_max_d(0.0, delayMs)*ctx_getSampleRate(c)/1000.0));
  for (int i = 0; i < numElem; i++) {
    switch (format[i]) {
      case 'b': msg_setBang(m,i); break;
      case 'f': msg_setFloat(m, i, (float) va_arg(ap, double)); break;
      case 's': msg_setSymbol(m, i, (char *) va_arg(ap, char *)); break;
      default: break;
    }
  }
  ctx_scheduleMessageForReceiver(c, receiverName, m);

  va_end(ap);
}

void hv_scheduleMessageForReceiver(HvBase *c, const char *receiverName, double delayMs, HvMessage *m) {
  msg_setTimestamp(m, c->blockStartTimestamp + (hv_uint32_t) (hv_max_d(0.0, delayMs)*ctx_getSampleRate(c)/1000.0));
  ctx_scheduleMessageForReceiver(c, receiverName, m);
}

HvTable *hv_getTableForName(HvBase *c, const char *tableName) {
  return ctx_getTableForName(c, tableName);
}

void hv_cancelMessage(HvBase *c, HvMessage *m) {
  ctx_cancelMessage(c, m);
}

double hv_getCurrentTime(HvBase *c) {
  return ((double) c->blockStartTimestamp)/c->sampleRate;
}

void *hv_getUserData(HvBase *c) {
  return ctx_getUserData(c);
}

void hv_setUserData(HvBase *c, void *userData) {
  ctx_setUserData(c, userData);
}

void hv_setBasePath(HvBase *c, const char *basePath) {
  ctx_setBasePath(c, basePath);
}
