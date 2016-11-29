#ifndef GPIO_H_
#define GPIO_H_

#include "board.h"

#define GPIO_INTERRUPT_NVIC    GPIO_IRQn

void GPIO_IRQHandler(void);
void GPIO_Interrupt_Enable();
void GPIO_Input_Init(uint8_t port, uint8_t pin);
void GPIO_Output_Init(uint8_t port, uint8_t pin);

#endif
