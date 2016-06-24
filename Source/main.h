#ifndef __MAIN_H
#define __MAIN_H

#include "ProgramVector.h"

#ifdef __cplusplus
 extern "C" {
#endif

   void setup(ProgramVector* pv);
   void processBlock(ProgramVector* pv);

   void doSetPatchParameter(uint8_t id, int16_t value);
   void doSetButton(uint8_t id, uint16_t state, uint16_t samples);
   void onButtonChanged(uint8_t bid, uint16_t state, uint16_t samples);
   void onEncoderChanged(uint8_t bid, int16_t delta, uint16_t samples);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
