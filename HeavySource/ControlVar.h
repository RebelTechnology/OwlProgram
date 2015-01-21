/* Copyright 2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_CONTROL_VAR_H_
#define _HEAVY_CONTROL_VAR_H_

#include "HvBase.h"

typedef struct ControlVar {
  Element e; // type is only every FLOAT or HASH
} ControlVar;

hv_size_t cVar_init_f(ControlVar *o, float k);

hv_size_t cVar_init_s(ControlVar *o, const char *s);

void cVar_free(ControlVar *o);

void cVar_onMessage(HvBase *_c, ControlVar *o, int letIn, const HvMessage *const m,
    void (*sendMessage)(HvBase *, int, const HvMessage *const));

#endif // _HEAVY_CONTROL_VAR_H_
