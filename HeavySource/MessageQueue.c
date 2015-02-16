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

#include "MessageQueue.h"
#include "Utils.h"

hv_size_t mq_init(MessageQueue *q) {
  q->head = NULL;
  q->tail = NULL;
  q->pool = NULL;
  return mp_init(&q->mp, 1);
}

void mq_initWithPoolSize(MessageQueue *q, hv_size_t poolSizeKB) {
  q->head = NULL;
  q->tail = NULL;
  q->pool = NULL;
  mp_init(&q->mp, poolSizeKB);
}

void mq_free(MessageQueue *q) {
  mq_clear(q);
  while (q->pool != NULL) {
    MessageNode *n = q->pool;
    q->pool = q->pool->next;
    hv_free(n);
  }
  mp_free(&q->mp);
}

static MessageNode *mq_getOrCreateNodeFromPool(MessageQueue *q) {
  if (q->pool == NULL) {
    // if necessary, create a new empty node
    q->pool = (MessageNode *) hv_malloc(sizeof(MessageNode));
    q->pool->next = NULL;
  }
  MessageNode *node = q->pool;
  q->pool = q->pool->next;
  return node;
}

int mq_size(MessageQueue *q) {
    int size = 0;
    MessageNode *n = q->head;
    while (n != NULL) {
        ++size;
        n = n->next;
    }
    return size;
}

HvMessage *mq_addMessage(MessageQueue *q, const HvMessage *m, int let,
    void (*sendMessage)(struct HvBase *, int, const HvMessage *)) {
  MessageNode *node = mq_getOrCreateNodeFromPool(q);
  node->m = mp_addMessage(&q->mp, m);
  node->let = let;
  node->sendMessage = sendMessage;
  node->prev = NULL;
  node->next = NULL;

  if (q->tail != NULL) {
    // the list already contains elements
    q->tail->next = node;
    node->prev = q->tail;
    q->tail = node;
  } else {
    // the list is empty
    node->prev = NULL;
    q->head = node;
    q->tail = node;
  }
  return mq_node_getMessage(node);
}

HvMessage *mq_addMessageByTimestamp(MessageQueue *q, HvMessage *m, int let,
    void (*sendMessage)(struct HvBase *, int, const HvMessage *)) {
  if (mq_hasMessage(q)) {
    MessageNode *n = mq_getOrCreateNodeFromPool(q);
    n->m = mp_addMessage(&q->mp, m);
    n->let = let;
    n->sendMessage = sendMessage;

    if (msg_getTimestamp(m) < msg_getTimestamp(q->head->m)) {
      // the message occurs before the current head
      n->next = q->head;
      q->head->prev = n;
      n->prev = NULL;
      q->head = n;
    } else if (msg_getTimestamp(m) >= msg_getTimestamp(q->tail->m)) {
      // the message occurs after the current tail
      n->next = NULL;
      n->prev = q->tail;
      q->tail->next = n;
      q->tail = n;
    } else {
      // the message occurs somewhere between the head and tail
      MessageNode *node = q->head;
      while (node != NULL) {
        if (m->timestamp < msg_getTimestamp(node->next->m)) {
          MessageNode *r = node->next;
          node->next = n;
          n->next = r;
          n->prev = node;
          r->prev = n;
          break;
        }
        node = node->next;
      }
    }
    return n->m;
  } else {
    // add a message to the head
    return mq_addMessage(q, m, let, sendMessage);
  }
}

void mq_pop(MessageQueue *q) {
  if (mq_hasMessage(q)) {
    MessageNode *n = q->head;

    mp_freeMessage(&q->mp, n->m);
    n->m = NULL;

    n->let = 0;
    n->sendMessage = NULL;

    q->head = n->next;
    if (q->head == NULL) {
      q->tail = NULL;
    } else {
      q->head->prev = NULL;
    }
    n->next = q->pool;
    n->prev = NULL;
    q->pool = n;
  }
}

void mq_removeMessage(MessageQueue *q, HvMessage *m) {
  if (mq_hasMessage(q)) {
    if (mq_node_getMessage(q->head) == m) { // msg in head node
      mq_pop(q);
    } else {
      MessageNode *prevNode = q->head;
      MessageNode *currNode = q->head->next;
      while ((currNode != NULL) && (currNode->m != m)) {
        prevNode = currNode;
        currNode = currNode->next;
      }
      if (currNode != NULL) {
        mp_freeMessage(&q->mp, m);
        currNode->m = NULL;
        currNode->let = 0;
        currNode->sendMessage = NULL;
        if (currNode == q->tail) { // msg in tail node
          prevNode->next = NULL;
          q->tail = prevNode;
        } else { // msg in middle node
          prevNode->next = currNode->next;
          currNode->next->prev = prevNode;
        }
        currNode->next = (q->pool == NULL) ? NULL : q->pool;
        currNode->prev = NULL;
        q->pool = currNode;
      }
    }
  }
}

void mq_clear(MessageQueue *q) {
  while (mq_hasMessage(q)) {
    mq_pop(q);
  }
}

void mq_clearAfter(MessageQueue *q, const double timestamp) {
  MessageNode *n = q->tail;
  while (n != NULL && timestamp <= msg_getTimestamp(n->m)) {
    // free the node's message
    mp_freeMessage(&q->mp, n->m);
    n->m = NULL;
    n->let = 0;
    n->sendMessage = NULL;

    // the tail points at the previous node
    q->tail = n->prev;

    // put the node back in the pool
    n->next = q->pool;
    n->prev = NULL;
    if (q->pool != NULL) q->pool->prev = n;
    q->pool = n;

    // update the tail node
    n = q->tail;
  }

  if (q->tail == NULL) q->head = NULL;
}
