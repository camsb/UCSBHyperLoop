/*
 * Main.c
 *
 *  Created on: Apr 1, 2016
 *      Author: benjaminhartl
 */

#include "Smooshed.h"

volatile uint32_t 	NumOfMS;
uint8_t	 			SampleSetting;
volatile uint8_t 	Timer0Running;
volatile uint32_t	Timer0Count;

int main(void)
{
    SystemCoreClockUpdate();
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
	printf("Celeste is SSOOOO lame! -Bucky\n");
	uint32_t 			temperature;
	uint32_t 			pressure;
	float*			acceleration;
	float*			rotation;
	struct constants  	*c;

	c = ( struct constants * ) calloc( 1, sizeof(struct constants) );

	i2cInit();

	initCalibrationData( c );
	timer0Init();

	//////////////////////////////////////////////////////////////////
	// get temperature
//	temperature = getTemperature( c );
//	DEBUGOUT( "temperature = %u\n", temperature );

	//////////////////////////////////////////////////////////////////
	// get pressure
//	pressure = getPressure( c );
//	DEBUGOUT( "pressure = %u\n", pressure );

	//////////////////////////////////////////////////////////////////
	// get acceleration
//	while(1) {
//			acceleration = getAcceleration();
//			delay(1);
//	}
	while( 1 )
	{
		acceleration = getAcceleration();
		rotation = getRotAcceleration();

		delay( 100 );
	}
	Chip_I2C_DeInit( I2C0 );
	timer0DeInit();

	free( c );
	//Chip_I2C_DeInit(I2C1);

	return 0;
}

