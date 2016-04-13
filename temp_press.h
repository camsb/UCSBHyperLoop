#ifndef TEMP_PRESS_H_
#define TEMP_PRESS_H_

#include "time.h"
#include "stdint.h"
#include "i2c.h"
#include "stdlib.h"

#define BMP_ADDRESS        0x77 // Datasheet is misleading--rightshift all bits by 1.  Determined via "probeslave"

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

uint32_t  	calculatePressure( uint32_t uncalcPres );
int32_t  	calculateTemperature( uint32_t uncalcTemp );
//uint32_t  getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len); // Original declaration.
uint32_t  	getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len, uint8_t periph );
int32_t  	getTemperature();
uint32_t  	getPressure();
void    	initCalibrationData();
void    	printConstants();
uint16_t  	readRegs( uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg );
//void    	setCalibrationData( struct constants *c );
void 		initTempPressCalibrationData();

#endif
