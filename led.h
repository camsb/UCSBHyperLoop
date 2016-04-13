#ifndef LED_H_
#define LED_H_

//#include "stdint.h"
#include "board.h"

void init_LED(uint8_t port, uint8_t pin);
void on_LED(uint8_t port, uint8_t pin);
void off_LED(uint8_t port, uint8_t pin);

#endif /* LED_H_ */
