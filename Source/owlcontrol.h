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

   LedPin getLed();

   inline void setLed(LedPin led){
     clearPin(LED_PORT, led ^ (LED_RED|LED_GREEN));
     setPin(LED_PORT, led);
#ifdef OWLMODULAR
     if(led == LED_RED)
       clearPin(GPIOB, GPIO_Pin_7); // PB7 OWL Modular digital output high
     else
       setPin(GPIOB, GPIO_Pin_7); // PB7 OWL Modular digital output low
#endif
   }

   inline void toggleLed(){
     togglePin(LED_PORT, LED_RED|LED_GREEN);
#ifdef OWLMODULAR
     togglePin(GPIOB, GPIO_Pin_7); // PB7 OWL Modular digital output
#endif
   }

   inline void blink(){
     togglePin(LED_PORT, LED_RED|LED_GREEN);
#ifdef OWLMODULAR
     togglePin(GPIOB, GPIO_Pin_7); // PB7 OWL Modular digital output
#endif
     togglePin(LED_PORT, LED_RED|LED_GREEN);
#ifdef OWLMODULAR
     togglePin(GPIOB, GPIO_Pin_7); // PB7 OWL Modular digital output
#endif
   }

#ifdef __cplusplus
}
#endif

#endif /* __OWL_CONTROL_H */
