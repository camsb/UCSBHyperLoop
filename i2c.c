
#include "Smooshed.h"

extern volatile uint32_t  NumOfMS     = 0;
extern uint8_t        SampleSetting = 0;
extern volatile uint8_t   Timer0Running = FALSE;
extern volatile uint32_t  Timer0Count   = 0;

/*-------- IO Expansion slave device implementation ----------*/
/* Update IN/OUT port states to real devices */
void i2c_iox_update_regs(int ops)
{
  if (ops & 1) { /* update out port */
    Board_LED_Set(0, iox_data[1] & 1);
    Board_LED_Set(1, iox_data[1] & 2);
    Board_LED_Set(2, iox_data[1] & 4);
    Board_LED_Set(3, iox_data[1] & 8);
  }

  if (ops & 2) { /* update in port */
    iox_data[0] = (uint8_t) Buttons_GetStatus();
  }
}

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

/* Get an integer input from UART */
/*static*/ int con_get_input(const char *str)
{
#ifdef DEBUG_ENABLE
  int input_valid = 0;
  int x;
  char ch[16], *ptr;
  int i = 0;

  while (!input_valid) {
    DEBUGOUT("%s", str);
    while(1) {
      /* Setting poll mode for slave is a very bad idea, it works nevertheless */
      if((mode_poll & (1 << i2cDev)) && Chip_I2C_IsStateChanged(i2cDev)) {
        Chip_I2C_SlaveStateHandler(i2cDev);
      }

      x = DEBUGIN();
      if (x == EOF) continue;
      if (i >= sizeof(ch) - 2) break;
      if ((x == '\r' || x == '\n') && i) {
        DEBUGOUT("\r\n");
        break;
      }
      if (x == '\b') {
        if (i) {
          DEBUGOUT("\033[1D \033[1D");
          i --;
        }
        continue;
      }
      DEBUGOUT("%c", x);
      ch[i++] = x;
    };
    ch[i] = 0;
    i = strtol(ch, &ptr, 0);
    if (*ptr) {
      i = 0;
      DEBUGOUT("Invalid input. Retry!\r\n");
      continue;
    }
    input_valid = 1;
  }
  return i;
#else
  static int sind = 0;
  static uint8_t val[] = {1, 1, 5, I2C_SLAVE_IOX_ADDR, 1, 1, 0};
  while(!val[sind]);

  return val[sind++];
#endif
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

/* Simulate an IO Expansion slave device */
/*static*/ void i2c_iox_init(I2C_ID_T id)
{
  Board_Buttons_Init();
  iox_xfer.slaveAddr = (I2C_SLAVE_IOX_ADDR << 1);
  i2c_iox_events(id, I2C_EVENT_DONE);
  Chip_I2C_SlaveSetup(id, I2C_SLAVE_1, &iox_xfer, i2c_iox_events, 0);
  i2c_iox_update_regs(3);
  /* Setup SysTick timer to get the button status updated at regular intervals */
  SysTick_Config(Chip_Clock_GetSystemClockRate() / 50);
}

/**
 * @brief SysTick Interrupt Handler
 * @return  Nothing
 * @note  Systick interrupt handler updates the button status
 */
void SysTick_Handler(void)
{
  i2c_iox_update_regs(2);
}

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
  /* Generic Initialization */
  //SystemCoreClockUpdate();
  //Board_Init();

  /* Initialize I2C0 */
  i2c_app_init(I2C0, SPEED_100KHZ);

  /* Simulate an EEPROM slave in I2C0 */
  //i2c_eeprom_init(I2C_EEPROM_BUS);

  /* Simulate an IO Expansion slave in I2C0 */
  //i2c_iox_init(I2C_IOX_BUS);
}

void TIMER0_IRQHandler(void)
{
  if ( Chip_TIMER_MatchPending( LPC_TIMER0, 1 ) )
  {
    if( NumOfMS > 0 ) // means timer is running
    {
      Timer0Count++;

      if( Timer0Count == NumOfMS )
      {
        // reset the timer count
        Timer0Count = 0;

        // toggle light to test
        Board_LED_Toggle( 0 );

        // release delay function while loop
        Timer0Running = FALSE;
      }
    }

    Chip_TIMER_ClearMatch( LPC_TIMER0, 1 );
  }
}

void timer0DeInit()
{
  Chip_TIMER_DeInit( LPC_TIMER0 );
}

// create timer that calls interrupt
void timer0Init()
{
    /* Enable timer 1 clock */
  uint32_t timerFreq;

    Chip_TIMER_Init( LPC_TIMER0 );

    /* Timer rate is system clock rate */
    timerFreq = Chip_Clock_GetSystemClockRate();

    /* Timer setup for match and interrupt at TICKRATE_HZ */
  Chip_TIMER_Reset( LPC_TIMER0 );
  Chip_TIMER_MatchEnableInt( LPC_TIMER0, 1 );

  // TODO: get rates for ms
  Chip_TIMER_SetMatch( LPC_TIMER0, 1, ( timerFreq / TICKRATE_HZ1 ) );
  Chip_TIMER_ResetOnMatchEnable( LPC_TIMER0, 1 );
  Chip_TIMER_Enable( LPC_TIMER0 );

  /* Enable timer interrupt */
  NVIC_ClearPendingIRQ( TIMER0_IRQn );
  NVIC_EnableIRQ( TIMER0_IRQn );

  return;
}


void delay( uint32_t ms )
{
  Timer0Running = TRUE;
  NumOfMS = ms;

  // wait for set amount of ms
  while( Timer0Running == TRUE );

  NumOfMS = 0;

  return;
}
