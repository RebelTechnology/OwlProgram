#ifndef __OWL_CONTROL_H
#define __OWL_CONTROL_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "device.h"
#include "gpio.h"
#include "StompBox.h"
#include "ProgramVector.h"
#include "message.h"

#ifdef __cplusplus
 extern "C" {
#endif

   typedef enum {
     NONE  = 0,
     GREEN = LED_GREEN,
     RED   = LED_RED
   }  LedPin;

#ifdef __cplusplus
}
#endif

#endif /* __OWL_CONTROL_H */
