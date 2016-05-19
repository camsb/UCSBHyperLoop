#include "temp_press.h"
#include "time.h"
#include "i2c.h"
#include "stdlib.h"

#define SAMPLE_SETTING 0

uint16_t readRegs( I2C_ID_T id, uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg )
{
  uint8_t  buffer;
  uint16_t value;

  value = 0;

  // get MSB
  Chip_I2C_MasterCmdRead( id, slaveAddr, msbReg, &buffer, 1 );
  value = ( ( uint16_t ) buffer ) << 8;

  // get LSB
  Chip_I2C_MasterCmdRead( id, slaveAddr, lsbReg, &buffer, 1 );
  value = value | ( uint16_t ) buffer;

  return value;
}

// set the initial calibration data
constants* temperaturePressureInit(I2C_ID_T id)
{
  constants* c = ( constants * ) calloc ( 1, sizeof( constants ) );

  c->AC1 = readRegs( id, BMP_ADDRESS, 0xAA, 0xAB );
  c->AC2 = readRegs( id, BMP_ADDRESS, 0xAC, 0xAD );
  c->AC3 = readRegs( id, BMP_ADDRESS, 0xAE, 0xAF );
  c->AC4 = readRegs( id, BMP_ADDRESS, 0xB0, 0xB1 );
  c->AC5 = readRegs( id, BMP_ADDRESS, 0xB2, 0xB3 );
  c->AC6 = readRegs( id, BMP_ADDRESS, 0xB4, 0xB5 );
  c->B1  = readRegs( id, BMP_ADDRESS, 0xB6, 0xB7 );
  c->B2  = readRegs( id, BMP_ADDRESS, 0xB8, 0xB9 );
  c->MB  = readRegs( id, BMP_ADDRESS, 0xBA, 0xBB );
  c->MC  = readRegs( id, BMP_ADDRESS, 0xBC, 0xBD );
  c->MD  = readRegs( id, BMP_ADDRESS, 0xBE, 0xBF );
  c->B5  = 0; // set in getTemperature, used in getPressure

  return c;
}

uint32_t getDataValue( I2C_ID_T id, uint8_t * writeBuf, uint8_t * readBuf, uint8_t recvLen, uint8_t sendLen, uint8_t periph )
{
  int8_t   i;
  uint8_t  readVal;
  uint32_t returnVal;

  returnVal = 0;

  for( i = 0; i < recvLen; i++ )
  {
    readVal   = 0;
    Chip_I2C_MasterCmdRead( id, periph, readBuf[ i ], &readVal, 1 );
    returnVal = returnVal | ( ((uint32_t)readVal) << ( 8 * ( recvLen - i - 1 ) ) );
  }

  Chip_I2C_MasterSend( id, periph, writeBuf, sendLen );

  return returnVal;
}

float calculatePressure( constants* c, uint32_t uncalcPressure )
{
  uint32_t  pressure;
  int32_t   X1;
  int32_t   X2;
  int32_t   X3;
  int32_t   B3;
  int32_t   B6;
  uint32_t  B4;
  uint32_t  B7;

  B6 = c->B5 - 4000;

  X1 = ( c->B2 * ( ( B6 * B6 ) >> 12 ) ) >> 11;
  X2 = ( c->AC2 * B6 ) >> 11;
  X3 = X1 + X2;
  B3 = ( ( ( ( c->AC1 * 4 ) + X3 ) << SAMPLE_SETTING ) + 2 ) >> 2;
  X1 = ( c->AC3 * B6 ) >> 13;
  X2 = ( c->B1 * ( ( B6 * B6 ) >> 12 ) ) >> 16;
  X3 = ( ( X1 + X2 ) + 2 ) >> 2;
  B4 = ( c->AC4 * ( unsigned long )( X3 + 32768 ) ) >> 15;
  B7 = ( ( unsigned long ) uncalcPressure - B3 ) * ( 50000 >> SAMPLE_SETTING );

  if( B7 < 0x80000000 )
    pressure = ( B7 * 2 ) / B4;
  else
    pressure = ( B7 / B4 ) << 1;

  X1 = ( pressure >> 8 ) * ( pressure >> 8 );
  X1 = ( X1 * 3038 ) >> 16;
  X2 = ( -7357 * pressure ) >> 16;
  pressure = pressure + ( ( X1 + X2 + 3791 ) >> 4 );

  return ((float)pressure)/1000.0; // Need to divide by 1000 to return in kPa.
}

float getPressure( constants * c, I2C_ID_T id )
{
  uint32_t  uncalcPressure;
  uint8_t   wBuffer[ 2 ];
  uint8_t   rBuffer[ 3 ];

  /* Requests temperature values to be loaded. */
  /* This removes the need for a delay. */
  wBuffer[ 0 ] = 0xF4;
  wBuffer[ 1 ] = 0x2E;

  rBuffer[ 0 ] = 0xF6; // BMP085_REGISTER_PRESSUREDATA
  rBuffer[ 1 ] = 0xF7;
  rBuffer[ 2 ] = 0xF8;

  uncalcPressure = getDataValue( id, wBuffer, rBuffer, 3, 2, BMP_ADDRESS );

  // TODO: set to 0 precision
  // bit shift back, for precision
  uncalcPressure = uncalcPressure >> 8;
  return calculatePressure( c, uncalcPressure );
}

float calculateTemperature( constants* c, uint32_t uncalcTemperature )
{
  int32_t  temperature;
  int32_t   X1;
  int32_t   X2;

  X1 = ( ( uncalcTemperature - c->AC6 ) * c->AC5 ) >> 15;
  X2 = ( c->MC << 11 ) / ( X1 + c->MD );
  c->B5 = X1 + X2;
  temperature = ( c->B5 + 8 ) >> 4;

  return ((float)temperature)/10.0; // Need to divide by 10 to return units in C.
}

float getTemperature( constants * c, I2C_ID_T id )
{
	uint32_t  uncalcTemperature;
	uint8_t   wBuffer[ 2 ];
	uint8_t   rBuffer[ 2 ];

	/* Requests pressure values to be loaded. */
	wBuffer[ 0 ] = 0xF4; // BMP085_REGISTER_CONTROL -- initializes register address
	wBuffer[ 1 ] = 0x34; // BMP085_REGISTER_READPRESSURECMD -- determine which option by putting data in register

	rBuffer[ 0 ] = 0xF6;
	rBuffer[ 1 ] = 0xF7;

	uncalcTemperature = getDataValue( id, wBuffer, rBuffer, 2, 2, BMP_ADDRESS );

	return calculateTemperature( c, uncalcTemperature );
}

