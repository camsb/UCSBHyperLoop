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

int mode_poll;   /* Poll/Interrupt mode flag */

void i2c_state_handling(I2C_ID_T id);
void con_print_data(const uint8_t *dat, int sz);
void i2c_set_mode(I2C_ID_T id, int polling);
void i2c_app_init(I2C_ID_T id, int speed);

#endif
