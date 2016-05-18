#include "initialization.h"
#include "sensor_data.h"
#include "accelerometer.h"
#include "time.h"
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
	rangingData shortRangingData, longRangingData;


	if (SMOOSHED_ONE_ACTIVE) {
		sensorData.pressure = getPressure(smooshedOne, I2C1);
		sensorData.temp1 = getTemperature(smooshedOne, I2C1);

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
		sensorData.temp2 = getTemperature(smooshedTwo, I2C2);
	}

	if(RANGING_SENSORS_ACTIVE) {
		longRangingData = getLongDistance();
		sensorData.longRangingJ25 = longRangingData.sensor0;
		sensorData.longRangingJ30 = longRangingData.sensor1;
		sensorData.longRangingJ22 = longRangingData.sensor2;
		sensorData.longRangingJ31 = longRangingData.sensor3;

		shortRangingData = getShortDistance();
		sensorData.shortRangingJ36 = shortRangingData.sensor0;
		sensorData.shortRangingJ37 = shortRangingData.sensor1;
		sensorData.shortRangingJ34 = shortRangingData.sensor2;
		sensorData.shortRangingJ35 = shortRangingData.sensor3;
	}

}

void TIMER1_IRQHandler(void){
	collectDataFlag = 1;
	Chip_TIMER_ClearMatch( LPC_TIMER1, 1 );
}

void gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
  	timerInit(timer, timerInterrupt, tickRate);
}
