#include "battery.h"

/*
 *  Returns the floating point 14-bit Analog to Digital converstion. It is multiplied
 *  by the gain and added to the offset.
 */
float ADCConversion14Bit( uint8_t msb, uint8_t lsb )
{
  uint16_t retValue;

  retValue = msb & MASK_6_BIT;
  retValue = retValue << 8;
  retValue = retValue | lsb;

  return ( battery.GAIN * retValue ) + battery.OFFSET;
}

/*
 * 	Converts the alert pin to be connected to a GPIO pin, sets to input so the
 * 	MC can be alerted by the Battery Management System
 */
void batteryInit()
{
	DEBUGOUT("Initializing the battery\n");
	Chip_IOCON_PinMuxSet( LPC_IOCON, BATT_GPIO_PORT, BATT_GPIO_PIN, IOCON_FUNC0 );
	Chip_GPIO_SetPinDIRInput( LPC_GPIO, BATT_GPIO_PORT, BATT_GPIO_PIN );

	/* Configure the GPIO interrupt */
	Chip_GPIOINT_SetIntRising(LPC_GPIOINT, BATT_GPIO_PORT, 1 << BATT_GPIO_PIN);

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(BATT_NVIC);
	NVIC_EnableIRQ(BATT_NVIC);

	battery.GAIN   = getGain();
	battery.OFFSET = getOffset();

	// set the max and min limits to trigger interrupts if crossed
	setVoltageLimit( VOLTAGE_MAX, REG_OV_TRIP );
	setVoltageLimit( VOLTAGE_MIN, REG_UV_TRIP );

	// set the times that the batteries will need to be past their limits to send interrupts
	setProtectDelayTimes();

	// set the ADC_EN bit in SYS_CTRL1 to 1 to activate the over voltage and under voltage limits
	enableBit( 1 << 4, REG_SYS_CTRL1 );

	// set this register to all 0s
	resetRegister( REG_SYS_STAT );

	batteryFlag = 0;
	batteryInterruptFlag = 0;
}

// will extract the register and will enable the bit and send it back
void enableBit( uint8_t bit, uint8_t reg )
{
	uint8_t regValue;
	uint8_t wBuffer[ 2 ];

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, reg, &regValue, 1 );

	wBuffer[ 0 ] = reg;
	wBuffer[ 1 ] = regValue | bit;
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );
	return;
}

/*
 *  This function will read the register values from the batteries and store
 *  them into the batteries struct located in battery.h.
 */
void getBatteryData()
{
	uint8_t rBuffer[ 10 ];

	batteryFlag = 0;

	// read the msb and lsb for the 5 batteries. 10 total registers to read from
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_VC1_HI, rBuffer, 10 );

	// set each of the battery values into the batteries struct
	battery.VC1 = ADCConversion14Bit( rBuffer[ 0 ], rBuffer[ 1 ] );
	battery.VC2 = ADCConversion14Bit( rBuffer[ 2 ], rBuffer[ 3 ] );
	battery.VC3 = ADCConversion14Bit( rBuffer[ 4 ], rBuffer[ 5 ] );
	battery.VC4 = ADCConversion14Bit( rBuffer[ 6 ], rBuffer[ 7 ] );
	battery.VC5 = ADCConversion14Bit( rBuffer[ 8 ], rBuffer[ 9 ] );
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

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_ADCGAIN1, rBuffer, 1);
	adcGain1 = rBuffer[ 0 ];
	adcGain1 = adcGain1 & ADC_GAIN1_MASK;
	adcGain1 = adcGain1 >> 2;
	// adcGain1 = 0b000000xx

	adcGain = adcGain1 << 3;
	// adcGain = 0b000xx000

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_ADCGAIN2, rBuffer, 1);
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

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_ADCOFFSET, rBuffer, 1 );

	// range should be within -128mV and 128 mV
	adcOffset = rBuffer[ 0 ]; // in mV

	return adcOffset * mV_TO_VOLTS; // convert to volts
}

/*
 * 	This function calculates the voltage limit which extracts the 8 middle bits
 * 	out of the 16 bit value ( voltageLimit ). It is initially a float, but needs to
 * 	be converted into a 16 bit int and then an 8 bit int.
 * 	ex. voltageLimit = 0000 xxxx xxxx 0000, we want to extract the x's after
 * 	calculating the full value using the OFFSET and GAIN stored in the battery struct.
 */
uint8_t getVoltageLimit( float v )
{
	float voltageLimit;

	voltageLimit = ( v - battery.OFFSET ) / battery.GAIN;
	DEBUGOUT( "voltageLimit = %f\n", voltageLimit );

	return ( uint8_t )( ( ( uint16_t ) voltageLimit ) >> 4 );
}

// Extract the value in SYS_STAT to determine which interrupt was hit.
void processBatteryInterrupt()
{
	uint8_t regValue;

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_STAT, &regValue, 1 );
	DEBUGOUT( "Battery Interrupt! REG_SYS_STAT = 0x%X\n", regValue );

	switch( regValue )
	{
		case regValue & ALERT_CC:
			DEBUGOUT( "ALERT_CC!\n" );
			break;
		case regValue & ALERT_UV:
			DEBUGOUT( "ALERT_UV!\n" );
			break;
		case regValue & ALERT_OV:
			DEBUGOUT( "ALERT_OV!\n" );
			break;
		case regValue & ALERT_SCD:
			DEBUGOUT( "ALERT_SCD!\n" );
			break;
		case regValue & ALERT_OCD:
			DEBUGOUT( "ALERT_OCD!\n" );
			break;
	}

	// reset this variable because an interrupt was hit.
	resetRegister( REG_SYS_STAT );
}

// extracts the register and send it back exactly the same to reset its values
void resetRegister( uint8_t reg )
{
	enableBit( 0, reg );
	return;
}

// set the UV_DELAY and OV_DELAY values into the Protect Register
void setProtectDelayTimes()
{
	uint8_t regValue;
	uint8_t wBuffer[ 2 ];
	uint8_t uv2BitDelay = UV_DELAY & 0x03;
	uint8_t ov2BitDelay = OV_DELAY & 0x03;

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_PROTECT3, &regValue, 1 );

	wBuffer[ 0 ] = REG_PROTECT3;
	wBuffer[ 1 ] = ( uv2BitDelay << 6 ) | ( ov2BitDelay << 4 ) | ( regValue & 0x0F );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );
	return;
}

void setVoltageLimit( float limit, uint8_t reg )
{
	uint8_t limit8Bit;
	uint8_t wBuffer[ 2 ];

	limit8Bit = getVoltageLimit( limit );

	wBuffer[ 0 ] = reg;
	wBuffer[ 1 ] = limit8Bit;
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );
	return;
}

