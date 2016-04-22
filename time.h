#ifndef TIME_H_
#define TIME_H_

#include "stdint.h"
#include "board.h"
#include "sensor_data.h"
#include "ethernet.h"


void    Reset_Timer_Counter(LPC_TIMER_T *pTMR);
void 	timerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate);
void    TIMER0_IRQHandler(void);
void    TIMER1_IRQHandler(void);
void    TIMER2_IRQHandler(void);
void    delay( uint32_t ms );

volatile uint8_t Timer0Running;
volatile uint32_t NumOfMS;
volatile int Timer0Count;

#endif
