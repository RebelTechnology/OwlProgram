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

#include "MessagePool.h"
#include "HvMessage.h"
#include "Utils.h"

// the number of bytes reserved at a time from the pool
#define MP_BLOCK_SIZE_BYTES 512

#if HV_APPLE
#pragma mark - MessageList
#endif

typedef struct MessageListNode {
  char *p;
  struct MessageListNode *next;
} MessageListNode;

static inline bool ml_hasAvailable(MessagePoolList *ml) {
  return (ml->head != NULL);
}

static char *ml_pop(MessagePoolList *ml) {
  MessageListNode *n = ml->head;
  ml->head = n->next;
  n->next = ml->pool;
  ml->pool = n;
  char *const p = n->p;
  n->p = NULL; // set to NULL to make it clear that this node does not have a valid buffer
  return p;
}

/** Push a MessageListNode with the given pointer onto the head of the queue. */
static void ml_push(MessagePoolList *ml, void *p) {
  MessageListNode *n = NULL;
  if (ml->pool != NULL) {
    // take an empty MessageListNode from the pool
    n = ml->pool;
    ml->pool = n->next;
  } else {
    // a MessageListNode is not available, allocate one
    n = (MessageListNode *) hv_malloc(sizeof(MessageListNode));
  }
  n->p = (char *) p;
  n->next = ml->head;
  ml->head = n; // push to the front of the queue
}

static void ml_free(MessagePoolList *ml) {
  if (ml != NULL) {
    while (ml_hasAvailable(ml)) {
      ml_pop(ml);
    }
    while (ml->pool != NULL) {
      MessageListNode *n = ml->pool;
      ml->pool = n->next;
      hv_free(n);
    }
  }
}

#if HV_APPLE
#pragma mark - MessagePool
#endif

static hv_size_t mp_messagelistIndexForSize(hv_size_t byteSize) {
  return (hv_size_t) hv_max_i((hv_min_max_log2((hv_uint32_t) byteSize) - 5), 0);
}

hv_size_t mp_init(MessagePool *mp, hv_size_t numKB) {
  mp->bufferSize = numKB * 1024;
  mp->buffer = (char *) hv_malloc(mp->bufferSize);
  mp->bufferIndex = 0;

  // initialise all message lists
  for (int i = 0; i < MP_NUM_MESSAGE_LISTS; i++) {
    mp->lists[i].head = NULL;
    mp->lists[i].pool = NULL;
  }

  return mp->bufferSize;
}

void mp_free(MessagePool *mp) {
  hv_free(mp->buffer);
  for (int i = 0; i < MP_NUM_MESSAGE_LISTS; i++) {
    ml_free(&mp->lists[i]);
  }
}

void mp_freeMessage(MessagePool *mp, HvMessage *m) {
  const hv_size_t b = msg_getNumBytes(m); // the number of bytes that a message occupies in memory
  const hv_size_t i = mp_messagelistIndexForSize(b); // the MessagePoolList index in the pool
  MessagePoolList *ml = &mp->lists[i];
  const hv_size_t chunkSize = 32 << i;
  hv_memset(m, chunkSize); // clear the chunk, just in case
  ml_push(ml, m);
}

HvMessage *mp_addMessage(MessagePool *mp, const HvMessage *m) {
  const hv_size_t b = msg_getNumHeapBytes(m);
  // determine the message list index to allocate data from based on the msg size
  // smallest chunk size is 32 bytes
  const hv_size_t i = mp_messagelistIndexForSize(b);

  assert(i < MP_NUM_MESSAGE_LISTS); // how many chunk sizes do we want to support? 32, 64, 128, 256 at the moment
  MessagePoolList *ml = &mp->lists[i];
  const hv_size_t chunkSize = 32 << i;

  if (ml_hasAvailable(ml)) {
    char *buf = ml_pop(ml);
    msg_copyToBuffer(m, buf, chunkSize);
    return (HvMessage *) buf;
  } else {
    // if no appropriately sized buffer is immediately available, increase the size of the used buffer
    const hv_size_t newIndex = mp->bufferIndex + MP_BLOCK_SIZE_BYTES;
    hv_assert(newIndex <= mp->bufferSize); // have we have exceeded the buffer size?

    for (hv_size_t i = mp->bufferIndex; i < newIndex; i += chunkSize) {
      ml_push(ml, mp->buffer + i); // push new nodes onto the list with chunk pointers
    }
    mp->bufferIndex = newIndex;
    char *buf = ml_pop(ml);
    msg_copyToBuffer(m, buf, chunkSize);
    return (HvMessage *) buf;
  }
}
