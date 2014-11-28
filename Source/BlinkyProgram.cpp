#include <inttypes.h>
#include "stm32f4xx.h"
#include "device.h"
#include "gpio.h"
#include "owlcontrol.h"

void setup(){
  setLed(GREEN);
}

void blinky(){
  static uint8_t counter = 0;
  switch(counter++ & 0xff){
  case 0x00:
    setLed(GREEN);
    break;
  case 0x40:
    setLed(NONE);
    break;
  case 0x80:
    setLed(RED);
    break;
  case 0xc0:
    setLed(NONE);
    break;
  }
}

void copyBlock(){
  for(int i=0; i<smem.audio_blocksize; ++i)
    smem.audio_output[i] = smem.audio_input[i];
}

void clearBlock(){
  for(int i=0; i<smem.audio_blocksize; ++i)
    smem.audio_output[i] = 0;
}

void processBlock(){
  blinky();
  // copyBlock();
  // clearBlock();
}
