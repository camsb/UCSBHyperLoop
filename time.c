#include "time.h"

void Reset_Timer_Counter(LPC_TIMER_T *pTMR) {
  pTMR->TC = 0; // Reset Timer Counter
//  pTMR->PC = 0; // Reset Prescale Counter
}

/* Pass in the timer (E.g. LPC_TIMER0), timer interrupt (E.g. TIMER0_IRQn), and a tickRate (E.g. 2000) */
/* tickRate is the frequency you desire. */
void timerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){

	uint32_t timerFreq;
	Chip_TIMER_Init( timer );

	/* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetSystemClockRate();

	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset( timer );
	Chip_TIMER_MatchEnableInt( timer, 1 );

	// TODO: get rates for ms
	Chip_TIMER_SetMatch( timer, 1, ( 2 * timerFreq / tickRate ) );
	Chip_TIMER_ResetOnMatchEnable( timer, 1 );
	Chip_TIMER_Enable( timer );

	/* Enable timer interrupt */
	NVIC_ClearPendingIRQ( timerInterrupt );
	NVIC_EnableIRQ( timerInterrupt );

	return;
}

void delayTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
  timerInit(timer, timerInterrupt, tickRate);
}

/* Used for the delay function */
void TIMER0_IRQHandler(void){
  if ( Chip_TIMER_MatchPending( LPC_TIMER0, 1 ) ){
    if( NumOfMS > 0 ){ // means timer is running
      Timer0Count++;
      if( Timer0Count == NumOfMS ){
        // reset the timer count
        Timer0Count = 0;
        // release delay function while loop
        Timer0Running = FALSE;
      }
    }
    Chip_TIMER_ClearMatch( LPC_TIMER0, 1 );
  }
}

/* Needs to be changed to be interrupt based. */
void delay( uint32_t ms )
{

 Timer0Running = TRUE;
 NumOfMS = ms;

 // wait for set amount of ms
 while( Timer0Running == TRUE );

 NumOfMS = 0;

}
