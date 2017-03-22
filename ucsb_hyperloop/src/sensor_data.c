#include "initialization.h"
#include "sensor_data.h"
#include "accelerometer.h"
#include "timer.h"
#include "ranging.h"
#include "temp_press.h"
#include "kinematics.h"
#include "photo_electric.h"

int y = 0;
void collectCalibrationData( I2C_ID_T id ){
	XYZ initialAccel;

	initialAccel = getInitialAccelMatrix(id);
	sensorData.initialAccelX = initialAccel.x;
	sensorData.initialAccelY = initialAccel.y;
	sensorData.initialAccelZ = initialAccel.z;

}

void collectData(){

	collectDataFlag = 0;
	sensorData.dataPrintFlag += 1;

	XYZ acceleration, velocity, position;
	//rangingData shortRangingData, longRangingData;
	positionAttitudeData positionAttitude;

	if (SMOOSHED_ONE_ACTIVE) {

		if(getPressureFlag){
			sensorData.pressure1 = getPressure(smooshedOne, I2C1);
		}
		else{
			sensorData.temp1 = getTemperature(smooshedOne, I2C1);
		}

		acceleration = getAccelerometerData(I2C1);
		sensorData.accelX = acceleration.x;
		sensorData.accelY = acceleration.y;
		sensorData.accelZ = acceleration.z;

		velocity = getInertialVelocity();
		sensorData.velocityX = velocity.x;
		sensorData.velocityY = velocity.y;
		sensorData.velocityZ = velocity.z;

		position = getInertialPosition();
		sensorData.positionX = position.x;
		sensorData.positionY = position.y;
		sensorData.positionZ = position.z;
	}

	if (SMOOSHED_TWO_ACTIVE) {

		if(getPressureFlag){
			sensorData.pressure2 = getPressure(smooshedOne, I2C2);
		}
		else{
			sensorData.temp2 = getTemperature(smooshedOne, I2C2);
		}

	}

	if(RANGING_SENSORS_ACTIVE) {

		sensorData.longRangingData = getLongDistance();
		sensorData.shortRangingData = getShortDistance();
		positionAttitude = computePositionAttitudeRanging(sensorData.longRangingData, sensorData.shortRangingData);

		sensorData.positionY = positionAttitude.y;
		sensorData.positionZ = positionAttitude.z;
		sensorData.roll = positionAttitude.roll;
		sensorData.pitch = positionAttitude.pitch;
		sensorData.yaw = positionAttitude.yaw;

	}

    if (PHOTO_ELECTRIC_ACTIVE){
        /* Handle Photoelectric Strip Detected */
        if(stripDetectedFlag) {
//            stripDetected();
        	stripDetectedFlag = 0;
            //DEBUGOUT("Strip %u, of %u in region %u!\n", stripCount, regionalStripCount, stripRegion);
            DEBUGOUT("Distance: %f feet\n", sensorData.photoelectric);
            sensorData.photoelectric+=100.0;
        }
    }

    if(MOTOR_BOARD_I2C_ACTIVE) {
    	int i;
    	for(i=0; i < NUM_MOTORS; i++) {
    		update_HEMS(motors[i]);
    	}

    	if(y%10 == 0) {
    		// Print sensor data at 1Hz.
    		int i;
    		for(i=0; i<NUM_MOTORS; i++) {
    			DEBUGOUT("Motor %d sensors: RPM=%d, CURRENT=%d, TEMP=%d,%d,%d,%d\n", i, motors[i]->rpm, motors[i]->amps, motors[i]->temperatures[0], motors[i]->temperatures[1],motors[i]->temperatures[2],motors[i]->temperatures[3]);
    		}
    		DEBUGOUT("\n");
    	}
    }


	getPressureFlag = !getPressureFlag; // Toggling between pressure and temperature register loading.

	// Currently disabled debug-out printing of data.
#if 0
    if (sensorData.dataPrintFlag == 2) { // Print every 2/1 = 2 seconds.
        DEBUGOUT( "temperature1 = %f\n", sensorData.temp1 );
        DEBUGOUT( "temperature2 = %f\n", sensorData.temp2 );
        DEBUGOUT( "pressure1 = %f\n", sensorData.pressure1 );
        DEBUGOUT( "pressure2 = %f\n", sensorData.pressure2 );
        DEBUGOUT( "accelX = %f\t", sensorData.accelX );
        DEBUGOUT( "accelY = %f\t", sensorData.accelY );
        DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
        DEBUGOUT( "velocityX = %f\t", sensorData.velocityX );
        DEBUGOUT( "velocityY = %f\t", sensorData.velocityY );
        DEBUGOUT( "velocityZ = %f\n", sensorData.velocityZ );
        DEBUGOUT( "positionX = %f\t", sensorData.positionX );
        DEBUGOUT( "positionY = %f\t", sensorData.positionY );
        DEBUGOUT( "positionZ = %f\n", sensorData.positionZ );
        DEBUGOUT( "Roll = %f\t", sensorData.roll );
        DEBUGOUT( "Pitch = %f\t", sensorData.pitch );
        DEBUGOUT( "Yaw = %f\n", sensorData.yaw );
        DEBUGOUT( "\n" );
        sensorData.dataPrintFlag = 0;
    }
#endif

}

void TIMER1_IRQHandler(void){
	collectDataFlag = 1;
	Chip_TIMER_ClearMatch( LPC_TIMER1, 1 );
}

void gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
  	timerInit(timer, timerInterrupt, tickRate);
}
