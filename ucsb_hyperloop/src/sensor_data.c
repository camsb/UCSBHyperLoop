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

//		sensorData.longRangingData = getLongDistance();
//		sensorData.shortRangingData = getShortDistance();
//		positionAttitude = computePositionAttitudeRanging(sensorData.longRangingData, sensorData.shortRangingData);
//
//		sensorData.positionY = positionAttitude.y;
//		sensorData.positionZ = positionAttitude.z;
//		sensorData.roll = positionAttitude.roll;
//		sensorData.pitch = positionAttitude.pitch;
//		sensorData.yaw = positionAttitude.yaw;

		float d_F = 17.0;
		float d_R = 11.0;
		float d_B = 17.0;
		float d_L = 11.0;

		// update z values to height of shortIR sensors
		float z_0 = motors[0]->short_data[0];
		float z_1 = motors[1]->short_data[0];
		float z_2 = motors[2]->short_data[0];
		float z_3 = motors[3]->short_data[0];

		// initial z values
		float z_0i = motors[0]->short_data[0];
		float z_1i = motors[1]->short_data[0];
		float z_2i = motors[2]->short_data[0];
		float z_3i = motors[3]->short_data[0];

		// d0 and d1 still need to measure these values when we do yaw sensors
		float d0 = 1.0;
		float d1 = 1.0;

		// y0i and y1i
		float y_0i = motors[0]->short_data[1];
		float y_1i = motors[1]->short_data[1];

		// y0 and y1
		float y_0 = motors[0]->short_data[1];
		float y_1 = motors[1]->short_data[1];

		float pitch_i = 0.0;
		float roll_i = 0.0;
		float z_ci = 0.0;
		//initialization calculations
		if(CALIBRATE_FLAG){
			pitch_i = (z_1i + z_2i - z_0i - z_3i) / 2*(d_F + d_B);
			roll_i = (z_0i + z_1i - z_2i - z_3i) / 2*(d_L + d_R);
			z_ci = z_0i - (d_L * roll_i) + (d_F * pitch_i);
			CALIBRATE_FLAG = 0;
		}

		// pitch

		float pitch = ((z_1 + z_2 - z_0 - z_3) / 2*(d_F + d_B)) - pitch_i;

		// roll
		float roll = ((z_0 + z_1 - z_2 - z_3) / 2*(d_L + d_R)) - roll_i;

		// COM vertical displacement
		float z_c = (z_0 - (d_L * roll) + (d_F * pitch)) - z_ci;

		// yaw
		float yaw_i = (y_0i - y_1i) / (d0 + d1);
		float yaw = ((y_0 - y_1) / (d0 + d1)) - yaw_i;

		// lateral position
		float y_ci = y_0i - (d0 * yaw_i);
		float y_c = (y_0 - (d0 * yaw)) - y_ci;


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
    			DEBUGOUT("Motor %d sensors: RPM0=%d, RPM1=%d CURRENT=%d, TEMP=%d,%d,%d,%d, SHORT=%f\n", i, motors[i]->rpm[0], motors[i]->rpm[1], motors[i]->amps, motors[i]->temperatures[0], motors[i]->temperatures[1],motors[i]->temperatures[2],motors[i]->temperatures[3], motors[i]->short_data[0]);
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
