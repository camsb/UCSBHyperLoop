#ifndef GPIO_H_
#define GPIO_H_

#include "board.h"
#include "timer.h"

#define GPIO_INTERRUPT_NVIC    GPIO_IRQn
#define GPIO_CONTACT_SENSOR_PORT 0
#define	GPIO_CONTACT_SENSOR_PIN 28

uint8_t prototypeRunFlag;
int prototypeRunStartTime;

void GPIO_IRQHandler(void);
void GPIO_Interrupt_Enable();
void GPIO_Input_Init(uint8_t port, uint8_t pin);
void GPIO_Output_Init(uint8_t port, uint8_t pin);

int GPIO_Contact_Sensor_Pushed();

#endif
