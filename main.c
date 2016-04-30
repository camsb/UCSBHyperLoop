
#include "time.h"
#include "board.h"
#include "temp_press.h"
#include "stdlib.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "lcd.h"
#include "stdio.h"
#include "string.h"
#include "sensor_data.h"
#include "i2c.h"
#include "photo_electric.h"
#include "ethernet.h"
#include "gpio.h"

int main(void)
{
    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();
//
//    i2c_app_init(I2C0, SPEED_100KHZ);
//    initTempPressCalibrationData();
//
//    timer0Init();
//    Photoelectric_Init();
//
//
    Wiz_Khalifa(PROTO_TCP, SOCKET_ID); // WizNet initialization
//    sendSensorDataTimerInit(LPC_TIMER2, TIMER2_IRQn, 1);

    /* Enable GPIO Interrupts */
    GPIO_Interrupt_Enable();

//    printf("send: AUT:gaucholoop\n");
//    send_method(AUT, "gaucholoop", 10);
    int val_len;
    char method[4] = {0};
    char value[30] = {0};
    rec_method(method, value, &val_len);
    printf("rec: %s:%s\n", method, value);
    printf("-----------------\n");

    sendDataFlag = 1;

    while(1) {
    	if(sendDataFlag) {
    		// sendData();

    		sendDataFlag = 0;
    		send_method(BMP, "100.0", 5);
    		send_method(BMP, "101.1", 5);
    		send_method(BMP, "102.2", 5);
    		send_method(BMP, "103.3", 5);
    		send_method(BMP, "104.4", 5);
    		send_method(BMP, "105.5", 5);
    		send_method(BMP, "106.6", 5);
    		send_method(BMP, "107.7", 5);
    		send_method(BMP, "108.8", 5);
    		send_method(BMP, "109.9", 5);

    	} if(wizIntFlag) {

    		wizIntFunction();

    	}
    }

//    DEBUGOUT(" UCSB Hyperloop Controller Initialized\n");
//    DEBUGOUT("_______________________________________\n\n");
//
//    gyroAccelXYZ acceleration, rotation;

    while( 1 )
    {

//        if(strip_detected) {
//        	DEBUGOUT("Strip %u, of %u in region %u!\n", strip_count, regional_strip_count, strip_region);
//        	strip_detected = 0;
//        }
//
//        sensorData.temp = getTemperature();
//        DEBUGOUT( "temperature = %d\n", sensorData.temp );
//
//        sensorData.pressure = getPressure();
//        DEBUGOUT( "pressure = %u\n", sensorData.pressure );
//
//        acceleration = getAccelerometerData();
//        rotation = getGyroscopeData();
//
//        sensorData.accelX = acceleration.x;
//        DEBUGOUT( "accelX = %f\n", sensorData.accelX );
//        sensorData.accelY = acceleration.y;
//        DEBUGOUT( "accelY = %f\n", sensorData.accelY );
//        sensorData.accelZ = acceleration.z;
//        DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
//
//        sensorData.gyroX = rotation.x;
//        DEBUGOUT( "gyroX = %f\n", sensorData.gyroX );
//        sensorData.gyroY = rotation.y;
//        DEBUGOUT( "gyroY = %f\n", sensorData.gyroY );
//        sensorData.gyroZ = rotation.z;
//        DEBUGOUT( "gyroZ = %f\n", sensorData.gyroZ );
//
//        delay( 100 );
    }

    return 0;
}
