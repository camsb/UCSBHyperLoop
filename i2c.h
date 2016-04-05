#ifndef SMOOSHED_H_
#define SMOOSHED_H_

// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>
#include "board.h"

#define DEFAULT_I2C          I2C0
#define I2C_EEPROM_BUS       DEFAULT_I2C
#define I2C_IOX_BUS          DEFAULT_I2C

#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000

void    i2cInit();
void    I2C0_IRQHandler(void);

static I2C_ID_T i2cDev = DEFAULT_I2C; /* Currently active I2C device */
static int mode_poll;   /* Poll/Interrupt mode flag */

#endif
