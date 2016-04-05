#include "temp_press.h"

#define SAMPLE_SETTING 0

uint16_t readRegs( uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg )
{
  uint8_t  buffer;
  uint16_t value;

  value = 0;

  // get MSB
  Chip_I2C_MasterCmdRead( i2cDev, slaveAddr, msbReg, &buffer, 1 );
  value = ( ( uint16_t ) buffer ) << 8;

  // get LSB
  Chip_I2C_MasterCmdRead( i2cDev, slaveAddr, lsbReg, &buffer, 1 );
  value = value | ( uint16_t ) buffer;

  return value;
}

// set the initial calibration data
void initCalibrationData( struct constants *c )
{
  c->AC1 = readRegs( BMP_ADDRESS, 0xAA, 0xAB );
  c->AC2 = readRegs( BMP_ADDRESS, 0xAC, 0xAD );
  c->AC3 = readRegs( BMP_ADDRESS, 0xAE, 0xAF );
  c->AC4 = readRegs( BMP_ADDRESS, 0xB0, 0xB1 );
  c->AC5 = readRegs( BMP_ADDRESS, 0xB2, 0xB3 );
  c->AC6 = readRegs( BMP_ADDRESS, 0xB4, 0xB5 );
  c->B1  = readRegs( BMP_ADDRESS, 0xB6, 0xB7 );
  c->B2  = readRegs( BMP_ADDRESS, 0xB8, 0xB9 );
  c->MB  = readRegs( BMP_ADDRESS, 0xBA, 0xBB );
  c->MC  = readRegs( BMP_ADDRESS, 0xBC, 0xBD );
  c->MD  = readRegs( BMP_ADDRESS, 0xBE, 0xBF );

  c->B5  = 0; // set in getTemperature, used in getPressure
}

void printConstants( struct constants *c )
{
  DEBUGOUT( "%i\n", c->AC1 );
  DEBUGOUT( "%i\n", c->AC2 );
  DEBUGOUT( "%i\n", c->AC3 );
  DEBUGOUT( "%u\n", c->AC4 );
  DEBUGOUT( "%u\n", c->AC5 );
  DEBUGOUT( "%u\n", c->AC6 );
  DEBUGOUT( "%i\n", c->B1 );
  DEBUGOUT( "%i\n", c->B2 );
  DEBUGOUT( "%i\n", c->MB );
  DEBUGOUT( "%i\n", c->MC );
  DEBUGOUT( "%i\n", c->MD );
}

// uint32_t getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len) //Original declaration w/o peripheral selection.
uint32_t getDataValue( uint8_t * writeBuf, uint8_t * readBuf, uint8_t len, uint8_t periph )
{
  int8_t   i;
  uint32_t readVal; // TODO: This is a uint8_t in Chip_I2C_MasterCmdRead, should be fixed when everything working
  uint32_t returnVal;

  returnVal = 0;

  Chip_I2C_MasterSend( i2cDev, periph, writeBuf, len );

  // delay to allow time for data to be set in correct registers
  delay( 1 );

  for( i = 0; i < len; i++ )
  {
    readVal   = 0;

    Chip_I2C_MasterCmdRead( i2cDev, periph, readBuf[ i ], &readVal, 1 );

    readVal   = readVal & 0xFF;
    returnVal = returnVal | ( readVal << ( 8 * ( len - i - 1 ) ) );
  }

  return returnVal;
}

uint32_t calculatePressure( struct constants *c, uint32_t uncalcPressure )
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

  return pressure;
}

uint32_t getPressure( struct constants *c )
{
  uint32_t  uncalcPressure;
  uint8_t   wBuffer[ 2 ];
  uint8_t   rBuffer[ 3 ];
  uint8_t   t0;
  uint8_t   t1;

  wBuffer[ 0 ] = 0xF4; // BMP085_REGISTER_CONTROL -- initializes register address
  wBuffer[ 1 ] = 0x34; // BMP085_REGISTER_READPRESSURECMD -- determine which option by putting data in register

  rBuffer[ 0 ] = 0xF6; // BMP085_REGISTER_PRESSUREDATA
  rBuffer[ 1 ] = &t0; // was 0xF7 (just an explicit declaration of memory)
  rBuffer[ 2 ] = &t1; // was 0xF8

  uncalcPressure = getDataValue( wBuffer, rBuffer, 3, BMP_ADDRESS );

  // TODO: set to 0 precision
  // bit shift back, for precision
  uncalcPressure = uncalcPressure >> 8;
  DEBUGOUT( "uncalcPres = %u\n", uncalcPressure );

  return calculatePressure( c, uncalcPressure );
}

uint32_t calculateTemperature( struct constants *c, uint32_t uncalcTemperature )
{
  uint32_t  temperature;
  int32_t   X1;
  int32_t   X2;
  //int32_t   X3;

  X1 = ( ( uncalcTemperature - c->AC6 ) * c->AC5 ) >> 15;
  if( DEBUG ) DEBUGOUT( "X1 = %d\n", X1 );

  X2 = ( c->MC << 11 ) / ( X1 + c->MD );
  if( DEBUG ) DEBUGOUT( "X2 = %d\n", X2 );

  c->B5 = X1 + X2;
  if( DEBUG ) DEBUGOUT( "c->B5 = %d\n", c->B5 );

  temperature = ( c->B5 + 8 ) >> 4;

  DEBUGOUT( "Calculated Temperature\n" );

  return temperature;
}

uint32_t getTemperature( struct constants *c )
{
  uint32_t  uncalcTemperature;
  uint8_t   wBuffer[ 2 ];
  uint8_t   rBuffer[ 2 ];
  uint8_t   t;

  wBuffer[ 0 ] = 0xF4;
  wBuffer[ 1 ] = 0x2E;

  rBuffer[ 0 ] = 0xF6;
  //rBuffer[ 1 ] = 0xF7; // Get needs to be an arbitrary address to which the data can be written.
  rBuffer[ 1 ] = &t;

  uncalcTemperature = getDataValue( wBuffer, rBuffer, 2, BMP_ADDRESS );
  DEBUGOUT( "uncalcTemperature = %u\n", uncalcTemperature );

  return calculateTemperature( c, uncalcTemperature );
}

