
#include "i2c.h"

/* State machine handler for I2C0 and I2C1 */
/*static*/ void i2c_state_handling(I2C_ID_T id)
{
  if (Chip_I2C_IsMasterActive(id)) {
    Chip_I2C_MasterStateHandler(id);
  } else {
    Chip_I2C_SlaveStateHandler(id);
  }
}

/* Print data to console */
/*static*/ void con_print_data(const uint8_t *dat, int sz)
{
  int i;
  if (!sz) return;
  for (i = 0; i < sz; i++) {
    if (!(i & 0xF)) DEBUGOUT("\r\n%02X: ", i);
    DEBUGOUT(" %02X", dat[i]);
  }
  DEBUGOUT("\r\n");
}

/* Set I2C mode to polling/interrupt */
/*static*/ void i2c_set_mode(I2C_ID_T id, int polling)
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

/* Initialize the I2C bus */
/*static*/ void i2c_app_init(I2C_ID_T id, int speed)
{
  Board_I2C_Init(id);

  /* Initialize I2C */
  Chip_I2C_Init(id);
  Chip_I2C_SetClockRate(id, speed);

  /* Set default mode to interrupt */
  //i2c_set_mode(id, 0);
  i2c_set_mode(id, 1); // Nah, fuck that
}

/**
 * @brief SysTick Interrupt Handler
 * @return  Nothing
 * @note  Systick interrupt handler updates the button status
 */
// void SysTick_Handler(void)
// {
//   i2c_iox_update_regs(2);
// }

/**
 * @brief I2C0 Interrupt handler
 * @return  None
 */
void I2C0_IRQHandler(void)
{
  i2c_state_handling(I2C0);
}

void i2cInit()
{

  /* Initialize I2C0 */
  i2c_app_init(I2C0, SPEED_100KHZ);

}
