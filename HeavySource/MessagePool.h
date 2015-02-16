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

#ifndef _MESSAGE_POOL_H_
#define _MESSAGE_POOL_H_

#include "Utils.h"

struct HvMessage;

#define MP_NUM_MESSAGE_LISTS 4

typedef struct MessagePoolList {
  struct MessageListNode *head; // list of currently available blocks
  struct MessageListNode *pool; // list of currently used blocks
} MessagePoolList;

typedef struct MessagePool {
  char *buffer; // the buffer of all messages
  hv_size_t bufferSize; // in bytes
  hv_size_t bufferIndex; // the number of total reserved bytes

  MessagePoolList lists[MP_NUM_MESSAGE_LISTS];
} MessagePool;

/**
 * The MessagePool is a basic memory management system. It reserves a large block of memory at initialisation
 * and proceeds to divide this block into smaller chunks (usually 512 bytes) as they are needed. These chunks are
 * further divided into 32, 64, 128, or 256 sections. Each of these sections is managed by a MessagePoolList (MPL).
 * An MPL is a linked-list data structure which is initialised such that its own pool of listnodes is filled with nodes
 * that point at each subblock (e.g. each 32-byte block of a 512-block chunk).
 *
 * MessagePool is loosely inspired by TCMalloc. http://goog-perftools.sourceforge.net/doc/tcmalloc.html
 */

hv_size_t mp_init(struct MessagePool *mp, hv_size_t numKB);

void mp_free(struct MessagePool *mp);

/** 
 * Adds a message to the pool and returns a pointer to the copy. Returns NULL 
 * if no space was available in the pool. 
 */
struct HvMessage *mp_addMessage(struct MessagePool *mp, const struct HvMessage *m);

void mp_freeMessage(struct MessagePool *mp, struct HvMessage *m);

#endif // _MESSAGE_POOL_H_
