/* UC Santa Barbara Hyperloop Team
 *
 * UCSB Hyperloop Controller
 *
 * Celeste Bean
 * TV's Connor Buckland
 * Ben Hartl
 * TV's Cameron McCarthy
 * Connor Mulcahey
 *
 */

#include <stdlib.h>		// Used for rand()
#include <time.h>		// Used for rand()
#include "time.h"
#include "board.h"
#include "temp_press.h"
#include "stdlib.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "stdio.h"
#include "string.h"
#include "sensor_data.h"
#include "i2c.h"
#include "photo_electric.h"
#include "pwm.h"
#include "ethernet.h"
#include "gpio.h"
#include "ranging.h"
#include "braking.h"

int main(void)
{
    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    /* Initialize PWM for motor control. Note, PWM function is commented for usage later.*/
//    Init_PWM(LPC_PWM1);
//    Init_Channel(LPC_PWM1, 1);
    // Set_Channel_PWM(LPC_PWM1, 1, 0.5);

    /* Initialize timers for the delay function, gathering data, and sending data. */
//    delayTimerInit(LPC_TIMER0, TIMER0_IRQn, 1000);
//    gatherSensorDataTimerInit(LPC_TIMER1, TIMER1_IRQn, 8);
    sendSensorDataTimerInit(LPC_TIMER2, TIMER2_IRQn, 4);

    /* Initialize I2C and all sensors. */
//    i2cInit(I2C0, SPEED_100KHZ);
//    temperaturePressureInit();
//    photoelectricInit();
    ethernetInit(PROTO_TCP, 0);
//    rangingSensorsInit();

    DEBUGOUT("\n UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n\n");

//	sendDataFlag = 1;

//	int val_len;
//	char method[4] = {0};
//	char value[30] = {0};
//	rec_method(method, value, &val_len);
//	printf("rec: %s:%s\n", method, value);
//	printf("-----------------\n");

    /* Enable GPIO Interrupts */
    GPIO_Interrupt_Enable();

    /* Handle all Wiznet Interrupts, including RECV */
    if(wizIntFlag) {
		wizIntFunction();
	}

    time_t t;
    srand((unsigned) time(&t));
    while( 1 ) {

    	/* Need to do this cleanly, on a timer to prevent multiple attempts before a response */
    	if(!connectionOpen && !connectionClosed && sendDataFlag) {
    		sendDataFlag = 0;
    		ethernetInit(PROTO_TCP, 0);
    	}

//        if(stripDetectedFlag) {
//            stripDetected();
//        	DEBUGOUT("Strip %u, of %u in region %u!\n", strip_count, regional_strip_count, strip_region);
//        }
//
//        if(collectDataFlag){
//            collectData();
//            DEBUGOUT( "longRangingJ22 = %f\t", sensorData.longRangingJ22 );
//            DEBUGOUT( "longRangingJ25 = %f\t", sensorData.longRangingJ25 );
//            DEBUGOUT( "longRangingJ30 = %f\t", sensorData.longRangingJ30 );
//            DEBUGOUT( "longRangingJ31 = %f\n", sensorData.longRangingJ31 );
//            DEBUGOUT( "shortRangingJ34 = %f\t", sensorData.shortRangingJ34 );
//            DEBUGOUT( "shortRangingJ35 = %f\t", sensorData.shortRangingJ35 );
//            DEBUGOUT( "shortRangingJ36 = %f\t", sensorData.shortRangingJ36 );
//            DEBUGOUT( "shortRangingJ37 = %f\t", sensorData.shortRangingJ37 );
//            DEBUGOUT( "temperature = %d\n", sensorData.temp );
//            DEBUGOUT( "pressure = %u\n", sensorData.pressure );
//            DEBUGOUT( "accelX = %f\t", sensorData.accelX );
//            DEBUGOUT( "accelY = %f\t", sensorData.accelY );
//            DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
//            DEBUGOUT( "gyroX = %f\t", sensorData.gyroX );
//            DEBUGOUT( "gyroY = %f\t", sensorData.gyroY );
//            DEBUGOUT( "gyroZ = %f\n", sensorData.gyroZ );
//        }

//    	/* Handle all Wiznet Interrupts, including RECV */
        if(wizIntFlag) {
    		wizIntFunction();
    	}

    	if(/*wizIntFlag || isXferCompleted || */(sendDataFlag && connectionOpen)) {

//    		wizStateHandler(); Used for interrupt driven

//    		//sendData();
//    		DEBUGOUT( "Sending Data!\n" );

    		sendDataFlag = 0;

    		sprintf(DataPacket.bmp, "%03d.00", rand() % 500);
    		sprintf(DataPacket.tmp, "%03d.00", rand() % 300);
    		sprintf(DataPacket.pos, "%03d.00", rand() % 1000);
    		sprintf(DataPacket.vel, "%03d.00", rand() % 130);
    		sprintf(DataPacket.acc, "%03d.00", rand() % 50);
    		sprintf(DataPacket.rol, "%03d.00", (rand() % 12) - 6);
    		sprintf(DataPacket.pit, "%03d.00", (rand() % 12) - 6);
    		sprintf(DataPacket.yaw, "%03d.00", (rand() % 12) - 6);
    		send_data_packet();
    	}

//        if(emergencyBrakeFlag){
//            emergencyBrake();
//        	DEBUGOUT( "Emergency brake signal received!\n" );
//        }

    }

    return 0;
}
