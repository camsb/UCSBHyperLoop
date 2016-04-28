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

#define SOCKET_ID 0

void send_method(char *method, char* val, int val_len) {

	memset (Net_Tx_Data, 0, DATA_BUF_SIZE);
	memcpy(Net_Tx_Data, method, 3);
	Net_Tx_Data[3] = ':';
	memcpy(Net_Tx_Data + 4, val, val_len);
	Net_Tx_Data[4 + val_len] = '\n';
	int i;
	for (i = 0; i < 5 + val_len; i++) {printf("%i:%c\n", i, Net_Tx_Data[i]);}
	Wiz_Send(SOCKET_ID, Net_Tx_Data);

}

// Singular, will change to multiple, or do an interrupt or something
void rec_method(char *method, char *val, int *val_len) {

	memset (Net_Rx_Data, 0, DATA_BUF_SIZE);
	if(Wiz_Check_Socket(SOCKET_ID)) {
		Wiz_Recv(SOCKET_ID, Net_Rx_Data);
		memcpy(method, Net_Rx_Data, 3);
		method[3] = '\0';
		*val_len = 0;
		while(Net_Rx_Data[*val_len] != '\n') (*val_len)++;
		memcpy(val, Net_Rx_Data + 4, *val_len);
		val[*val_len] = '\0';
	}
}

 int main(void)
 {
	uint8_t socket_int;

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
    Wiz_Khalifa(PROTO_TCP, SOCKET_ID); // WizNet initialization

    printf("send: AUT:gaucholoop\n");
    send_method(AUT, "gaucholoop", 10);
    int val_len;
    char method[4] = {0};
    char value[30] = {0};
    rec_method(method, value, &val_len);
    printf("rec: %s:%s\n", method, value);

    while(1) {
    	if(Wiz_Int_Check()) {
    		printf("Interrupt Detected\n");
			socket_int = Wiz_Int_Clear(SOCKET_ID);
			if(socket_int & SEND_OK) {	 // Send Completed

			} if(socket_int & TIMEOUT) { // Timeout Occurred

			} if(socket_int & RECV) {	 // Packet Received

			} if(socket_int & DISCON) {	 // FIN/FIN ACK Received
				printf("Connection Closed\n");
			} if(socket_int & Sn_CON) {	 // Socket Connection Completed

			}
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
