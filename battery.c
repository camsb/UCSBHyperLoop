#include "battery.h"

/* 
 *  This will convert the msb and the lsb to either a 14 bit or 16 bit value
 *  depending on the value of the mask. ex. 0x3F will be 14, 0xFF will be 16.
 *  It only masks off the msb so the lsb are unmodified.
 *  The adcValue is the Analog to Digital converstion value that is multiplied
 *  by the 14 or 16 bit integer value that will convert it to the correct floating
 *  point value to be returned.
 */
float convertBits14or16( uint8_t msb, uint8_t lsb, uint8_t mask, float adcValue )
{
  uint16_t retValue;
  
  // can be done in one line if we want
  retValue = msb & mask;
  retValue = retValue << 8;
  retValue = retValue | lsb;
  
  return retValue * adcValue
}

/*
 *  This function will read the register values from the batteries and store
 *  them into the struct batteries located in battery.h.
 */
void getBatteryData()
{
  uint8_t   rBuffer[ 10 ];
  
  // read the msb and lsb for the 5 batteries. 10 total registers to read from
  Chip_I2C_MasterCmdRead( I2C0, BATTERY_SYS, VC1_HI, rBuffer, 10);
  
  // set each of the battery values into the batteries struct
  battery.VC1 = convertBits14or16( rBuffer[ 0 ], rBuffer[ 1 ], 0x3F, ADC_14_BIT );
  battery.VC2 = convertBits14or16( rBuffer[ 2 ], rBuffer[ 3 ], 0x3F, ADC_14_BIT );
  battery.VC3 = convertBits14or16( rBuffer[ 4 ], rBuffer[ 5 ], 0x3F, ADC_14_BIT );
  battery.VC4 = convertBits14or16( rBuffer[ 6 ], rBuffer[ 7 ], 0x3F, ADC_14_BIT );
  battery.VC5 = convertBits14or16( rBuffer[ 8 ], rBuffer[ 9 ], 0x3F, ADC_14_BIT );
  
  // read the msb and lsb for the sum of the batteries
  Chip_I2C_MasterCmdRead( I2C0, BATTERY_SYS, 0x2A, rBuffer, 2 );
  battery.BATSUM = convertBits14or16( rBuffer[ 0 ], rBuffer[ 1 ], 0xFF, ADC_16_BIT );

  return;
}
