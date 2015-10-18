#include "board.h"
#include <stdio.h>

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define TICKRATE_HZ1 (10) /* 10 ticks per second */
#define TEMP_SENSOR_ADDR    0x48
#define SPEED_100KHZ        100000
static I2C_ID_T i2cDev =    I2C0; /* Currently active I2C device */
static int mode_poll;   /* Poll/Interrupt mode flag */
int timer_flag;

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/*****************************************************************************
 * Name: TIMER0_IRQHandler
 * Purpose: This is the handler for the timer interrupt. It sets the timer_flag
 variable to begin reading from the temperature sensor.
 ****************************************************************************/

void TIMER0_IRQHandler(void)
{
  if (Chip_TIMER_MatchPending(LPC_TIMER0, 1)) {
    timer_flag = 1;
    Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
  }
}

/*****************************************************************************
 * Name: i2c_set_mode
 * Purpose: This function is used to interrupt the I2C rather than poll.
 ****************************************************************************/

static void i2c_set_mode(I2C_ID_T id, int polling)
{
  if(!polling) {
    mode_poll &= ~(1 << id);
    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
    NVIC_EnableIRQ(id == i2cDev ? I2C0_IRQn : I2C1_IRQn);
  } else {
    mode_poll |= 1 << id;
    NVIC_DisableIRQ(id == i2cDev ? I2C0_IRQn : I2C1_IRQn);
    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPolling);
  }
}

/*****************************************************************************
 * Name: set_up_i2c
 * Purpose: This function is used to interrupt the I2C rather than poll.
 ****************************************************************************/

static void set_up_i2c(I2C_ID_T id, int speed)
{
  Board_I2C_Init(id);

  /* Initialize I2C */
  Chip_I2C_Init(id);
  Chip_I2C_SetClockRate(id, speed);

  /* Set default mode to interrupt */
  i2c_set_mode(id, 1);
}

/*****************************************************************************
 * Name: read_temp_sensor
 * Purpose: This function reads from the temperature sensor and returns the
 temperature in celcius * 100, as to not using floating point data.
 ****************************************************************************/

int32_t read_temp_sensor(void){
  uint8_t temp[2];
  int16_t t = 0;
  Chip_I2C_MasterCmdRead(i2cDev, 0x48, 0x00, temp, 2);
  t = ((temp[0] << 8) | (temp[1]));
  return ((t * 100) >> 8);
}

/*****************************************************************************
 * Name: set_up_board
 * Purpose: This is a basic function which performs the two functions
 associated with setting up any project.
 ****************************************************************************/

void set_up_board(void){
  // Evaluates the clock register settings and calculates the current core clock
  SystemCoreClockUpdate();
  // Set up and initialize all required blocks and functions related to the 
  // board hardware. This includes the UART, GPIO, and LEDs
  Board_Init();
}

/*****************************************************************************
 * Name: set_up_timer
 * Purpose: The timer for LPC_TIMER0 is set up with respect to the declaration
 of TICKRATE_HZ1. The value of this declaration becomes the 
 ****************************************************************************/

void set_up_timer(void){
  uint32_t timerFreq;
  timer_flag = 0;

  // Starts up the timer
  Chip_TIMER_Init(LPC_TIMER0);

  /* Timer rate is system clock rate */
  timerFreq = Chip_Clock_GetSystemClockRate();

  // Reset the timer upon initializiation to ensure good starting point.
  Chip_TIMER_Reset(LPC_TIMER0);
  
  Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);
  Chip_TIMER_SetMatch(LPC_TIMER0, 1, (timerFreq / TICKRATE_HZ1));
  Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);
  Chip_TIMER_Enable(LPC_TIMER0);
}

/*****************************************************************************
 * Name: initialize_interrupts
 * Purpose: This function clears and enables the requested interrupt.
 ****************************************************************************/

void initialize_interrupts(void){
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  NVIC_EnableIRQ(TIMER0_IRQn);
}

/*****************************************************************************
 * Name: main
 * Purpose: The main function calls all necessary start up functions then loops
 checking the temperature sensor at a rate of 10Hz (or whatever value 
 TICKRATE_HZ1 is set to).
 ****************************************************************************/

int main(void)
{
  int32_t temp;
  printf("Setting up the board.\n");
  set_up_board();
  printf("Enabling timer 1\n");
  set_up_timer();
  printf("Setting up the interrupts\n");
  initialize_interrupts();
  printf("Initializing I2C\n");
  set_up_i2c(i2cDev, SPEED_100KHZ);

  /* Temperature Sensor Is On By Default */

  while (1) {
    if(timer_flag == 1){
      timer_flag = 0;
      temp = read_temp_sensor();
      printf("Temp is: %d\n", temp);
    }
  }

  return 0;
}



