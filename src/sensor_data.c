#include "initialization.h"
#include "sensor_data.h"
#include "accelerometer.h"
#include "timer.h"
#include "ranging.h"
#include "temp_press.h"
#include "kinematics.h"

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

    if(MOTOR_BOARD_I2C_ACTIVE) {
    	int i;
    	for(i=0; i<NUM_MOTORS; i++) {
    		update_HEMS(motors[i]);
    	}
    }

	getPressureFlag = !getPressureFlag; // Toggling between pressure and temperature register loading.

}

void TIMER1_IRQHandler(void){
	collectDataFlag = 1;
	Chip_TIMER_ClearMatch( LPC_TIMER1, 1 );
}

void gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
  	timerInit(timer, timerInterrupt, tickRate);
}
