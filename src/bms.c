#include "bms.h"
#include "time.h"
#include "i2c.h"
#include "stdlib.h"

#define I2CADDRESS 0x08
#define TEMP_REG_1 0x2C
#define TEMP_REF_2 0x2D
#define CELL_1_VOLT_REG_1 0x0C
#define CELL_1_VOLT_REG_2 0x0D

// Check this.
#define I2CINTERFACE I2C0

// Pinouts for I2C0
// SDA -
// SCL -

void testBMS(){
    // Do all the things.

	// Read the chip die temperature
	int temp = readBMSRegs(I2CINTERFACE, I2CADDRESS, TEMP_REG_1, TEMP_REG_2);
	printf("Temp is: " + temp);

	int volt = readBMSRegs(I2CINTERFACE, I2CADDRESS, CELL_1_VOLT_REG_1, CELL_1_VOLT_REG_2);
	printf("Voltage of cell 1 is: " + volt);
}


// Read 2-byte register, taken from temp_press.c
uint16_t readBMSRegs(I2C_ID_T id, uint8_t slaveAddr, uint8_t msbReg, uint8_t lsbReg)
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


uint32_t getBMSDataValue(I2C_ID_T id, uint8_t * writeBuf, uint8_t * readBuf, uint8_t recvLen, uint8_t sendLen, uint8_t periph)
{
  int8_t   i;
  uint8_t  readVal;
  uint32_t returnVal;

  returnVal = 0;

  for( i = 0; i < recvLen; i++ )  {
    readVal   = 0;
    Chip_I2C_MasterCmdRead( id, periph, readBuf[ i ], &readVal, 1 );
    returnVal = returnVal | ( ((uint32_t)readVal) << ( 8 * ( recvLen - i - 1 ) ) );
  }

  Chip_I2C_MasterSend( id, periph, writeBuf, sendLen );

  return returnVal;
}

