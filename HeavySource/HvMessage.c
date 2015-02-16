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

#include "HvMessage.h"

HvMessage *msg_init(HvMessage *m, hv_size_t numElements, hv_uint32_t timestamp) {
  m->timestamp = timestamp;
  m->numElements = (hv_uint16_t) numElements;
  m->numBytes = (hv_uint16_t) msg_getByteSize(numElements);
  return m;
}

HvMessage *msg_initWithFloat(HvMessage *m, hv_uint32_t timestamp, float f) {
  m->timestamp = timestamp;
  m->numElements = 1;
  m->numBytes = sizeof(HvMessage);
  msg_setFloat(m, 0, f);
  return m;
}

HvMessage *msg_initWithBang(HvMessage *m, hv_uint32_t timestamp) {
  m->timestamp = timestamp;
  m->numElements = 1;
  m->numBytes = sizeof(HvMessage);
  msg_setBang(m, 0);
  return m;
}

HvMessage *msg_initWithSymbol(HvMessage *m, hv_uint32_t timestamp, char *s) {
  m->timestamp = timestamp;
  m->numElements = 1;
  m->numBytes = sizeof(HvMessage);
  msg_setSymbol(m, 0, s);
  return m;
}

HvMessage *msg_initWithHash(HvMessage *m, hv_uint32_t timestamp, hv_uint32_t h) {
  m->timestamp = timestamp;
  m->numElements = 1;
  m->numBytes = sizeof(HvMessage);
  msg_setHash(m, 0, h);
  return m;
}

HvMessage *msg_initV(HvMessage *const m, const hv_uint32_t timestamp, const char *format, ...) {
  va_list ap;
  va_start(ap, format);

  const int numElem = (int) hv_strlen(format);
  msg_init(m, numElem, timestamp);
  for (int i = 0; i < numElem; i++) {
    switch (format[i]) {
      case 'b': msg_setBang(m,i); break;
      case 'f': msg_setFloat(m, i, (float) va_arg(ap, double)); break;
      case 's': msg_setSymbol(m, i, (char *) va_arg(ap, char *)); break;
      case 'h': // hash not supported
      default: break;
    }
  }
  va_end(ap);

  return m;
}

hv_size_t msg_getNumHeapBytes(const HvMessage *m) {
  // get the size of all symbol elements
  hv_size_t rsizeofsym = 0;
  for (int i = 0; i < msg_getNumElements(m); ++i) {
    if (msg_isSymbol(m,i)) {
      rsizeofsym += hv_strlen(msg_getSymbol(m,i)) + 1; // +1 to allow for trailing '\0'
    }
  }

  // the total byte size on the heap
  return (msg_getByteSize(msg_getNumElements(m)) + rsizeofsym);
}

void msg_copyToBuffer(const HvMessage *m, char *buffer, hv_size_t len) {
  HvMessage *r = (HvMessage *) buffer;

  // assert that the message is not already larger than the length of the buffer
  hv_assert(msg_getNumBytes(m) <= len);

  // copy the basic message to the buffer
  hv_memcpy(r, m, msg_getNumBytes(m));

  hv_size_t len_r = msg_getNumBytes(m);

  char *p = buffer + msg_getByteSize(msg_getNumElements(m)); // points to the end of the base message
  for (int i = 0; i < msg_getNumElements(m); ++i) {
    if (msg_isSymbol(m,i)) {
      const hv_size_t symLen = hv_strlen(msg_getSymbol(m,i)) + 1; // include the trailing null char
      hv_assert(len_r + symLen <= len); // stay safe!
      hv_strncpy(p, msg_getSymbol(m,i), symLen);
      msg_setSymbol(r, i, p);
      p += symLen;
      len_r += symLen;
    }
  }

  r->numBytes = (hv_uint16_t) len_r; // update the message size in memory
}

// the message is serialised such that all symbol elements are placed in order at the end of the buffer
HvMessage *msg_copy(const HvMessage *m) {
  const hv_size_t heapSize = msg_getNumHeapBytes(m);
  char *r = (char *) hv_malloc(heapSize);
  msg_copyToBuffer(m, r, heapSize);
  return (HvMessage *) r;
}

