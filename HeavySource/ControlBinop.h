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
  HV_BINOP_EQ,
  HV_BINOP_NEQ,
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
