#include "time.h"

void TIMER0_IRQHandler(void)
{
  if ( Chip_TIMER_MatchPending( LPC_TIMER0, 1 ) )
  {
    if( NumOfMS > 0 ) // means timer is running
    {
      Timer0Count++;

      if( Timer0Count == NumOfMS )
      {
        // reset the timer count
        Timer0Count = 0;

        // release delay function while loop
        Timer0Running = FALSE;
      }
    }

    Chip_TIMER_ClearMatch( LPC_TIMER0, 1 );
  }
}

// void timer0DeInit()
// {
//   Chip_TIMER_DeInit( LPC_TIMER0 );
// }

// create timer that calls interrupt
void timer0Init()
{
  Timer0Running = FALSE;
  NumOfMS = 0;
  Timer0Count = 0;
    /* Enable timer 1 clock */
  uint32_t timerFreq;

    Chip_TIMER_Init( LPC_TIMER0 );

    /* Timer rate is system clock rate */
    timerFreq = Chip_Clock_GetSystemClockRate();

    /* Timer setup for match and interrupt at TICKRATE_HZ */
  Chip_TIMER_Reset( LPC_TIMER0 );
  Chip_TIMER_MatchEnableInt( LPC_TIMER0, 1 );

  // TODO: get rates for ms
  Chip_TIMER_SetMatch( LPC_TIMER0, 1, ( timerFreq / TICKRATE_HZ1 ) );
  Chip_TIMER_ResetOnMatchEnable( LPC_TIMER0, 1 );
  Chip_TIMER_Enable( LPC_TIMER0 );

  /* Enable timer interrupt */
  NVIC_ClearPendingIRQ( TIMER0_IRQn );
  NVIC_EnableIRQ( TIMER0_IRQn );

  return;
}


void delay( uint32_t ms )
{
  
 Timer0Running = TRUE;
 NumOfMS = ms;

 // wait for set amount of ms
 while( Timer0Running == TRUE );

 NumOfMS = 0;

}
