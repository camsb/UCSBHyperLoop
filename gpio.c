#include "gpio.h"

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

