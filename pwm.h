#ifndef PWM_H_
#define PWM_H_

#include "board.h"

typedef struct {                          /*!< TIMERn Structure       */
	__IO uint32_t ISR;                    /*!< Interrupt Status Register. The ISR can be written to clear interrupts. The ISR can be read to identify which of eight possible interrupt sources are pending. */
	__IO uint32_t TCR;                    /*!< Timer Control Register. The TCR is used to control the Timer Counter functions. The Timer Counter can be disabled or reset through the TCR. */
	__IO uint32_t TC;                     /*!< Timer Counter. The 32 bit TC is incremented every PR+1 cycles of PCLK. The TC is controlled through the TCR. */
	__IO uint32_t PR;                     /*!< Prescale Register. The Prescale Counter (below) is equal to this value, the next clock increments the TC and clears the PC. */
	__IO uint32_t PC;                     /*!< Prescale Counter. The 32 bit PC is a counter which is incremented to the value stored in PR. When the value in PR is reached, the TC is incremented and the PC is cleared. The PC is observable and controllable through the bus interface. */
	__IO uint32_t MCR;                    /*!< Match Control Register. The MCR is used to control if an interrupt is generated and if the TC is reset when a Match occurs. */
	__IO uint32_t MR0;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t MR1;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t MR2;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t MR3;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t CCR;                    /*!< Capture Control Register. The CCR controls which edges of the capture inputs are used to load the Capture Registers and whether or not an interrupt is generated when a capture takes place. */
	__IO uint32_t CR[2];                  /*!< Capture Register. CR is loaded with the value of TC when there is an event on the CAPn.0 input. */
	__IO uint32_t RESERVED[3];
	__IO uint32_t MR4;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t MR5;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t MR6;                    /*!< Match Register. MR can be enabled through the MCR to reset the TC, stop both the TC and PC, and/or generate an interrupt every time MR matches the TC. */
	__IO uint32_t PCR;
	__IO uint32_t LER;
	__IO uint32_t CTCR;
} LPC_PWM_T;

#define ON_RESET_VALUE 3500

/** PWM0, PWM1 structures */
#define LPC_PWM0 			  ((LPC_PWM_T *) LPC_PWM0_BASE)
#define LPC_PWM1 			  ((LPC_PWM_T *) LPC_PWM1_BASE)

/** Timer/counter enable bit */
#define PWM_TIM_ENABLE        ((uint32_t) (1 << 0))
/** Timer/counter reset bit */
#define PWM_TIM_RESET         ((uint32_t) (1 << 1))

/** Timer/counter enable bit */
#define PWM_ENABLE            ((uint32_t) (1 << 3))
/** Timer/counter reset bit */
#define PWM_RESET             ((uint32_t) (1 << 3))

/** Bit location for reset on MRx match, n = 0 to 3 */
#define PWM_RESET_ON_MATCH(n) (_BIT((((n) * 3) + 1)))

#define PWM_APPEND_MR_MATCH(x)  (pTMR->MR##x)

typedef enum
{
 PWM_SINGLE_EDGE_CONTROL_MODE=0,
 PWM_DOUBLE_EDGE_CONTROL_MODE=1,
} PWM_EDGE_CONTROL_MODE;

typedef enum
{
    PWM_OUT_DISABLED=0,
    PWM_OUT_ENABLED=1,
} PWM_OUT_CMD;

void Init_PWM(LPC_PWM_T * pwm);
void Init_Channel(LPC_PWM_T * pwm, uint8_t channel);
void Set_Channel_PWM(LPC_PWM_T * pwm, uint8_t channel, float duty);
void Chip_TIMER_PWMWrite(LPC_TIMER_T *pTMR, uint32_t pwmval);
uint32_t Chip_TIMER_PWMRead(LPC_TIMER_T *pTMR);

/* Initialize a pwm */
void Chip_PWM_Init(LPC_PWM_T *pTMR);

/* Shutdown a pwm */
void Chip_PWM_DeInit(LPC_PWM_T *pTMR);

