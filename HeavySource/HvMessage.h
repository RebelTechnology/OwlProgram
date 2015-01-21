/* Copyright 2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_MESSAGE_H_
#define _HEAVY_MESSAGE_H_

#include "Utils.h"

typedef enum ElementType {
  BANG,
 	FLOAT,
 	SYMBOL,
  HASH
} ElementType;

typedef struct Element {
  ElementType type;
  union {
    float f; // float
    char *s; // symbol
    hv_uint32_t h; // hash
  } data;
} Element;

typedef struct HvMessage {
  hv_uint32_t timestamp; // the sample at which this message should be processed
  hv_uint16_t numElements;
  hv_uint16_t numBytes; // the number of bytes that this message occupies in memory
  Element elem;
} HvMessage;

#define HV_MESSAGE_ON_STACK(_x) (HvMessage *) hv_alloca(msg_getByteSize(_x))

/** Returns the total length in bytes of this message for a given number of elements. */
static inline hv_size_t msg_getByteSize(hv_size_t numElements) {
  assert(numElements > 0);
  return sizeof(HvMessage) + ((numElements-1) * sizeof(Element));
}

HvMessage *msg_copy(const HvMessage *m);

/** Returns the number of bytes that this message would occupy on the heap. */
hv_size_t msg_getNumHeapBytes(const HvMessage *m);

/** Copies the message into the given buffer. The buffer must be at least as large as msg_getNumHeapBytes(). */
void msg_copyToBuffer(const HvMessage *m, char *buffer, hv_size_t len);

void msg_setElementToFrom(HvMessage *n, int indexN, const HvMessage *const m, int indexM);

/** Frees a message on the heap. Does nothing if argument is NULL. */
void msg_free(HvMessage *m);

HvMessage *msg_init(HvMessage *m, hv_size_t numElements, hv_uint32_t timestamp);

HvMessage *msg_initWithFloat(HvMessage *m, hv_uint32_t timestamp, float f);

HvMessage *msg_initWithBang(HvMessage *m, hv_uint32_t timestamp);

HvMessage *msg_initWithSymbol(HvMessage *m, hv_uint32_t timestamp, char *s);

HvMessage *msg_initWithHash(HvMessage *m, hv_uint32_t timestamp, hv_uint32_t h);

HvMessage *msg_initV(HvMessage *const m, const hv_uint32_t timestamp, const char *format, ...);

static inline hv_uint32_t msg_getTimestamp(const HvMessage *m) {
  return m->timestamp;
}

static inline void msg_setTimestamp(HvMessage *m, hv_uint32_t timestamp) {
  m->timestamp = timestamp;
}

static inline int msg_getNumElements(const HvMessage *m) {
  return (int) m->numElements;
}

/** Returns the number of bytes this message in memory. */
static inline hv_size_t msg_getNumBytes(const HvMessage *m) {
  return m->numBytes;
}

static inline ElementType msg_getType(const HvMessage *m, int index) {
  return (&(m->elem)+index)->type;
}

static inline void msg_setBang(HvMessage *m, int index) {
  (&(m->elem)+index)->type = BANG;
  (&(m->elem)+index)->data.s = NULL;
}

static inline int msg_isBang(const HvMessage *m, int index) {
  return (&(m->elem)+index)->type == BANG;
}

static inline float msg_getFloat(const HvMessage *const m, int index) {
  return (&(m->elem)+index)->data.f;
}

static inline void msg_setFloat(HvMessage *m, int index, float f) {
  (&(m->elem)+index)->type = FLOAT;
  (&(m->elem)+index)->data.f = f;
}

static inline int msg_isFloat(const HvMessage *const m, int index) {
  return (&(m->elem)+index)->type == FLOAT;
}

static inline void msg_setHash(HvMessage *m, int index, hv_uint32_t h) {
  (&(m->elem)+index)->type = HASH;
  (&(m->elem)+index)->data.h = h;
}

static inline int msg_isHash(const HvMessage *m, int index) {
  return (&(m->elem)+index)->type == HASH;
}

/** Returns true if the element is a hash or symbol. False otherwise. */
static inline bool msg_isHashLike(const HvMessage *m, int index) {
  return ((&(m->elem)+index)->type == HASH) || ((&(m->elem)+index)->type == SYMBOL);
}

/** Returns a 32-bit hash of the given string. */
hv_uint32_t msg_symbolToHash(const char *s);

/** Returns a 32-bit hash of the given element. */
hv_uint32_t msg_getHash(const HvMessage *const m, int i);

static inline void msg_setSymbol(HvMessage *m, int index, char *s) {
  (&(m->elem)+index)->type = SYMBOL;
  (&(m->elem)+index)->data.s = s;
}

static inline char *msg_getSymbol(const HvMessage *m, int index) {
  return (&(m->elem)+index)->data.s;
}

static inline int msg_isSymbol(const HvMessage *m, int index) {
  return (&(m->elem)+index)->type == SYMBOL;
}

bool msg_compareSymbol(const HvMessage *m, int i, const char *s);

/** Returns 1 if the element i_m of message m is equal to element i_n of message n. */
bool msg_equalsElement(const HvMessage *m, int i_m, const HvMessage *n, int i_n);

bool msg_hasFormat(const HvMessage *m, const char *fmt);

/**
 * Create a string representation of the message. Suitable for use by the print object.
 * The resulting string must be freed by the caller.
 */
char *msg_toString(const HvMessage *msg);

/**
 * Resolves any number of dollar arguments and generates a string based on the arguments.
 * @param m  The message from which to take values
 * @param n  The message to fill in
 * @param z  The element index to resolve
 * @param buf  The scratch (i.e. resolution) buffer
 * @param len  The length of the scratch buffer
 * @param args  A string of 'i' and 's' chars indicating the type of the arguments, either indicies or strings
 * @param varargs  The components to resolve, either dollar indicies or strings.
 * If the index is negative, the graph id is used
 * @return  true if the resolution buffer is needed, false otherwise.
 */
// bool msg_resolveDollarArguments(HvMessage *m, HvMessage *n, int z, char *buf, hv_size_t len, const char *args, ...);

#endif // _HEAVY_MESSAGE_H_
