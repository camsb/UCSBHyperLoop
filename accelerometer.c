/*
===============================================================================
 Name        : proj2.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
#include <stdio.h>
#include <cr_section_macros.h>

#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000
#define TICKRATE_HZ1 (5)  /* 5 ticks per second, 200ms period */

#define ACC_I2C_ADDR    (0x1D)
#define ACC_I2C_ADDR_R  0x3A
#define ACC_I2C_ADDR_W  0x3B

#define ACC_ADDR_XOUTL  0x00
#define ACC_ADDR_XOUTH  0x01
#define ACC_ADDR_YOUTL  0x02
#define ACC_ADDR_YOUTX  0x03
#define ACC_ADDR_ZOUTL  0x04
#define ACC_ADDR_ZOUTH  0x05
#define ACC_ADDR_XOUT8  0x06
#define ACC_ADDR_YOUT8  0x07
#define ACC_ADDR_ZOUT8  0x08
#define ACC_ADDR_STATUS 0x09
#define ACC_ADDR_DETSRC 0x0A
#define ACC_ADDR_TOUT   0x0B
#define ACC_ADDR_I2CAD  0x0D
#define ACC_ADDR_USRINF 0x0E
#define ACC_ADDR_WHOAMI 0x0F
#define ACC_ADDR_XOFFL  0x10
#define ACC_ADDR_XOFFH  0x11
#define ACC_ADDR_YOFFL  0x12
#define ACC_ADDR_YOFFH  0x13
#define ACC_ADDR_ZOFFL  0x14
#define ACC_ADDR_ZOFFH  0x15
#define ACC_ADDR_MCTL   0x16
#define ACC_ADDR_INTRST 0x17
#define ACC_ADDR_CTL1   0x18
#define ACC_ADDR_CTL2   0x19
#define ACC_ADDR_LDTH   0x1A
#define ACC_ADDR_PDTH   0x1B
#define ACC_ADDR_PW     0x1C
#define ACC_ADDR_LT     0x1D
#define ACC_ADDR_TW     0x1E

#define ACC_STATUS_DRDY 0x01
#define ACC_STATUS_DOVR 0x02
#define ACC_STATUS_PERR 0x04

int timer_flag;
static int mode_poll;   /* Poll/Interrupt mode flag */

/*void SysTick_Handler(void)
{
  Board_LED_Toggle(0);
}*/
void TIMER0_IRQHandler(void)
{
  if (Chip_TIMER_MatchPending(LPC_TIMER0, 1)) {
    timer_flag = 1;
    Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
  }
}

/* Initialize accelerometer */
void acc_init (void)
{
    /* set to measurement mode by default */
    uint8_t buf[2];

    buf[0] = ACC_ADDR_MCTL;
    buf[1] = 0x05; // Set range to 2G, mode to measure
    /* buf[1]: 0x0(gm)
     *
     * ACC_MODE_STANDBY == 00
     * ACC_MODE_MEASURE == 01
     * ACC_MODE_LEVEL   == 10 (level detection)
     * ACC_MODE_PULSE   == 11 (pulse detection)
     *
     * ACC_RANGE_8G   == 00
     * ACC_RANGE_2G   == 01
     * ACC_RANGE_4G   == 10
     *
     */
        //(ACC_MCTL_MODE(ACC_MODE_MEASURE)
        //| ACC_MCTL_GLVL(ACC_RANGE_2G));
    Chip_I2C_MasterSend(I2C0, ACC_I2C_ADDR, buf, 2);
}

/* Determine if slave ready to report acceleration */
static uint8_t slave_status(void)
{
    uint8_t cmd, buf[1];
    cmd = ACC_ADDR_STATUS;

    Chip_I2C_MasterCmdRead(I2C0, ACC_I2C_ADDR, cmd, buf, 1);
    return buf[0];
}

/* Read x,y,z acceleration from slave accelerometer.
 * Should be read individually to prevent reading clash */
void acc_read (int8_t *x, int8_t *y, int8_t *z)
{
  uint8_t cmd, buf[1];

    /* wait for ready flag */
    while ((slave_status() & ACC_STATUS_DRDY) == 0);

    // Send command to read x acceleration from slave
    cmd = ACC_ADDR_XOUT8;
    Chip_I2C_MasterCmdRead(I2C0, ACC_I2C_ADDR, cmd, buf, 1);
    *x = (int8_t)buf[0];

    // Send command to read y acceleration from slave
    cmd = ACC_ADDR_YOUT8;
    Chip_I2C_MasterCmdRead(I2C0, ACC_I2C_ADDR, cmd, buf, 1);
    *y = (int8_t)buf[0];

    // Send command to read z acceleration from slave
    cmd = ACC_ADDR_ZOUT8;
    Chip_I2C_MasterCmdRead(I2C0, ACC_I2C_ADDR, cmd, buf, 1);
    *z = (int8_t)buf[0];
}

