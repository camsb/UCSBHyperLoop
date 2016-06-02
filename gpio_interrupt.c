#include "gpio_interrupt.h"
#include "photo_electric.h"
#include "battery.h"

void GPIO_IRQHandler(void)
{
  uint32_t interrupt_bits = Chip_GPIOINT_GetStatusRising(LPC_GPIOINT, 2);

  if(interrupt_bits & (1 << PHOTOELECTRIC_INT_PIN)){    
    Chip_TIMER_MatchDisableInt(LPC_TIMER3, 1);
    Chip_TIMER_Disable(LPC_TIMER3);

    Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, PHOTOELECTRIC_INT_PORT, 1 << PHOTOELECTRIC_INT_PIN);
    stripDetectedFlag = 1;
    strip_count++;
    regional_strip_count++;

    Reset_Timer_Counter(LPC_TIMER3);
    Chip_TIMER_Enable(LPC_TIMER3);
    Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);
  }

  if( interrupt_bits & ( 1 << BATT_GPIO_PIN ) ) {
    batteryFlag = 1;
    Chip_GPIOINT_ClearIntStatus( LPC_GPIOINT, BATT_GPIO_PORT, 1 << BATT_GPIO_PIN );
  }

}
