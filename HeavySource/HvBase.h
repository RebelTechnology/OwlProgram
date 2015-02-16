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

#ifndef _HEAVY_BASE_H_
#define _HEAVY_BASE_H_

#include "HvTable.h"
#include "MessageQueue.h"
#include "Utils.h"

#define Base(_x) ((HvBase *) _x)

typedef struct HvBase {
  int numInputChannels;
  int numOutputChannels;
  double sampleRate;
  hv_uint32_t blockStartTimestamp;
  unsigned int numBytes; // the total number of bytes allocated for this patch
  void (*f_scheduleMessageForReceiver)(struct HvBase *const, const char *, HvMessage *);
  struct HvTable *(*f_getTableForHash)(struct HvBase *const, hv_uint32_t);
  MessageQueue mq;
  void (*printHook)(double, const char *, const char *, void *);
  void (*sendHook)(const char *, HvMessage *, void *);
  char *basePath;
  void *userData;
} HvBase;

/**
 * Schedule a message in the message queue according to its timestamp.
 * The copy of the message added to the queue is returned.
 */
static inline HvMessage *ctx_scheduleMessage(HvBase *_c, const HvMessage *const m,
    void (*sendMessage)(HvBase *, int, const HvMessage *), int outletIndex) {
  return mq_addMessageByTimestamp(&_c->mq, (HvMessage *) m, outletIndex, sendMessage);
}

static inline void ctx_scheduleMessageForReceiver(HvBase *const _c,
    const char *name, HvMessage *m) {
  _c->f_scheduleMessageForReceiver(_c, name, m);
}

void ctx_scheduleMessageForReceiverV(HvBase *const _c, const char *name,
    const hv_uint32_t timestamp, const char *format, ...);

void ctx_cancelMessage(HvBase *_c, HvMessage *m);

static inline int ctx_millisecondsToSamples(HvBase *_c, float timeInMs) {
  return (int) (timeInMs * _c->sampleRate / 1000.0);
}

static inline double ctx_samplesToMilliseconds(HvBase *_c, int samples) {
  return 1000.0 * samples / _c->sampleRate;
}

static inline double ctx_getSampleRate(HvBase *_c) {
  return _c->sampleRate;
}

static inline int ctx_getNumInputChannels(HvBase *_c) {
  return _c->numInputChannels;
}

static inline int ctx_getNumOutputChannels(HvBase *_c) {
  return _c->numOutputChannels;
}

/** Returns the first sample of the block. */
static inline hv_uint32_t ctx_getBlockStartTimestamp(HvBase *_c) {
  return _c->blockStartTimestamp;
}

static inline void ctx_setPrintHook(HvBase *const _c, void (*f)(double,
    const char *, const char *, void *)) {
  _c->printHook = f;
}

static inline void ctx_setSendHook(HvBase *const _c, void (*f)(const char *, HvMessage *m, void *)) {
  _c->sendHook = f;
}

static inline void *ctx_getUserData(HvBase *const _c) {
  return _c->userData;
}

static inline void ctx_setUserData(HvBase *const _c, void *userData) {
  _c->userData = userData;
}

void ctx_setBasePath(HvBase *const _c, const char *basePath);

static inline const char *ctx_getBasePath(HvBase *const _c) {
  return _c->basePath;
}

static inline struct HvTable *ctx_getTableForHash(HvBase *const _c, hv_uint32_t h) {
  return _c->f_getTableForHash(_c, h);
}

static inline struct HvTable *ctx_getTableForName(HvBase *const _c, const char *tableName) {
  return ctx_getTableForHash(_c, msg_symbolToHash(tableName));
}

/** Returns the total number of bytes allocated for this patch. */
static inline unsigned int ctx_getNumBytes(HvBase *_c) {
  return _c->numBytes;
}

#endif // _HEAVY_BASE_H_
