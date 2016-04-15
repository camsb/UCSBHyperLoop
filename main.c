//
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

// Protocol methods
#define AUT "AUT"
#define BMP "ACK"
#define BMP "BMP"
#define TMP "TMP"
#define POS "POS"
#define VEL "VEL"
#define ACC "ACC"
#define ROL "ROL"
#define PIT "PIT"
#define YAW "YAW"
#define PWR "PWR"
#define MSG "MSG"
#define PWR "PWR"
#define PASSWORD "gaucholoop"

// DATA_BUF_SIZE is the size of a packet which we don't expect to excede
uint8_t Net_Tx_Data[DATA_BUF_SIZE] = {0};
uint8_t Net_Rx_Data[DATA_BUF_SIZE] = {0};

 int main(void)
 {

    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();
//
    i2c_app_init(I2C0, SPEED_100KHZ);
//    initTempPressCalibrationData();
//
//    timer0Init();
//    Photoelectric_Init();
//
//
    uint8_t socket_id = 0;
    Wiz_Khalifa(PROTO_TCP, socket_id); // WizNet initialization

    char *message = "AUT:gaucholoop\n";
    memcpy(Net_Tx_Data, message, 16);

    Wiz_Send(socket_id, Net_Tx_Data);
    printf("Sent: %s\n", message);

    int expecting_rec = 1;
    int authenticated = 0;
    while(expecting_rec) {
    	if(Wiz_Check_Socket(socket_id)) {
    		Wiz_Recv(socket_id, Net_Rx_Data);
    		printf("Recv: %s\n", (char *)Net_Rx_Data);

    		expecting_rec = 0;
    	}
    }

    printf("Done!\n");
//
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
