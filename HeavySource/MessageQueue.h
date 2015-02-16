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

#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include "HvMessage.h"
#include "MessagePool.h"

struct HvBase;

typedef struct MessageNode {
  struct MessageNode *prev; // doubly linked list
  struct MessageNode *next;
  int let;
  HvMessage *m;
  void (*sendMessage)(struct HvBase *, int, const HvMessage *);
} MessageNode;

/** A doubly linked list containing scheduled messages. */
typedef struct MessageQueue {
  MessageNode *head; // the head of the queue
  MessageNode *tail; // the tail of the queue
  MessageNode *pool; // the head of the reserve pool
  MessagePool mp;
} MessageQueue;

hv_size_t mq_init(MessageQueue *q);

void mq_initWithPoolSize(MessageQueue *q, hv_size_t poolSizeKB);

void mq_free(MessageQueue *q);

int mq_size(MessageQueue *q);

static inline HvMessage *mq_node_getMessage(MessageNode *n) {
  return n->m;
}

static inline int mq_node_getLet(MessageNode *n) {
  return n->let;
}

static inline bool mq_hasMessage(MessageQueue *q) {
  return (q->head != NULL);
}

// true if there is a message and it occurs before (<) timestamp
static inline bool mq_hasMessageBefore(MessageQueue *const q, const hv_uint32_t timestamp) {
  return mq_hasMessage(q) && (msg_getTimestamp(mq_node_getMessage(q->head)) < timestamp);
}

static inline MessageNode *mq_peek(MessageQueue *q) {
  return q->head;
}

/** Appends the message to the end of the queue. */
HvMessage *mq_addMessage(MessageQueue *q, const HvMessage *m, int let,
    void (*sendMessage)(struct HvBase *, int, const HvMessage *));

/** Insert in ascending order the message acccording to its timestamp. */
HvMessage *mq_addMessageByTimestamp(MessageQueue *q, HvMessage *m, int let,
    void (*sendMessage)(struct HvBase *, int, const HvMessage *));

/** Pop the message at the head of the queue (and free its memory). */
void mq_pop(MessageQueue *q);

/** Remove a message from the queue (and free its memory) */
void mq_removeMessage(MessageQueue *q, HvMessage *m);

/** Clears (and frees) all messages in the queue. */
void mq_clear(MessageQueue *q);

/** Removes all messages occuring at or after the given timestamp. */
void mq_clearAfter(MessageQueue *q, const double timestamp);

#endif // _MESSAGE_QUEUE_H_
