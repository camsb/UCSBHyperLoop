#ifndef TIME_H_
#define TIME_H_

#include "board.h"

void runtimeTimerInit();
uint32_t getRuntime();	// In milliseconds.
void    Reset_Timer_Counter(LPC_TIMER_T *pTMR);
void 	timerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate);

#endif
