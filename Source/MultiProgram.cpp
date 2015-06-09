#include <inttypes.h>
#include "stm32f4xx.h"
#include "device.h"
#include "gpio.h"
#include "owlcontrol.h"
#include "ProgramVector.h"
#include "SampleBuffer.hpp"
#include "PatchRegistry.h"
#include "PatchController.h"

SampleBuffer buffer;
PatchRegistry registry;
PatchController patches;

void setup(){
  setLed(GREEN);
  // int* foo = new int; // causes reset
  registry.init();
  patches.init(); // causes reset?
}

void processBlock(){
  buffer.split(getProgramVector()->audio_input, getProgramVector()->audio_blocksize);
  patches.process(buffer);
  buffer.comb(getProgramVector()->audio_output);
}