void msg_free(HvMessage *m) {
  hv_free(m); // because heap messages are serialised in memory, a simple call to free releases the message
}

bool msg_hasFormat(const HvMessage *m, const char *fmt) {
  if (fmt == NULL) return false;
  if (msg_getNumElements(m) != hv_strlen(fmt)) return false;
  for (int i = 0; i < msg_getNumElements(m); i++) {
    switch (fmt[i]) {
      case 'b': if (!msg_isBang(m, i)) return false; break;
      case 'f': if (!msg_isFloat(m, i)) return false; break;
      case 's': if (!msg_isSymbol(m, i)) return false; break;
      case 'h': if (!msg_isHash(m, i)) return false; break;
      default: return false;
    }
  }
  return true;
}

bool msg_compareSymbol(const HvMessage *m, int i, const char *s) {
  switch (msg_getType(m,i)) {
    case SYMBOL: return !hv_strcmp(msg_getSymbol(m, i), s);
    case HASH: return (msg_getHash(m,i) == msg_symbolToHash(s));
    default: return false;
  }
}

bool msg_equalsElement(const HvMessage *m, int i_m, const HvMessage *n, int i_n) {
  if (i_m < msg_getNumElements(m) && i_n < msg_getNumElements(n)) {
    if (msg_getType(m, i_m) == msg_getType(n, i_n)) {
      switch (msg_getType(m, i_m)) {
        case BANG: return true;
        case FLOAT: return (msg_getFloat(m, i_m) == msg_getFloat(n, i_n));
        case SYMBOL: return msg_compareSymbol(m, i_m, msg_getSymbol(n, i_n));
        case HASH: return msg_getHash(m,i_m) == msg_getHash(n,i_n);
        default: break;
      }
    }
  }
  return false;
}

void msg_setElementToFrom(HvMessage *n, int i_n, const HvMessage *const m, int i_m) {
  switch (msg_getType(m, i_m)) {
    case BANG: msg_setBang(n, i_n); break;
    case FLOAT: msg_setFloat(n, i_n, msg_getFloat(m, i_m)); break;
    case SYMBOL: msg_setSymbol(n, i_n, msg_getSymbol(m, i_m)); break;
    case HASH: msg_setHash(n, i_n, msg_getHash(m, i_m));
    default: break;
  }
}

hv_uint32_t msg_symbolToHash(const char *s) {
  // this hash is based MurmurHash2
  // http://en.wikipedia.org/wiki/MurmurHash
  // https://sites.google.com/site/murmurhash/
  static const unsigned int n = 0x5bd1e995;
  static const int r = 24;

  int len = (int) hv_strlen(s);
  hv_uint32_t x = (hv_uint32_t) (len); // seed (0) ^ len

  while (len >= 4) {
    hv_uint32_t k = *(hv_uint32_t *)s;
    k *= n;
    k ^= k >> r;
    k *= n;
    x *= n;
    x ^= k;
    s += 4; len -= 4;
  }

  switch(len) {
    case 3: x ^= s[2] << 16;
    case 2: x ^= s[1] << 8;
    case 1: x ^= s[0]; x *= n;
    default: break;
  }

  x ^= x >> 13;
  x *= n;
  x ^= x >> 15;

  return x;
}

hv_uint32_t msg_getHash(const HvMessage *const m, int i) {
  switch (msg_getType(m,i)) {
    case BANG: return 0xFFFFFFFF;
    case FLOAT: {
      float f = msg_getFloat(m,i);
      return *((hv_uint32_t *) &f);
    }
    case SYMBOL: return msg_symbolToHash(msg_getSymbol(m,i));
    case HASH: return (&(m->elem)+i)->data.h;
    default: return 0;
  }
}

