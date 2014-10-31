#include <inttypes.h>
#include "stm32f4xx.h"
#include "device.h"
#include "gpio.h"
#include "owlcontrol.h"
#include "SharedMemory.h"

void processBlock(){
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
