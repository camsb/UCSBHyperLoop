#include "pwm.h"

void Set_Channel_PWM(LPC_PWM_T * pwm, uint8_t channel, float duty){
	uint32_t duty_int = ((float)ON_RESET_VALUE)*duty;
	Chip_PWM_SetMatch(pwm, channel, duty_int);
	Chip_PWM_LatchEnable(pwm, channel, PWM_OUT_ENABLED);
}

void Init_Channel(LPC_PWM_T * pwm, uint8_t channel){
	if(pwm == LPC_PWM0)
		Chip_IOCON_PinMuxSet(LPC_IOCON, 3, channel + 15, IOCON_FUNC2);
	else
		Chip_IOCON_PinMuxSet(LPC_IOCON, 3, channel + 23, IOCON_FUNC2);

	Chip_PWM_SetMatch(pwm, channel, 0);
	Chip_PWM_LatchEnable(pwm, channel, PWM_OUT_ENABLED);
	Chip_PWM_SetControlMode(pwm, channel, PWM_SINGLE_EDGE_CONTROL_MODE, PWM_OUT_ENABLED);
}

/* Initialize PWM */
void Init_PWM(LPC_PWM_T * pwm)
{
	Chip_PWM_Init(pwm);
	Chip_PWM_PrescaleSet(pwm, 0);
	Chip_PWM_SetMatch(pwm, 0, ON_RESET_VALUE);
	Chip_PWM_ResetOnMatchEnable(pwm, 0);
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
