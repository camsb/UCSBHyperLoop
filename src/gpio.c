#include "gpio.h"
#include "photo_electric.h"
#include "ethernet.h"
#include "timer.h"
#include "battery.h"

void GPIO_IRQHandler(void)
{
	uint32_t interrupt_bits_port0 = Chip_GPIOINT_GetStatusRising(LPC_GPIOINT, 0);
	uint32_t interrupt_bits_port2 = Chip_GPIOINT_GetStatusRising(LPC_GPIOINT, 2);


	/* Photoelectric Interrupt Pin */
	if(interrupt_bits_port2 & (1 << PHOTOELECTRIC_INT_PIN)){
		Chip_TIMER_MatchDisableInt(LPC_TIMER3, 1);
		Chip_TIMER_Disable(LPC_TIMER3);

		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, PHOTOELECTRIC_INT_PORT, 1 << PHOTOELECTRIC_INT_PIN);
		stripDetectedFlag = 1;
		stripCount++;
		regionalStripCount++;

		Reset_Timer_Counter(LPC_TIMER3);
		Chip_TIMER_Enable(LPC_TIMER3);
		Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);
	}

//	/* Battery Monitoring System Interrupt Pin */
//	if(interrupt_bits_port2 & (1 << BATT_GPIO_PIN)){
//		batteryFlag = 1;
//		DEBUGOUT( "battery interrupt!\n" );
//		processBatteryInterrupt();
//		//    uint8_t rBuffer[ 1 ];
//		//	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, SYS_STAT, rBuffer, 1 );
//		//	DEBUGOUT( "gpio_interrupt. SYS_STAT = %d\n", rBuffer[ 0 ] );
//		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, BATT_GPIO_PORT, 1 << BATT_GPIO_PIN);
//	}

	if(interrupt_bits_port2 & (1 << 11)){
	    DEBUGOUT("Interrupt!\n");
        Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 2, 1 << 11);
	}

	/* Wiznet Interrupt Pin */
	if(!(interrupt_bits_port0 & (1 << WIZNET_INT_PIN))){
		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, WIZNET_INT_PORT, 1 << WIZNET_INT_PIN);
		wizIntFlag = 1;
	}

}

void GPIO_Interrupt_Enable() {
	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(GPIO_INTERRUPT_NVIC);
	NVIC_EnableIRQ(GPIO_INTERRUPT_NVIC);
}

/* Setup GPIO port[pin] as input */
void GPIO_Input_Init(uint8_t port, uint8_t pin) {
	/* Configure photoelectric interrupt pin as output */
	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
}

/* Setup GPIO port[pin] as output */
void GPIO_Output_Init(uint8_t port, uint8_t pin) {
	/* Configure photoelectric interrupt pin as output */
	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, IOCON_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
}