char *msg_toString(const HvMessage *m) {
  // http://stackoverflow.com/questions/295013/using-sprintf-without-a-manually-allocated-buffer
  hv_size_t *lengths = (hv_size_t *) hv_alloca(m->numElements*sizeof(hv_uint32_t));  // how long is the string of each atom
  char *finalString; // the final buffer we will pass back after concatenating all strings - user should free it
  int size = 0; // the total length of our final buffer
  int pos = 0;

  // loop through every element in our list of atoms
  // first loop figures out how long our buffer should be
  // chrism: apparently this might fail under MSVC because of snprintf(NULL) - do we care?
  for (int i = 0; i < msg_getNumElements(m); i++) {
    lengths[i] = 0;
    switch (msg_getType(m, i)) {
      case BANG: lengths[i] = hv_snprintf(NULL, 0, "%s", "bang"); break;
      case FLOAT: lengths[i] = hv_snprintf(NULL, 0, "%g", msg_getFloat(m, i)); break;
      case SYMBOL: lengths[i] = hv_snprintf(NULL, 0, "%s", msg_getSymbol(m, i)); break;
      case HASH: lengths[i] = hv_snprintf(NULL, 0, "0x%X", msg_getHash(m, i)); break;
      default: break;
    }
    // total length of our string is each atom plus a space, or \0 on the end
    size += lengths[i] + 1;
  }

  // now we do the piecewise concatenation into our final string
  finalString = (char *) hv_malloc(size <= 0 ? 1 : sizeof(char)); // ensure that size is at least 1
  for (int i = 0; i < msg_getNumElements(m); i++) {
    // first element doesn't have a space before it
    if (i > 0) {
      hv_strncat(finalString, " ", 1);
      pos += 1;
    }
    // put a string representation of each atom into the final string
    switch (msg_getType(m, i)) {
      case BANG: hv_snprintf(&finalString[pos], lengths[i] + 1, "%s", "bang"); break;
      case FLOAT: hv_snprintf(&finalString[pos], lengths[i] + 1, "%g", msg_getFloat(m, i)); break;
      case SYMBOL: hv_snprintf(&finalString[pos], lengths[i] + 1, "%s", msg_getSymbol(m, i)); break;
      case HASH: hv_snprintf(&finalString[pos], lengths[i] + 1, "0x%X", msg_getHash(m, i)); break;
      default: break;
    }
    pos += lengths[i];
  }
  return finalString;
}

/*
 * TODO(mhroth): unnecessary for now
bool msg_resolveDollarArguments(HvMessage *m, HvMessage *n, int z, char *buf, hv_size_t len, const char *args, ...) {
  va_list ap;
  va_start(ap, args);

  hv_memset(buf, 0, len); // clear the buffer
  hv_size_t j = 0; // position in buffer
  const hv_size_t numArgs = hv_strlen(args); // the number of arguments

  // if there is only one argument then the result has the chance of being a number, otherwise no
  bool isNumber = (numArgs == 1);

  for (hv_size_t i = 0; i < numArgs; ++i) {
    switch (args[i]) {
      case 'i': { // a message index
        const int index = (int) va_arg(ap, int);
        if (index < 0) {
          // $0 always resolve to "0"
          const hv_size_t x = 1;
          if (x < len-j) { // always < in order to allow for trailing \0
            j += snprintf(buf+j, len-j, "0");
          }
        } else {
          switch (msg_getType(m, index)) {
            default:
            case BANG: break; // this case should never happen
            case FLOAT: {
              const hv_size_t x = snprintf(NULL, 0, "%g", msg_getFloat(m,index));
              if (x < len-j) { // ensure that the buffer is big enough
                j += snprintf(buf+j, len-j, "%g", msg_getFloat(m,index));
              }
              break;
            }
            case SYMBOL: {
              const hv_size_t x = snprintf(NULL, 0, "%s", msg_getSymbol(m,index));
              if (x < len-j) {
                j += snprintf(buf+j, len-j, "%s", msg_getSymbol(m,index));
                isNumber = false;
              }
              break;
            }
          }
        }
        break;
      }
      case 's': { // a string
        const char *s = (char *) va_arg(ap, char *);
        const hv_size_t x = snprintf(NULL, 0, "%s", s);
        if (x <= len-j) {
          j += snprintf(buf+j, len-j, "%s", s);
          isNumber = false;
        }
        break;
      }
      default: break;
    }
  }

  if (isNumber) {
    msg_setFloat(n,z,(float) atof(buf));
  } else {
    msg_setSymbol(n,z,buf);
  }

  va_end(ap);

  return !isNumber;
}
*/
