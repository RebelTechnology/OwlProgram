#include "owlcontrol.h"
#include "usbcontrol.h"
#include "armcontrol.h"
#include <string.h>
#include "stm32f4xx.h"
#include "device.h"
#include "gpio.h"

LedPin getLed(){
  if(getPin(LED_PORT, LED_GREEN))
    return GREEN;
  else if(getPin(LED_PORT, LED_RED))
    return RED;
  else
    return NONE;
}

void ledSetup(){
  RCC_AHB1PeriphClockCmd(LED_CLOCK, ENABLE);
  configureDigitalOutput(LED_PORT, LED_RED|LED_GREEN);
  clearPin(LED_PORT, LED_RED|LED_GREEN);
}

uint16_t adc_values[NOF_ADC_VALUES];
void adcSetup(){
  memset(adc_values, 0, sizeof adc_values);
  adcSetupDMA(&adc_values[0]);
}

uint16_t getAnalogValue(uint8_t index){
  /* assert_param(index < sizeof(adc_values)); */
  return adc_values[index];
}

uint16_t* getAnalogValues(){
  return adc_values;
}

void (*externalInterruptCallbackA)();
void (*externalInterruptCallbackB)();

/** 
 * Configure EXTI callback
 */
void setupSwitchA(void (*f)()){
  externalInterruptCallbackA = f;

  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the clocks */
  RCC_AHB1PeriphClockCmd(SWITCH_A_CLOCK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure switch pin */
  configureDigitalInput(SWITCH_A_PORT, SWITCH_A_PIN, GPIO_PuPd_UP);

  /* Connect EXTI Line to GPIO Pin */
  SYSCFG_EXTILineConfig(SWITCH_A_PORT_SOURCE, SWITCH_A_PIN_SOURCE);

  /* Configure EXTI line */
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = SWITCH_A_PIN_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SWITCH_A_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = SWITCH_A_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = SWITCH_A_SUBPRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void SWITCH_A_HANDLER(void) {
  if(EXTI_GetITStatus(SWITCH_A_PIN_LINE) != RESET){ 
    (*externalInterruptCallbackA)();
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(SWITCH_A_PIN_LINE);
  }
}

/** 
 * Configure EXTI callback
 */
void setupSwitchB(void (*f)()){
  externalInterruptCallbackB = f;

  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the clocks */
  RCC_AHB1PeriphClockCmd(SWITCH_B_CLOCK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure switch pin */
  configureDigitalInput(SWITCH_B_PORT, SWITCH_B_PIN, GPIO_PuPd_UP);

  /* Connect EXTI Line to GPIO Pin */
  SYSCFG_EXTILineConfig(SWITCH_B_PORT_SOURCE, SWITCH_B_PIN_SOURCE);

  /* Configure EXTI line */
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = SWITCH_B_PIN_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
/*   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SWITCH_B_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = SWITCH_B_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = SWITCH_B_SUBPRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void SWITCH_B_HANDLER(void) {
  if(EXTI_GetITStatus(SWITCH_B_PIN_LINE) != RESET){ 
    (*externalInterruptCallbackB)();
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(SWITCH_B_PIN_LINE);
  }
}

void setupExpressionPedal(){
  configureDigitalOutput(EXPRESSION_PEDAL_TIP_PORT, EXPRESSION_PEDAL_TIP_PIN); // PA3, expression pedal tip
  setPin(EXPRESSION_PEDAL_TIP_PORT, EXPRESSION_PEDAL_TIP_PIN);
  // expression pedal ring, PA2, is set up in adcSetupDMA()
}

bool hasExpressionPedal(){
  /* clearPin(EXPRESSION_PEDAL_TIP_PORT, EXPRESSION_PEDAL_TIP_PIN); */
  configureDigitalInput(EXPRESSION_PEDAL_TIP_PORT, EXPRESSION_PEDAL_TIP_PIN, GPIO_PuPd_UP);
  bool connected = getPin(EXPRESSION_PEDAL_TIP_PORT, EXPRESSION_PEDAL_TIP_PIN);
  setupExpressionPedal();
  return connected;
}
