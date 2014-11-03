#include <inttypes.h>
#include "stm32f4xx.h"
#include "device.h"
#include "gpio.h"
#include "owlcontrol.h"
#include "SharedMemory.h"
#include "SampleBuffer.hpp"
#include "PatchRegistry.h"
#include "PatchController.h"
#include "ApplicationSettings.h"

void setup(){
  setLed(RED);
  settings.init();
  patches.init();
}

SampleBuffer buffer CCM;
PatchRegistry registry CCM;
PatchController patches;
ApplicationSettings settings CCM;

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

void processBlock(){
  blinky();
  buffer.split(smem.audio_input, smem.audio_blocksize);
  patches.process(buffer);  
  buffer.comb(smem.audio_output);
}
