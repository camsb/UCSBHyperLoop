#include "led.h"

void init_LED(uint8_t port, uint8_t pin) {
	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_FUNC0));
	Chip_GPIO_WriteDirBit(LPC_GPIO, port, pin, true);
}

void on_LED(uint8_t port, uint8_t pin) {
	Chip_GPIO_WritePortBit(LPC_GPIO, port, pin, false);
}

void off_LED(uint8_t port, uint8_t pin) {
	Chip_GPIO_WritePortBit(LPC_GPIO, port, pin, true);
}
