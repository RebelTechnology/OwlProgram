/* Copyright 2014 Section6. All Rights Reserved. */

#include "ControlVar.h"

hv_size_t cVar_init_f(ControlVar *o, float k) {
  o->e.type = FLOAT;
  o->e.data.f = k;
  return 0;
}

hv_size_t cVar_init_s(ControlVar *o, const char *s) {
  o->e.type = HASH;
  o->e.data.h = msg_symbolToHash(s);
  return 0;
}

void cVar_free(ControlVar *o) {
  // nothing to do
}

void cVar_onMessage(HvBase *_c, ControlVar *o, int letIn, const HvMessage *const m,
    void (*sendMessage)(HvBase *, int, const HvMessage *const)) {
  switch (letIn) {
    case 0: {
      switch (msg_getType(m,0)) {
        case BANG: {
          HvMessage *n = HV_MESSAGE_ON_STACK(1);
          if (o->e.type == FLOAT) msg_initWithFloat(n, msg_getTimestamp(m), o->e.data.f);
          else if (o->e.type == HASH) msg_initWithHash(n, msg_getTimestamp(m), o->e.data.h);
          else return;
          sendMessage(_c, 0, n);
          break;
        }
        case FLOAT: {
          o->e.type = FLOAT;
          o->e.data.f = msg_getFloat(m,0);
          sendMessage(_c, 0, m);
          break;
        }
        case SYMBOL:
        case HASH: {
          o->e.type = HASH;
          o->e.data.h = msg_getHash(m,0);
          sendMessage(_c, 0, m);
          break;
        }
        default: return;
      }
      break;
    }
    case 1: {
      switch (msg_getType(m,0)) {
        case FLOAT: {
          o->e.type = FLOAT;
          o->e.data.f = msg_getFloat(m,0);
          break;
        }
        case SYMBOL:
        case HASH: {
          o->e.type = HASH;
          o->e.data.h = msg_getHash(m,0);
          break;
        }
        default: break;
      }
    }
    default: return;
  }
}
