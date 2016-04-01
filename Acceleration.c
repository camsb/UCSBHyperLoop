/*
 * Acceleration.c
 */

#include "Smooshed.h"

volatile uint32_t 	NumOfMS;
uint8_t	 			SampleSetting;
volatile uint8_t 	Timer0Running;
volatile uint32_t	Timer0Count;

float* calculateAcceleration( uint8_t* uncalcAcceleration )
{
	static int16_t concAcceleration[3];
	static float acceleration[3];
	static float LSM303ACCEL_MG_LSB = (0.001F); 	// 1, 2, 4 or 12 mg per lsb
	//acceleration = uncalcAcceleration;

	// Not sure what shifting right by 4 bits does, we may want to take this out for increased
	// accuracy
	concAcceleration[0] = (int16_t)(uncalcAcceleration[0] | (uncalcAcceleration[1] << 8)) >> 4;
	concAcceleration[1] = (int16_t)(uncalcAcceleration[2] | (uncalcAcceleration[3] << 8)) >> 4;
	concAcceleration[2] = (int16_t)(uncalcAcceleration[4] | (uncalcAcceleration[5] << 8)) >> 4;

	acceleration[0] = ((float)concAcceleration[0]) * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
	acceleration[1] = ((float)concAcceleration[1]) * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;
	acceleration[2] = ((float)concAcceleration[2]) * LSM303ACCEL_MG_LSB * SENSORS_GRAVITY_STANDARD;

	DEBUGOUT( "Calculated Acceleration:\n" );
	DEBUGOUT( "X: %f, Y: %f, Z: %f\n",
			acceleration[0], acceleration[1], acceleration[2]);

	return acceleration;
}

uint8_t* getAcceleration()
{
	//Sample code has options for controlling resolution.

	uint8_t  		wBuffer[ 2 ];
	static uint8_t 	rBuffer[ 7 ];
	uint8_t 		readVal;
	float*			calcAcceleration;


	wBuffer[ 0 ] = ( LSM303_REGISTER_ACCEL_CTRL_REG1_A ); // Control register initializes all
	wBuffer[ 1 ] = 0x57;

	rBuffer[ 0 ] = LSM303_REGISTER_ACCEL_OUT_X_L_A;

	Chip_I2C_MasterSend( i2cDev, ACC_ADDRESS, wBuffer, 2 );
	Chip_I2C_MasterRead( i2cDev, ACC_ADDRESS, rBuffer, 2);

	Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_X_L_A, &rBuffer[0], 1 );
	Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_X_H_A, &rBuffer[1], 1 );
	Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_Y_L_A, &rBuffer[2], 1 );
	Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_Y_H_A, &rBuffer[3], 1 );
	Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_Z_L_A, &rBuffer[4], 1 );
	Chip_I2C_MasterCmdRead( i2cDev, ACC_ADDRESS, LSM303_REGISTER_ACCEL_OUT_Z_H_A, &rBuffer[5], 1 );

	calcAcceleration = calculateAcceleration( (uint8_t *)rBuffer);
	return (uint8_t *)rBuffer;//calculateAcceleration( c, uncalcAcceleration);
}
