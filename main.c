#include "time.h"
#include "board.h" 
#include "temp_press.h"
#include "stdlib.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "lcd.h"
#include "stdio.h"
#include "sensor_data.h"
#include "i2c.h"
#include "photo_electric.h"
#include "ethernet.h"
#include "ranging.h"
#include "braking.h"

 int main(void)
 {

    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    /* Initialize timers for the delay function, sending data, and gathering data. */
    timerInit(LPC_TIMER0, TIMER0_IRQn, 1000);
    timerInit(LPC_TIMER1, TIMER1_IRQn, 4);
    timerInit(LPC_TIMER2, TIMER2_IRQn, 8);

    /* Initialize I2C and all sensors. */
    i2cInit(I2C0, SPEED_100KHZ);
    temperaturePressureInit();
    photoelectricInit();
    ethernetInit(PROTO_UDP, 0);
    rangingSensorsInit();

    DEBUGOUT(" UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n\n");

    gyroAccelXYZ acceleration, rotation;

    while( 1 )
    {
        if(strip_detected) {
        	strip_detected = 0;

        	DEBUGOUT("Strip %u, of %u in region %u!\n", strip_count, regional_strip_count, strip_region);
        }

        if(gatherData){
        	gatherData = 0;

			getLongDistance(); /* Fills in global array LongRangingMovingAverage */
			sensorData.longRangingJ22 = LongRangingMovingAverage[2];
			DEBUGOUT( "longRangingJ22 = %f\t", sensorData.longRangingJ22 );
			sensorData.longRangingJ25 = LongRangingMovingAverage[0];
			DEBUGOUT( "longRangingJ25 = %f\t", sensorData.longRangingJ25 );
			sensorData.longRangingJ30 = LongRangingMovingAverage[1];
			DEBUGOUT( "longRangingJ30 = %f\t", sensorData.longRangingJ30 );
			sensorData.longRangingJ31 = LongRangingMovingAverage[3];
			DEBUGOUT( "longRangingJ31 = %f\n", sensorData.longRangingJ31 );

			getShortDistance(); /* Fills in global array ShortRangingMovingAverage */
			sensorData.shortRangingJ34 = ShortRangingMovingAverage[2];
			DEBUGOUT( "shortRangingJ34 = %f\t", sensorData.shortRangingJ34 );
			sensorData.shortRangingJ35 = ShortRangingMovingAverage[3];
			DEBUGOUT( "shortRangingJ35 = %f\t", sensorData.shortRangingJ35 );
			sensorData.shortRangingJ36 = ShortRangingMovingAverage[0];
			DEBUGOUT( "shortRangingJ36 = %f\t", sensorData.shortRangingJ36 );
			sensorData.shortRangingJ37 = ShortRangingMovingAverage[1];
			DEBUGOUT( "shortRangingJ37 = %f\t", sensorData.shortRangingJ37 );

			sensorData.temp = getTemperature();
			DEBUGOUT( "temperature = %d\n", sensorData.temp );

			sensorData.pressure = getPressure();
			DEBUGOUT( "pressure = %u\n", sensorData.pressure );

			acceleration = getAccelerometerData();
			sensorData.accelX = acceleration.x;
			DEBUGOUT( "accelX = %f\t", sensorData.accelX );
			sensorData.accelY = acceleration.y;
			DEBUGOUT( "accelY = %f\t", sensorData.accelY );
			sensorData.accelZ = acceleration.z;
			DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );

			rotation = getGyroscopeData();
			sensorData.gyroX = rotation.x;
			DEBUGOUT( "gyroX = %f\t", sensorData.gyroX );
			sensorData.gyroY = rotation.y;
			DEBUGOUT( "gyroY = %f\t", sensorData.gyroY );
			sensorData.gyroZ = rotation.z;
			DEBUGOUT( "gyroZ = %f\n", sensorData.gyroZ );
        }

        if(sendData){
        	sendData = 0;

        	DEBUGOUT( "Sending Data!\n" );
        }

        if(recvData){
        	recvData = 0;

        	DEBUGOUT( "Receiving Data!\n" );
        }

        if(emergencyBrake){
        	emergencyBrake = 0;

        	DEBUGOUT( "Emergency brake signal received!\n" );
        }

    }

    return 0;
 }
