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
#define I2CADDRESS 0x08
#define TEMP_REG_1 0x2C
#define TEMP_REG_2 0x2D
#define CELL_1_VOLT_REG_1 0x0C
#define CELL_1_VOLT_REG_2 0x0D

#define I2CINTERFACE I2C0

// I2C0 located on J174
// SCL = pin 1
// SDA = pin 2
// GND = pin 3
// 3.3V = pin 4

void testBMS(){
    // Do all the things.

    // Read the chip die temperature
    int temp = readBMSRegs(I2CINTERFACE, I2CADDRESS, TEMP_REG_1, TEMP_REG_2);
    printf("Temp is: %d \n", temp);

    int volt = readBMSRegs(I2CINTERFACE, I2CADDRESS, CELL_1_VOLT_REG_1, CELL_1_VOLT_REG_2);
    printf("Voltage of cell 1 is: %d \n", volt);
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



/*
 * 	Converts the alert pin to be connected to a GPIO pin, sets to input so the
 * 	MC can be alerted by the Battery Management System
 */
void batteryInit()
{
//	batteryFlag = 1;
	batteryFlag = 0;
	uint8_t regValue;
	uint8_t wBuffer[ 2 ];

	DEBUGOUT("Initializing the battery\n");

    // Read the chip die temperature
    int temp = readBMSRegs(I2C0, 0x08, 0x2C, 0x2D);
    DEBUGOUT("Temp is: %d \n", temp);


	Chip_IOCON_PinMuxSet( LPC_IOCON, BATT_GPIO_PORT, BATT_GPIO_PIN, IOCON_FUNC0 );
	Chip_GPIO_SetPinDIRInput( LPC_GPIO, BATT_GPIO_PORT, BATT_GPIO_PIN );

	/////////////////////////////////////////////////////
	// enable DSG_ON
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
	wBuffer[ 0 ] = REG_SYS_CTRL2;
//	wBuffer[ 1 ] = regValue | 0b010;
	wBuffer[ 1 ] = 0xFF;
	DEBUGOUT( "ENABLE DSG_ON Send Back = %x\n", wBuffer[ 1 ] );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_STAT, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
	DEBUGOUT( "ENABLE DSG_ON Read Back = %x\n", regValue );
	/////////////////////////////////////////////////////

	/* Configure the GPIO interrupt */
//	Chip_GPIOINT_SetIntRising(LPC_GPIOINT, BATT_GPIO_PORT, 1 << BATT_GPIO_PIN);
//
//	/* Enable interrupt in the NVIC */
//	NVIC_ClearPendingIRQ(BATT_NVIC);
//	NVIC_EnableIRQ(BATT_NVIC);

	battery.GAIN   = getGain();
	battery.OFFSET = getOffset();

	// set the max and min limits to trigger interrupts if crossed
	setVoltageLimit( VOLTAGE_MAX, REG_OV_TRIP );
	setVoltageLimit( VOLTAGE_MIN, REG_UV_TRIP );

	// set the times that the batteries will need to be past their limits to send interrupts
//	setProtectDelayTimes();

	// set the ADC_EN bit in SYS_CTRL1 to 1 to activate the over voltage and under voltage limits
	enableMask( ADC_EN, REG_SYS_CTRL1 );

	// Coulomb Counter Reading:
	// NOT 1 Shot: whenever we want
		//	disableMask( CC_EN, REG_SYS_CTRL2 );
		//	enableMask( CC_ONESHOT, REG_SYS_CTRL2 );
	// Always On mode: every 250ms
		//	enableMask( CC_EN, REG_SYS_CTRL2 );

	// Discharge FET driver or discharge signal control


	// enable CC bit
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
	wBuffer[ 0 ] = REG_SYS_CTRL2;
	wBuffer[ 1 ] = regValue | 0x40;
	DEBUGOUT( "ENABLE CC Send Back = %x\n", wBuffer[ 1 ] );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
	DEBUGOUT( "ENABLE CC Read Back = %x\n", regValue );

	// enable DSG_ON
//	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
//	wBuffer[ 0 ] = REG_SYS_CTRL2;
//	wBuffer[ 1 ] = ( ( regValue | 0x02 ) & 0xFE ) & 0xBF;
//	DEBUGOUT( "ENABLE DSG_ON, DISABLE CHG_ON Send Back = %x\n", wBuffer[ 1 ] );
//	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );
//
//	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
//	DEBUGOUT( "ENABLE DSG_ON, DISABLE CHG_ON Read Back = %x\n", regValue );

	/////////////////////////////////////////////////////////////////
	// disable CHG_ON
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
	wBuffer[ 0 ] = REG_SYS_CTRL2;
	wBuffer[ 1 ] = regValue & 0xFE;
	DEBUGOUT( "DISABLE CHG_ON Send Back = %x\n", wBuffer[ 1 ] );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );
	DEBUGOUT( "DISABLE CHG_ON Read Back = %x\n", regValue );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_STAT, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_STAT, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_STAT, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );

	//

	// enable DSG_ON
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
	wBuffer[ 0 ] = REG_SYS_CTRL2;
