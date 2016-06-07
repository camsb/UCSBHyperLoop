#include "photo_electric.h"
#include "gpio.h"
#include "timer.h"
#include "kinematics.h"

// Do we need this function?
void stripDetected(){
  stripDetectedFlag = 0;

  uint32_t cyclesBetweenInterrupts;
  cyclesBetweenInterrupts = Chip_TIMER_ReadCount(LPC_TIMER3);
  timeBetweenInterrupts = ((float) cyclesBetweenInterrupts)/120000000.0;

  combinePositions();
}

void Photoelectric_Timer_Init() {
	/* Enable timer 1 clock */
	Chip_TIMER_Init(LPC_TIMER3);

	/* Timer rate is system clock rate */
	uint32_t timerFreq = Chip_Clock_GetSystemClockRate();

	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER3);
	//Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);
	Chip_TIMER_SetMatch(LPC_TIMER3, 1, (timerFreq / PHOTOELECTRIC_TIMER_RATE));
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);

	/* Enable timer interrupt */
	NVIC_ClearPendingIRQ(TIMER3_IRQn);
	NVIC_EnableIRQ(TIMER3_IRQn);
}

void TIMER3_IRQHandler(void)
{
  if (Chip_TIMER_MatchPending(LPC_TIMER3, 1)) {
    Chip_TIMER_ClearMatch(LPC_TIMER3, 1);
    Chip_TIMER_MatchDisableInt(LPC_TIMER3, 1);
    Chip_TIMER_Disable(LPC_TIMER3);
    Reset_Timer_Counter(LPC_TIMER3);
  }
  stripRegion++;
  regionalStripCount = 0;
}

/* Setup Photoelectric sensor pin as input */
void Photoelectric_GPIO_Init() {
	/* Configure photoelectric interrupt pin as input */
	GPIO_Input_Init(PHOTOELECTRIC_INT_PORT, PHOTOELECTRIC_INT_PIN);
}

/* Initialize and enable photoelectric sensor interrupts */
void Photoelectric_Interrupt_Enable() {
	/* Configure the GPIO interrupt */
	Chip_GPIOINT_SetIntRising(LPC_GPIOINT, PHOTOELECTRIC_INT_PORT, 1 << PHOTOELECTRIC_INT_PIN); // Set to rising edge trigger
}

void photoelectricInit() {
	Photoelectric_GPIO_Init();      // Initialize photoelectric GPIO Pin
	Photoelectric_Interrupt_Enable(); // Initialize photoelectric interrupts
	Photoelectric_Timer_Init();     // Initialize photoelectric timer
}