/* Set I2C mode to polling/interrupt */
static void i2c_set_mode(I2C_ID_T id, int polling)
{
  if(!polling) {
    mode_poll &= ~(1 << id);
    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
    NVIC_EnableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
  } else {
    mode_poll |= 1 << id;
    NVIC_DisableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPolling);
  }
}

/* Main function */
int main(void) {
  // Read clock settings and update SystemCoreClock variable
  SystemCoreClockUpdate();
  // Set up and initialize all required blocks and
  // functions related to the board hardware
  Board_Init();

  printf("Board and clock initialized\n");

  int8_t numOfLoops = 10;
  int8_t i = 0;

  int32_t xoff, yoff, zoff;
  int8_t x_read, y_read, z_read;
  double xAcc, yAcc, zAcc;
  double xVel, yVel, zVel;
  double x, y, z;
  double cycles;
  //int8_t x[ numOfLoops ], y[ numOfLoops ], z[ numOfLoops ];
  uint32_t timerFreq;
  double dt, acc_to_metric, time_to_metric;

  printf("Enabling timer 1\n");
    /* Enable timer 1 clock */
    Chip_TIMER_Init(LPC_TIMER0);
    timer_flag = 0;

    /* Timer rate is system clock rate */
    timerFreq = Chip_Clock_GetSystemClockRate();
    acc_to_metric = 0.153;
    time_to_metric = 1.0/((double)timerFreq);

    /* Timer setup for match and interrupt at TICKRATE_HZ */
  Chip_TIMER_Reset(LPC_TIMER0);
  Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);
  Chip_TIMER_SetMatch(LPC_TIMER0, 1, (timerFreq / TICKRATE_HZ1));
  //Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);
  Chip_TIMER_Enable(LPC_TIMER0);

  printf("Enabling timer interrupts\n");
  /* Enable timer interrupt */
  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  NVIC_EnableIRQ(TIMER0_IRQn);

  printf("Initializing I2C\n");
    /* Initialize I2C */
    Board_I2C_Init(I2C0); // Pins
    Chip_I2C_Init(I2C0); // Clock and operation
    Chip_I2C_SetClockRate(I2C0, SPEED_100KHZ);

    /* Set I2C to Polling/Interrupt mode */
    // Polling = 1
    // Interrupt = 0
    i2c_set_mode(I2C0, 1);

    //Initialize accelerometer
    acc_init();

    //Read from accelerometer
    acc_read(&x_read, &y_read, &z_read);
    xoff = 0-x_read;
    yoff = 0-y_read;
    zoff = 0-z_read;
    printf("Initial conditions: x=%d, y=%d, z=%d\n", (int)x_read, (int)y_read, (int)z_read);

    xVel = 0;
  yVel = 0;
  zVel = 0;

  x = 0;
  y = 0;
  z = 0;

    // Set LED 0 to "On" state
    Board_LED_Set(0, true);

    //SysTick_Config(SystemCoreClock / TICKRATE_HZ1);
    while( 1 ) {
      if(timer_flag) {
        timer_flag = 0;
        Board_LED_Toggle(0);
      acc_read(&x_read, &y_read, &z_read);

      cycles = ((double)Chip_TIMER_ReadCount(LPC_TIMER0));
      Chip_TIMER_Reset(LPC_TIMER0);

      dt = ((double)cycles)*time_to_metric;

      xAcc = ((double)(x_read+xoff))*acc_to_metric;
      yAcc = ((double)(y_read+yoff))*acc_to_metric;
      zAcc = ((double)(z_read+zoff))*acc_to_metric;

      xVel = xVel + cycles*xAcc*dt;
      yVel = yVel + cycles*yAcc*dt;
      zVel = zVel + cycles*zAcc*dt;

      x = x + xVel*cycles*dt;
      y = y + yVel*cycles*dt;
      z = z + zVel*cycles*dt;

      i++;

      // print out x, y, and z every numOfLoops
      if( i % numOfLoops == 0 )
      {
        printf( "x=%f, y=%f, z=%f,  at time %f\n", x, y, z, cycles);
        i = 0;
      }
      }
      //__WFI();
    }
    return 0 ;
}