/**
 * @brief       Enables the timer (starts count)
 * @param       pTMR    : Pointer to timer IP register address
 * @return      Nothing
 * @note        Enables the timer to start counting.
 */
STATIC INLINE void Chip_PWM_Enable(LPC_PWM_T *pTMR)
{
	pTMR->TCR |= PWM_TIM_ENABLE;
	pTMR->TCR |= PWM_ENABLE;
}

/**
 * @brief       Disables the timer (stops count)
 * @param       pTMR    : Pointer to timer IP register address
 * @return      Nothing
 * @note        Disables the timer to stop counting.
 */
STATIC INLINE void Chip_PWM_Disable(LPC_PWM_T *pTMR)
{
	pTMR->TCR &= ~PWM_ENABLE;
	pTMR->TCR &= ~PWM_TIM_ENABLE;
}

/**
 * @brief       Sets the prescaler value
 * @param       pTMR            : Pointer to pwm IP register address
 * @param       prescale        : Prescale value to set the prescale register to
 * @return      Nothing
 * @note        Sets the prescale count value.
 */
STATIC INLINE void Chip_PWM_PrescaleSet(LPC_PWM_T *pTMR, uint32_t prescale)
{
	pTMR->PR = prescale;
}

/**
 * @brief       Sets a timer match value
 * @param       pTMR            : Pointer to timer IP register address
 * @param       matchnum        : Match timer to set match count for
 * @param       matchval        : Match value for the selected match count
 * @return      Nothing
 * @note        Sets one of the timer match values.
 */

STATIC INLINE void Chip_PWM_SetMatch(LPC_PWM_T *pTMR, int8_t matchnum, uint32_t matchval)
{
//        pTMR->MR[matchnum] = matchval;
    switch (matchnum) {
	case 0:
		pTMR->MR0=matchval;
		break;
	case 1:
		pTMR->MR1=matchval;
		break;
	case 2:
		pTMR->MR2=matchval;
		break;
	case 3:
		pTMR->MR3=matchval;
		break;
	case 4:
		pTMR->MR4=matchval;
		break;
	case 5:
		pTMR->MR5=matchval;
		break;
	case 6:
		pTMR->MR6=matchval;
		break;
	default:
		break;
    }
//    PWM_APPEND_MR_MATCH(matchnum) =matchval;
}

/**
 * @brief       Resets the timer terminal and prescale counts to 0
 * @param       pTMR    : Pointer to timer IP register address
 * @return      Nothing
 */
void Chip_PWM_Reset(LPC_PWM_T *pTMR);

/**
 * @brief       For the specific match counter, enables reset of the terminal count register when a match occurs
 * @param       pTMR            : Pointer to timer IP register address
 * @param       matchnum        : Match timer, 0 to 3
 * @return      Nothing
 */
STATIC INLINE void Chip_PWM_ResetOnMatchEnable(LPC_PWM_T *pTMR, int8_t matchnum)
{
	pTMR->MCR |= PWM_RESET_ON_MATCH(matchnum);
}

/**
 * @brief Standard timer initial match pin state and change state
 */
typedef enum IP_PWM_PIN_MATCH_STATE {
    PWM_EXTMATCH_DO_NOTHING = 0,  /*!< Timer match state does nothing on match pin */
    PWM_EXTMATCH_CLEAR      = 1,  /*!< Timer match state sets match pin low */
    PWM_EXTMATCH_SET        = 2,  /*!< Timer match state sets match pin high */
    PWM_EXTMATCH_TOGGLE     = 3   /*!< Timer match state toggles match pin */
} PWM_PIN_MATCH_STATE_T;

void Chip_PWM_SetControlMode(LPC_PWM_T *pTMR, uint8_t pwmChannel,PWM_EDGE_CONTROL_MODE pwmEdgeMode, PWM_OUT_CMD pwmCmd );

void Chip_PWM_LatchEnable(LPC_PWM_T *pTMR, uint8_t pwmChannel, PWM_OUT_CMD pwmCmd );

#endif
