#include "sensor_data.h"
#include "gyroscope.h"
#include "accelerometer.h"
#include "time.h"
#include "ranging.h"

void collectData(){
  collectDataFlag = 0;

  gyroAccelXYZ acceleration, rotation;
  rangingData shortRangingData, longRangingData;

  sensorData.temp = getTemperature();

  sensorData.pressure = getPressure();

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

  acceleration = getAccelerometerData();
  sensorData.accelX = acceleration.x;
  sensorData.accelY = acceleration.y;
  sensorData.accelZ = acceleration.z;

  rotation = getGyroscopeData();
  sensorData.gyroX = rotation.x;
  sensorData.gyroY = rotation.y;
  sensorData.gyroZ = rotation.z;
}

void TIMER1_IRQHandler(void){
  collectDataFlag = 1;
}

void gatherSensorDataTimerInit(LPC_TIMER_T * timer, uint8_t timerInterrupt, uint32_t tickRate){
  timerInit(timer, timerInterrupt, tickRate);
}
