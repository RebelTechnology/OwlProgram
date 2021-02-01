#ifndef __MAIN_H
#define __MAIN_H

#include "ProgramVector.h"

#ifdef __cplusplus
 extern "C" {
#endif

   void setup(ProgramVector* pv);
   void run(ProgramVector* pv);

   void doSetPatchParameter(uint8_t id, int16_t value);
   void doSetButton(uint8_t id, uint16_t state, uint16_t samples);
   void onButtonChanged(uint8_t bid, uint16_t state, uint16_t samples);
   void onDrawCallback(uint8_t* pixels, uint16_t width, uint16_t height);

#ifdef USE_MIDI_CALLBACK
   void doMidiSend(uint8_t port, uint8_t d0, uint8_t d1, uint8_t d2);
#endif /* USE_MIDI_CALLBACK */


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
