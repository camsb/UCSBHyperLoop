#include "pcb_arduino_i2c.h"

void sendPrintCmd(I2C_ID_T id, uint8_t slaveAddr, const uint8_t *buff, uint8_t len) {
	DEBUGOUT("sendPrintCmd\n");
	uint8_t TxBuf[32];
	uint8_t printCmd = 1;
	memset(TxBuf, 0, sizeof TxBuf);
	memcpy(TxBuf, &printCmd, 1);
	memcpy(TxBuf+1, buff, len);
	Chip_I2C_MasterSend(id, slaveAddr, &TxBuf, len + 1);
}

void sendPrint2Cmd(I2C_ID_T id, uint8_t slaveAddr) {
	DEBUGOUT("sendPrint2Cmd\n");
	uint8_t printCmd = 2;
	Chip_I2C_MasterSend(id, slaveAddr, &printCmd, 1);
}
