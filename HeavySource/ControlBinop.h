/* Copyright 2014 Section6. All Rights Reserved. */

#ifndef _HEAVY_CONTROL_BINOP_H_
#define _HEAVY_CONTROL_BINOP_H_

#include "HvBase.h"

typedef enum BinopType {
  HV_BINOP_ADD,
  HV_BINOP_SUBTRACT,
  HV_BINOP_MULTIPLY,
  HV_BINOP_DIVIDE,
  HV_BINOP_INT_DIV,
  HV_BINOP_MOD_BIPOLAR,
  HV_BINOP_MOD_UNIPOLAR,
  HV_BINOP_BIT_LEFTSHIFT,
  HV_BINOP_BIT_RIGHTSHIFT,
  HV_BINOP_BIT_AND,
  HV_BINOP_BIT_XOR,
  HV_BINOP_BIT_OR,
  HV_BINOP_EQL_EQL,
  HV_BINOP_NOT_EQL,
  HV_BINOP_LOGICAL_AND,
  HV_BINOP_LOGICAL_OR,
  HV_BINOP_LESS_THAN,
  HV_BINOP_LESS_THAN_EQL,
  HV_BINOP_GREATER_THAN,
  HV_BINOP_GREATER_THAN_EQL,
  HV_BINOP_MAX,
  HV_BINOP_MIN,
  HV_BINOP_POW,
  HV_BINOP_ATAN2
} BinopType;

typedef struct ControlBinop {
  float k;
} ControlBinop;

hv_size_t cBinop_init(ControlBinop *o, float k);

void cBinop_onMessage(HvBase *_c, ControlBinop *o, BinopType op, int letIn,
    const HvMessage *const m,
    void (*sendMessage)(HvBase *, int, const HvMessage *const));

void cBinop_k_onMessage(HvBase *_c, void *o, BinopType op, const float k,
    int letIn, const HvMessage *const m,
    void (*sendMessage)(HvBase *, int, const HvMessage *const));

#endif // _HEAVY_CONTROL_BINOP_H_