//	wBuffer[ 1 ] = regValue | 0b010;
	wBuffer[ 1 ] = 2;
	DEBUGOUT( "ENABLE DSG_ON Send Back = %x\n", wBuffer[ 1 ] );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_STAT, &regValue, 1 );
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
	DEBUGOUT( "ENABLE DSG_ON Read Back = %x\n", regValue );
	/////////////////////////////////////////////////////////////////


	// disable CC bit
//	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
//	wBuffer[ 0 ] = REG_SYS_CTRL2;
//	wBuffer[ 1 ] = regValue & 0xBF;
//	DEBUGOUT( "DISABLE CC Send Back = %x\n", wBuffer[ 1 ] );
//	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );
//
//	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1 );
//	DEBUGOUT( "DISABLE CC Read Back = %x\n", regValue );

	//	 enableMask( DSG_ON, REG_SYS_CTRL2 );
//	 disableMask( CHG_ON, REG_SYS_CTRL2 );

		// enableMask( CHG_ON, REG_SYS_CTRL2 );
		// disableMask( DSG_ON, REG_SYS_CTRL2 );

	// set this register to all 0s by sending itself back
	resetRegister( REG_SYS_STAT );
}

// will extract the register and will disable the mask and send it back
void disableMask( uint8_t mask, uint8_t reg )
{
	uint8_t regValue;
	uint8_t wBuffer[ 2 ];

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, reg, &regValue, 1 );

	wBuffer[ 0 ] = reg;
	wBuffer[ 1 ] = regValue & ( ! mask );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );
	return;
}

// will extract the register and will enable the mask and send it back
void enableMask( uint8_t mask, uint8_t reg )
{
	uint8_t regValue;
	uint8_t wBuffer[ 2 ];

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, reg, &regValue, 1 );

	wBuffer[ 0 ] = reg;
	wBuffer[ 1 ] = regValue | mask;
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

//	batteryFlag = 1;
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
	uint8_t  	readValue;
    uint8_t  	adcGain1;
    uint8_t		adcGain2;
    uint16_t 	adcGain;

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_ADCGAIN1, &readValue, 1);
	adcGain1 = readValue;
	adcGain1 = adcGain1 & ADC_GAIN1_MASK;
	adcGain1 = adcGain1 >> 2;
	// adcGain1 = 0b000000xx

	adcGain = adcGain1 << 3;
	// adcGain = 0b000xx000

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_ADCGAIN2, &readValue, 1);
	adcGain2 = readValue;
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
	uint8_t		readValue;
    int8_t 		adcOffset;

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_ADCOFFSET, &readValue, 1 );

	// range should be within -128mV and 128 mV
	adcOffset = readValue; // in mV

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

	return ( uint8_t )( ( ( uint16_t ) voltageLimit ) >> 4 );
}

