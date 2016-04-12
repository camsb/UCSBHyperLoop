
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

 int main(void)
 {

    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    i2c_app_init(I2C0, SPEED_100KHZ);
    initTempPressCalibrationData();

    timer0Init();
    Photoelectric_Init();

    gyroAccelXYZ acceleration, rotation;

    while( 1 )
    {

        if(strip_detected) {
          printf("Strip %u, of %u in region %u!\n", strip_count, regional_strip_count, strip_region);
          strip_detected = 0;
        }

        sensorData.temp = getTemperature();
        DEBUGOUT( "temperature = %d\n", sensorData.temp );

        sensorData.pressure = getPressure();
        DEBUGOUT( "pressure = %u\n", sensorData.pressure );

        acceleration = getAccelerometerData();
        rotation = getGyroscopeData();

        sensorData.accelX = acceleration.x;
        DEBUGOUT( "accelX = %f\n", sensorData.accelX );
        sensorData.accelY = acceleration.y;
        DEBUGOUT( "accelY = %f\n", sensorData.accelY );
        sensorData.accelZ = acceleration.z;
        DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );

        sensorData.gyroX = rotation.x;
        DEBUGOUT( "gyroX = %f\n", sensorData.gyroX );
        sensorData.gyroY = rotation.y;
        DEBUGOUT( "gyroY = %f\n", sensorData.gyroY );
        sensorData.gyroZ = rotation.z;
        DEBUGOUT( "gyroZ = %f\n", sensorData.gyroZ );

        delay( 100 );
    }

    return 0;
 }
