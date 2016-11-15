#ifndef __PCB_ARDUINO_I2C_H
#define __PCB_ARDUINO_I2C_H

#include <string.h>
#include "i2c.h"

void sendPrintCmd(I2C_ID_T id, uint8_t slaveAddr, const uint8_t *buff, uint8_t len);
void sendPrint2Cmd(I2C_ID_T id, uint8_t slaveAddr);
#endif	// __PCB_ARDUINO_I2C_H
