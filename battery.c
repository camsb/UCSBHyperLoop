#include "battery.h"

/*
 * 	Converts the alert pin to be connected to a GPIO pin, sets to input so the
 * 	MC can be alerted by the Battery Management System
 */
void batteryInit()
{
	Chip_IOCON_PinMuxSet(    LPC_IOCON, BATT_GPIO_PORT, BATT_GPIO_PIN, IOCON_FUNC0 );
	Chip_GPIO_SetPinDIRInput( LPC_GPIO, BATT_GPIO_PORT, BATT_GPIO_PORT );
}

/* 
 *  This will convert the msb and the lsb to either a 14 bit or 16 bit value
 *  depending on the value of the mask. ex. 0x3F will be 14, 0xFF will be 16.
 *  It only masks off the msb so the lsb are unmodified.
 *  The adcValue is the Analog to Digital converstion value that is multiplied
 *  by the 14 or 16 bit integer value that will convert it to the correct floating
 *  point value to be returned.
 */
float convertBits14or16( uint8_t msb, uint8_t lsb, uint8_t mask )
{
  uint16_t retValue;
  
  retValue = msb & mask;
  retValue = retValue << 8;
  retValue = retValue | lsb;
  
  return ( battery.GAIN * retValue ) + battery.OFFSET;
}

/*
 *  This function will read the register values from the batteries and store
 *  them into the batteries struct located in battery.h.
 */
void getBatteryData()
{
  uint8_t rBuffer[ 10 ];
  uint8_t wBuffer[ 2 ];
  uint8_t sys_ctrl1_reg;
  uint8_t overVoltageThreshold;
  uint8_t underVoltageThreshold;

  // TODO: may want to divide up this function into the init function

  battery.GAIN   = getGain();
  battery.OFFSET = getOffset();

  overVoltageThreshold  = getVoltageThreshold( VOLTAGE_MAX );
  DEBUGOUT( "8-bit overVoltageThreshold in Hex  = 0x%X\n", overVoltageThreshold );

  underVoltageThreshold = getVoltageThreshold( VOLTAGE_MIN );
  DEBUGOUT( "8-bit underVoltageThreshold in Hex = 0x%X\n", underVoltageThreshold );

  //  TODO: want to make sure these values are correct before we set them in the registers.
  //  send these values to their corresponding registers
  //  wBuffer[ 0 ] = OV_TRIP;
  //  wBuffer[ 1 ] = overVoltageThreshold;
  //  Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

  //  wBuffer[ 0 ] = UV_TRIP;
  //  wBuffer[ 1 ] = underVoltageThreshold;
  //  Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

  //  TODO: set the ADC_EN bit to 1 to activate these thresholds
  //  to set the ADC_EN bit, need to pull the register value out before performing bit operation
  //  Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, SYS_CTRL1, rBuffer, 1 );
  //  sys_ctrl1_reg = rBuffer[ 0 ];
  //  sys_ctrl1_reg = sys_ctrl1_reg | ADC_EN_BIT;

  //  wBuffer[ 0 ] = SYS_CTRL1;
  //  wBuffer[ 1 ] = sys_ctrl1_reg;
  //  Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

  // read the msb and lsb for the 5 batteries. 10 total registers to read from
  Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, VC1_HI, rBuffer, 10 );

  // set each of the battery values into the batteries struct
  battery.VC1 = convertBits14or16( rBuffer[ 0 ], rBuffer[ 1 ], MASK_6_BIT );
  battery.VC2 = convertBits14or16( rBuffer[ 2 ], rBuffer[ 3 ], MASK_6_BIT );
  battery.VC3 = convertBits14or16( rBuffer[ 4 ], rBuffer[ 5 ], MASK_6_BIT );
  battery.VC4 = convertBits14or16( rBuffer[ 6 ], rBuffer[ 7 ], MASK_6_BIT );
  battery.VC5 = convertBits14or16( rBuffer[ 8 ], rBuffer[ 9 ], MASK_6_BIT );

  // read the msb and lsb for the sum of the batteries
  // TODO: STUB, BATSUM is not correct
  battery.BATSUM = -1;
  //  Chip_I2C_MasterCmdRead( I2C0, BATT_ADDRESS, BAT_HI, rBuffer, 2 );
  //  battery.BATSUM = convertBits14or16( rBuffer[ 0 ], rBuffer[ 1 ], MASK_8_BIT );
}

/*
 *	The value for the gain is 5 bits long and is stored in the two separate
 *	registers. This is used for calculating the voltage for each of the battery cells.
 */
float getGain()
{
	uint8_t		rBuffer[ 1 ];
    uint8_t     adcGain1;
    uint8_t 	adcGain2;
    uint16_t 	adcGain;

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, ADCGAIN1, rBuffer, 1);
	adcGain1 = rBuffer[ 0 ];
	adcGain1 = adcGain1 & ADC_GAIN1_MASK;
	adcGain1 = adcGain1 >> 2;
	// adcGain1 = 0b000000xx

	adcGain = adcGain1 << 3;
	// adcGain = 0b000xx000

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, ADCGAIN2, rBuffer, 1);
	adcGain2 = rBuffer[ 0 ];
	adcGain2 = adcGain2 & ADC_GAIN2_MASK;
	adcGain2 = adcGain2 >> 5;
	// adcGain2 = 0b00000xxx

	adcGain = adcGain | adcGain2;
	// adcGain = 0b000xxxxx

	// range should be within 365uV and 396uV
	adcGain = adcGain + ADC_GAIN_MIN; // in uV

	return adcGain * uV_TO_VOLTS; // convert to volts
}

float getOffset()
{
	uint8_t		rBuffer[ 1 ];
    int8_t 		adcOffset;

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, ADCOFFSET, rBuffer, 1 );

	// range should be within -128mV and 128 mV
	adcOffset = rBuffer[ 0 ]; // in mV

	return adcOffset * mV_TO_VOLTS; // convert to volts
}

/*
 * 	This function calculates the Voltage Threshold which extracts the 8 middle bits
 * 	out of the 16 bit value ( fullVoltageThreshold ).
 * 	ex. fullVoltageThreshold = 0000 xxxx xxxx 0000, we want to extract the x's after
 * 	calculating the full value using the OFFSET and GAIN stored in the battery struct.
 */
uint8_t getVoltageThreshold( float v )
{
	float	 fullVoltageThreshold;

	fullVoltageThreshold = ( v - battery.OFFSET ) / battery.GAIN;
	DEBUGOUT( "fullVoltageThreshold = %f\n", fullVoltageThreshold );

	return ( ( uint8_t ) fullVoltageThreshold ) >> 4;
}

