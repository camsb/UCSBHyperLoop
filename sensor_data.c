#include "sensor_data.h"
#include "gyroscope.h"
#include "accelerometer.h"
#include "time.h"
#include "ranging.h"
#include "temp_press.h"
#include "kinematics.h"

void collectCalibrationData(){
	XYZ initialAccel;

	initialAccel = getInitialAccelMatrix();
	sensorData.initialAccelX = initialAccel.x;
	sensorData.initialAccelY = initialAccel.y;
	sensorData.initialAccelZ = initialAccel.z;

}

void collectData(){
  uint32_t currTime, endTime;

  collectDataFlag = 0;
  sensorData.dataPrintFlag += 1;

  XYZ acceleration, rotation, velocity, position;
//  rangingData shortRangingData, longRangingData;

//  currTime = Chip_TIMER_ReadCount(LPC_TIMER1);
  sensorData.temp1 = getTemperature(I2C1);
//  sensorData.temp2 = getTemperature(I2C2);
//  endTime = Chip_TIMER_ReadCount(LPC_TIMER1);
//  printf("Gathering the temperature data took: %d\n", endTime - currTime);

//  currTime = Chip_TIMER_ReadCount(LPC_TIMER1);
  sensorData.pressure = getPressure();
//  endTime = Chip_TIMER_ReadCount(LPC_TIMER1);
//  printf("Gathering the pressure data took: %d\n", endTime - currTime);

//  longRangingData = getLongDistance();
//  sensorData.longRangingJ25 = longRangingData.sensor0;
//  sensorData.longRangingJ30 = longRangingData.sensor1;
//  sensorData.longRangingJ22 = longRangingData.sensor2;
//  sensorData.longRangingJ31 = longRangingData.sensor3;
//
//  shortRangingData = getShortDistance();
//  sensorData.shortRangingJ36 = shortRangingData.sensor0;
//  sensorData.shortRangingJ37 = shortRangingData.sensor1;
//  sensorData.shortRangingJ34 = shortRangingData.sensor2;
//  sensorData.shortRangingJ35 = shortRangingData.sensor3;

//  currTime = Chip_TIMER_ReadCount(LPC_TIMER1);
  acceleration = getAccelerometerData();
  sensorData.accelX = acceleration.x;
  sensorData.accelY = acceleration.y;
  sensorData.accelZ = acceleration.z;
//  endTime = Chip_TIMER_ReadCount(LPC_TIMER1);
//  printf("Gathering the accelerometer data took: %d\n", endTime - currTime);

  currTime = Chip_TIMER_ReadCount(LPC_TIMER1);
  rotation = getGyroscopeData();
  sensorData.gyroX = rotation.x;
  sensorData.gyroY = rotation.y;
  sensorData.gyroZ = rotation.z;
  endTime = Chip_TIMER_ReadCount(LPC_TIMER1);
//  printf("Gathering the gyroscope data took: %d\n", endTime - currTime);

  currTime = Chip_TIMER_ReadCount(LPC_TIMER1);
  velocity = getInertialVelocity();
  sensorData.velocityX = velocity.x;
  sensorData.velocityY = velocity.y;
  sensorData.velocityZ = velocity.z;
  endTime = Chip_TIMER_ReadCount(LPC_TIMER1);
//  printf("Calculating the velocity data took: %d\n", endTime - currTime);

  currTime = Chip_TIMER_ReadCount(LPC_TIMER1);
  position = getInertialPosition();
  sensorData.positionX = position.x;
  sensorData.positionY = position.y;
  sensorData.positionZ = position.z;
  endTime = Chip_TIMER_ReadCount(LPC_TIMER1);
//  printf("Calculating the positional data took: %d\n", endTime - currTime);

}

void TIMER1_IRQHandler(void){
  collectDataFlag = 1;
  Chip_TIMER_ClearMatch( LPC_TIMER1, 1 );
}

void gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
  	timerInit(timer, timerInterrupt, tickRate);
}
