#ifndef TEMP_PRESS_H_
#define TEMP_PRESS_H_

#include "board.h"


// Datasheet is misleading--rightshift all bits by 1.  Determined via "probeslave"

uint32_t  	getBMSDataValue( I2C_ID_T id, uint8_t * writeBuf, uint8_t * readBuf, uint8_t recvLen, uint8_t sendLen, uint8_t periph );
uint16_t  	readBMSRegs( I2C_ID_T id, uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg );
void        testBMS();

#endif
