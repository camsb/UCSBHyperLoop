#include "initialization.h"
#include "sensor_data.h"
#include "accelerometer.h"
#include "time.h"
#include "ranging.h"
#include "temp_press.h"
#include "kinematics.h"
#include "pwm.h"
#include "timer.h"
#include "stdio.h"

float PWMVal = 0.3;
int rampUp = 1;
int stageTime = 0;

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

	getPressureFlag = !getPressureFlag; // Toggling between pressure and temperature register loading.

}

void TIMER1_IRQHandler(void){
	// collectDataFlag = 1;
	if(!rampUp){
		PWMVal = 0;
		printf("The system is off.\n");
	}
	else if(PWMVal >= 1){
		printf("Reached final stage, shutting off in next stage.\n");
		rampUp = 0;
	}
	else if(stageTime == 2){
		printf("Incrementing the PWM value to %f\n", PWMVal + 0.1);
		PWMVal += 0.1;
		stageTime = 0;
	}
	else{
		stageTime++;
	}

	Set_Channel_PWM(LPC_PWM1, 1, PWMVal);
	Chip_TIMER_ClearMatch( LPC_TIMER1, 1 );
	// Set_Channel_PWM(LPC_PWM1, 1, PWMVal);
	// Chip_TIMER_ClearMatch( LPC_TIMER1, 1 );

}

void gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
  	timerInit(timer, timerInterrupt, tickRate);
}