void printRegValues()
{
	uint8_t regValue;

//	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_, &readValue, 1 );
//	DEBUGOUT( "regValue = %d", regValue );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL1, &regValue, 1 );
	DEBUGOUT( "LOAD_PRESENT  = %d\n", ( regValue & LOAD_PRESENT ) >> 7 );

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_CTRL2, &regValue, 1);
	DEBUGOUT( "DSG_ON        = %d\n", ( regValue & DSG_ON ) >> 1 );
	DEBUGOUT( "CHG_ON        = %d\n", ( regValue & CHG_ON ) >> 0 );
}


// Extract the value in SYS_STAT to determine which interrupt was hit.
void processBatteryInterrupt()
{
	uint8_t regValue;

	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_SYS_STAT, &regValue, 1 );

	if( regValue == 0 )
	{
		DEBUGOUT( "REG_SYS_STAT  = 0x%X\n\n", regValue );
	}

	// Check which interrupt regValue was
	if( regValue & ALERT_CC )
	{
		DEBUGOUT( "ALERT_CC: Coulomb Counter!\n" );
	}
	if( regValue & DEVICE_XREADY )
	{
		DEBUGOUT( "DEVICE_XREADY: Internal Chip Fault!\n" );
		DEBUGOUT( "               Disables CHG_ON and DSG_ON to 0!\n" );
	}
	if( regValue & OVRD_ALERT )
	{
		DEBUGOUT( "OVRD_ALERT: Pin is forced high externally while low!\n" );
		DEBUGOUT( "            Disables CHG_ON and DSG_ON to 0!\n" );
	}
	if( regValue & ALERT_UV )
	{
		DEBUGOUT( "ALERT_UV: Under Voltage!\n" );
		DEBUGOUT( "          Disables DSG_ON to 0\n" );
	}
	if( regValue & ALERT_OV )
	{
		DEBUGOUT( "ALERT_OV: Over Voltage!\n" );
		DEBUGOUT( "          Disables CHG_ON to 0\n" );
	}
	if( regValue & ALERT_SCD )
	{
		DEBUGOUT( "ALERT_SCD: Short Circuit Discharge!\n" );
		DEBUGOUT( "           Disables DSG_ON to 0\n" );
	}
	if( regValue & ALERT_OCD )
	{
		DEBUGOUT( "ALERT_OCD: Over Current in Discharge!\n" );
		DEBUGOUT( "           Disables DSG_ON to 0\n" );
	}


	// reset this variable because an interrupt was hit.
	resetRegister( REG_SYS_STAT );
}

// extracts the register and send it back exactly the same to reset its values
void resetRegister( uint8_t reg )
{
	enableMask( 0, reg );
	return;
}
/*
 * Set the delay values into the Protect Registers to delay for values defined as
 * macros in the header file. These delay times are used to make sure the values
 * are accurate before sending an alert.
 */
void setProtectDelayTimes()
{
	uint8_t regValue;
	uint8_t wBuffer[ 2 ];
	uint8_t uv2BitDelay  = DELAY_UV  & 0x03;
	uint8_t ov2BitDelay  = DELAY_OV  & 0x03;
	uint8_t scd2BitDelay = DELAY_SCD & 0x03;
	uint8_t ocd3BitDelay = DELAY_OCD & 0x07;

	// set Under Voltage and Over Voltage delay
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_PROTECT3, &regValue, 1 );
	wBuffer[ 0 ] = REG_PROTECT3;
	wBuffer[ 1 ] = ( uv2BitDelay << 6 ) | ( ov2BitDelay << 4 ) | ( regValue & 0x0F );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

	// set the Overcurrent in discharge delay
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_PROTECT2, &regValue, 1 );
	wBuffer[ 0 ] = REG_PROTECT2;
	wBuffer[ 1 ] = ( regValue & 0x8F ) | ( ocd3BitDelay << 4 );
	Chip_I2C_MasterSend( BATT_I2C, BATT_ADDRESS, wBuffer, 2 );

	// set the Short Circuit Discharge delay
	Chip_I2C_MasterCmdRead( BATT_I2C, BATT_ADDRESS, REG_PROTECT1, &regValue, 1 );
	wBuffer[ 0 ] = REG_PROTECT1;
	wBuffer[ 1 ] = ( regValue & 0xE7 ) | ( scd2BitDelay << 3 );
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

