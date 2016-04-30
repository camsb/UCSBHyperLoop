#include "i2c.h"

void i2cInit(I2C_ID_T id, int speed){
	i2c_app_init(id, speed);
}

/* State machine handler for I2C0 and I2C1 */
void i2c_state_handling(I2C_ID_T id) {
  if (Chip_I2C_IsMasterActive(id)) {
    Chip_I2C_MasterStateHandler(id);
  } else {
    Chip_I2C_SlaveStateHandler(id);
  }
}

/* Set I2C mode to polling/interrupt */
void i2c_set_mode(I2C_ID_T id, int polling) {
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

/* Initialize the I2C bus */
void i2c_app_init(I2C_ID_T id, int speed) {
  Board_I2C_Init(id);

  /* Initialize I2C */
  Chip_I2C_Init(id);
  Chip_I2C_SetClockRate(id, speed);

  /* Set default mode to interrupt */
  //i2c_set_mode(id, 0);
  i2c_set_mode(id, 1); // Nah, fuck that
}

void I2C0_IRQHandler(void) {
  i2c_state_handling(I2C0);
}
