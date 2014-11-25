#include <inttypes.h>
#include "stm32f4xx.h"
#include "device.h"
#include "gpio.h"
#include "owlcontrol.h"
#include "SharedMemory.h"
#include "SampleBuffer.hpp"
#include "PatchRegistry.h"
#include "PatchController.h"
#include "../Libraries/OwlPatches/GainPatch.hpp"

void setup(){
  setLed(GREEN);
  // int* foo = new int; // causes reset
  registry.init();
  patches.init(); // causes reset?
}

SampleBuffer buffer;
PatchRegistry registry;
PatchController patches;

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
  // for(int i=0; i<smem.audio_blocksize; ++i){
  //   // smem.audio_output[i] = 0;
  //   smem.audio_output[i] = smem.audio_input[i];
  // }
  buffer.split(smem.audio_input, smem.audio_blocksize);
  patches.process(buffer);
  buffer.comb(smem.audio_output);
}
