#ifndef PHOTO_ELECTRIC_H_
#define PHOTO_ELECTRIC_H_

#include "board.h"

#define PHOTOELECTRIC_TIMER_RATE      (11)        /* 1 ticks per second */
#define PHOTOELECTRIC_INT_PIN         5       /* GPIO pin number mapped to interrupt */
#define PHOTOELECTRIC_INT_PORT        GPIOINT_PORT0 /* GPIO port number mapped to interrupt */
#define PHOTOELECTRIC_IRQ_HANDLER     GPIO_IRQHandler /* GPIO interrupt IRQ function name */
#define PHOTOELECTRIC_INTERRUPT_NVIC  GPIO_IRQn   /* GPIO interrupt NVIC interrupt name */

uint8_t stripDetectedFlag;     // Photoelectric interrupt flag
uint8_t strip_count;      // Total strips detected
uint8_t regional_strip_count; // Strips in current region
uint8_t strip_region;     // Current region

void stripDetected();
// void PHOTOELECTRIC_IRQ_HANDLER(void);
void TIMER3_IRQHandler(void);
void Photoelectric_GPIO_Init();
void Photoelectric_Interrupt_Enable();
void Photoelectric_Timer_Init();
void photoelectricInit();

#endif
