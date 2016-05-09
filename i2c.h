#ifndef I2C_H_
#define I2C_H_

#include "board.h"

#define DEFAULT_I2C          I2C0
#define I2C_EEPROM_BUS       DEFAULT_I2C
#define I2C_IOX_BUS          DEFAULT_I2C
#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000

int mode_poll;   /* Poll/Interrupt mode flag */

void Hyperloop_I2C_Init(I2C_ID_T id);
void i2cInit(I2C_ID_T id, int speed);
void I2C0_IRQHandler(void);
void i2c_state_handling(I2C_ID_T id);
void i2c_set_mode(I2C_ID_T id, int polling);
void i2c_app_init(I2C_ID_T id, int speed);

#endif
