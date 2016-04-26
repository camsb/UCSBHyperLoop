#include "pwm.h"

///* Initialize PWM */
//void Init_PWM(LPC_TIMER_T * timer, uint16_t prescaler, uint16_t OnResetValue)
//{
//    /* Setup Timer for PWM */
//    Chip_TIMER_Init(timer);
//    // MR0 reset
//    Chip_TIMER_ResetOnMatchEnable(timer, 0);
//    // Set the frequency prescaler for the timer
//    Chip_TIMER_PrescaleSet(timer, prescaler-1);
//    // Set MR0 value for resetting the timer
//    Chip_TIMER_SetMatch(timer, 0, OnResetValue);
//    // Set PWM Control Register
//    Chip_TIMER_PWMWrite(timer, (1<<0 | 1<<1 | 1<<2));
//    // Enable Timer
//    Chip_TIMER_Enable(timer);
//}

/* Initialize PWM */
void Init_PWM(LPC_PWM_T * pwm, uint16_t prescaler, float duty)
{
	uint16_t OnResetValue = 3500;
	uint32_t duty_int = ((float)OnResetValue)*duty;

	Chip_PWM_Init(pwm);
	Chip_PWM_PrescaleSet(pwm, prescaler);
	Chip_PWM_SetMatch(pwm, 0, OnResetValue);
	Chip_PWM_SetMatch(pwm, 1, duty_int);
	Chip_PWM_ResetOnMatchEnable(pwm, 0);
//	Chip_PWM_SetCountClockSrc(pwm, PWM_CAPSRC_RISING_PCLK, 0);
	Chip_PWM_LatchEnable(pwm, 1, PWM_OUT_ENABLED);
	Chip_PWM_SetControlMode(pwm, 1, PWM_SINGLE_EDGE_CONTROL_MODE, PWM_OUT_ENABLED);
	Chip_PWM_Reset(pwm);
	Chip_PWM_Enable(pwm);
}

/* Returns clock for the peripheral block */
STATIC CHIP_SYSCTL_CLOCK_T Chip_Pwm_GetClockIndex(LPC_PWM_T *pTMR)
{
	CHIP_SYSCTL_CLOCK_T clkTMR;

	if (pTMR == LPC_PWM0) {
		clkTMR = SYSCTL_CLOCK_PWM0;
	}
	else if (pTMR == LPC_PWM1) {
		clkTMR = SYSCTL_CLOCK_PWM1;
	}
	else {
		clkTMR = SYSCTL_CLOCK_PWM0;
	}

	return clkTMR;
}

/* Initialize a pwm */
void Chip_PWM_Init(LPC_PWM_T *pTMR)
{
	Chip_Clock_EnablePeriphClock(Chip_Pwm_GetClockIndex(pTMR));
}

/*      Shutdown a pwm */
void Chip_PWM_DeInit(LPC_PWM_T *pTMR)
{
	Chip_Clock_DisablePeriphClock(Chip_Pwm_GetClockIndex(pTMR));
}

/* Resets the timer terminal and prescale counts to 0 */
void Chip_PWM_Reset(LPC_PWM_T *pTMR)
{
	uint32_t reg;

	/* Disable timer, set terminal count to non-0 */
	reg = pTMR->TCR;
	pTMR->TCR = 0;
	pTMR->TC = 1;

	/* Reset timer counter */
	pTMR->TCR |= PWM_RESET;

	/* Wait for terminal count to clear */
//    while (pTMR->TC != 0) {}

	/* Restore timer state */
	pTMR->TCR = reg;
}

void Chip_PWM_SetControlMode(LPC_PWM_T *pTMR, uint8_t pwmChannel,PWM_EDGE_CONTROL_MODE pwmEdgeMode, PWM_OUT_CMD pwmCmd )
{
    if(pwmChannel)
    {
        if(pwmCmd)
        {
            pTMR->PCR |= (1<<(pwmChannel+8));
        }
        else
        {
            pTMR->PCR &= ~(1<<(pwmChannel+8));
        }

        if(pwmEdgeMode==PWM_DOUBLE_EDGE_CONTROL_MODE)
        {
            pTMR->PCR |= (1<<(pwmChannel));
        }
        else
        {
            pTMR->PCR &= ~(1<<(pwmChannel));
        }
    }
}

void Chip_PWM_LatchEnable(LPC_PWM_T *pTMR, uint8_t pwmChannel, PWM_OUT_CMD pwmCmd )
{

	if(pwmCmd)
	{
		pTMR->LER |= (1<<(pwmChannel));
	}
	else
	{
		pTMR->LER &= ~(1<<(pwmChannel));
	}
}

/* Sets the PWM Control Register */
void Chip_TIMER_PWMWrite(LPC_TIMER_T *pTMR, uint32_t pwmval) {
    pTMR->PWMC = pwmval;
}

/* Returns the PWMC Register */
uint32_t Chip_TIMER_PWMRead(LPC_TIMER_T *pTMR) {
    return pTMR->PWMC;
}
