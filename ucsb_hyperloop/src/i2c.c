#include "i2c.h"

/* Sets up board specific I2C interface */
void Hyperloop_I2C_Init(I2C_ID_T id)
{
	switch (id) {
	case I2C0:
		Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 2, (IOCON_FUNC5));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 3, (IOCON_FUNC5));
		break;

	case I2C1:
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		break;

	case I2C2:
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 11, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		break;

	default:
		return;
	}
}

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
    // Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPolling);
    Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPollingRetry);
  }
}

/* Initialize the I2C bus */
void i2c_app_init(I2C_ID_T id, int speed) {
  Hyperloop_I2C_Init(id);

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
