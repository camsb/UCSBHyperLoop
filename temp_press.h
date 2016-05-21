#ifndef TEMP_PRESS_H_
#define TEMP_PRESS_H_

#include "board.h"

#define BMP_ADDRESS 0x77 // Datasheet is misleading--rightshift all bits by 1.  Determined via "probeslave"

typedef struct {
  int16_t  AC1;
  int16_t  AC2;
  int16_t  AC3;
  uint16_t AC4;
  uint16_t AC5;
  uint16_t AC6;
  int16_t  B1;
  int16_t  B2;
  int16_t  MB;
  int16_t  MC;
  int16_t  MD;
  int32_t  B5;
} constants;

constants *smooshedOne, *smooshedTwo;

float  		calculatePressure( constants * c, uint32_t uncalcPres );
float  		calculateTemperature( constants * c, uint32_t uncalcTemp );
uint32_t  	getDataValue( I2C_ID_T id, uint8_t * writeBuf, uint8_t * readBuf, uint8_t recvLen, uint8_t sendLen, uint8_t periph );
float  		getTemperature( constants * c, I2C_ID_T id );
float 		getPressure( constants * c, I2C_ID_T id );
uint16_t  	readRegs( I2C_ID_T id, uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg );
constants* 	temperaturePressureInit(I2C_ID_T id);

#endif
