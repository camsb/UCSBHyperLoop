#ifndef TEMP_PRESS_H_
#define TEMP_PRESS_H_

#include "time.h"
#include "stdint.h"
#include "i2c.h"

#define BMP_ADDRESS        0x77 // Datasheet is misleading--rightshift all bits by 1.  Determined via "probeslave"

typedef struct constants {
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
};

uint32_t  calculatePressure( struct constants *c, uint32_t uncalcPres );
uint32_t  calculateTemperature( struct constants *c, uint32_t uncalcTemp );
//uint32_t  getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len); // Original declaration.
uint32_t  getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len, uint8_t periph );
uint32_t  getTemperature( struct constants *c );
uint32_t  getPressure( struct constants *c );
void    initCalibrationData( struct constants *c );
void    printConstants( struct constants *c );
uint16_t  readRegs( uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg );
//void    setCalibrationData( struct constants *c );

#endif
