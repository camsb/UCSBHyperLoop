#ifndef TIME_H_
#define TIME_H_

#include "stdint.h"
#include "board.h"

void    delay( uint32_t ms );
void    timer0DeInit();
void    timer0Init();
void    TIMER0_IRQHandler(void);

volatile int Timer0Running;
volatile int NumOfMS;
volatile int Timer0Count;

#define TICKRATE_HZ1         2000      // tentatively 100 persecond         // uncomment this line to have time in milliseconds
//#define TICKRATE_HZ1         2        // 1 per sec

#endif
