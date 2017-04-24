#include "timer.h"
#include "sensor_data.h"
#include "ethernet.h"

void runtimeTimerInit() {
	uint32_t timerFreq = Chip_Clock_GetSystemClockRate();

	Chip_TIMER_Init(LPC_TIMER0);

	// MAGIC CONSTANT 2067 - This was determined relative to the RTC on 4/24/17
	Chip_TIMER_PrescaleSet(LPC_TIMER0, timerFreq/2067);	// Increment counter every millisecond.

	Chip_TIMER_Enable(LPC_TIMER0);
}

uint32_t getRuntime() {
	return Chip_TIMER_ReadCount(LPC_TIMER0);
}

void Reset_Timer_Counter(LPC_TIMER_T *pTMR) {
  pTMR->TC = 0; // Reset Timer Counter
//  pTMR->PC = 0; // Reset Prescale Counter
}

/* Pass in the timer (E.g. LPC_TIMER0), timer interrupt (E.g. TIMER0_IRQn), and a tickRate (E.g. 2000)*/
/* tickRate is the frequency you desire. */
void timerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){

	uint32_t timerFreq;
	Chip_TIMER_Init( timer );

	/* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetSystemClockRate();

	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset( timer );
	Chip_TIMER_MatchEnableInt( timer, 1 );

	Chip_TIMER_SetMatch( timer, 1, ( timerFreq / (tickRate * 2) ) );
	Chip_TIMER_ResetOnMatchEnable( timer, 1 );
	Chip_TIMER_Enable( timer );

	/* Enable timer interrupt */
	NVIC_ClearPendingIRQ( timerInterrupt );
	NVIC_EnableIRQ( timerInterrupt );

	return;
}
