/* UC Santa Barbara Hyperloop Team
 *
 * UCSB Hyperloop Controller
 *
 * Celeste "is the Coolest" Bean
 * Connor "The Gooblet" Buckland
 * Ben "Super Weenie Hut Jr" Hartl
 * Cameron "Frat Star" McCarthy
 * Connor "Leg Exploding" Mulcahey
 *
 */

#include "time.h"
#include "board.h" 
#include "temp_press.h"
#include "stdlib.h"
#include "accelerometer.h"
#include "gyroscope.h"
#include "stdio.h"
#include "sensor_data.h"
#include "i2c.h"
#include "photo_electric.h"
#include "pwm.h"
#include "ethernet.h"
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
    delayTimerInit(LPC_TIMER0, TIMER0_IRQn, 1000);
    gatherSensorDataTimerInit(LPC_TIMER1, TIMER1_IRQn, 1);
//    sendSensorDataTimerInit(LPC_TIMER2, TIMER2_IRQn, 4);

    /* Initialize I2C and all sensors. */
    i2cInit(I2C1, SPEED_100KHZ);
    temperaturePressureInit();
//    photoelectricInit();
//    ethernetInit(PROTO_UDP, 0);
//    rangingSensorsInit();

    DEBUGOUT(" UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n\n");

    collectCalibrationData();

    while( 1 )
    {
//    	collectDataFlag = 1;
//        if(stripDetectedFlag) {
//            stripDetected();
//        	DEBUGOUT("Strip %u, of %u in region %u!\n", strip_count, regional_strip_count, strip_region);
//        }

        if(collectDataFlag){
            collectData();
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
            DEBUGOUT( "accelX = %f\t", sensorData.accelX );
            DEBUGOUT( "accelY = %f\t", sensorData.accelY );
            DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
//            DEBUGOUT( "gyroX = %f\t", sensorData.gyroX );
//            DEBUGOUT( "gyroY = %f\t", sensorData.gyroY );
//            DEBUGOUT( "gyroZ = %f\n", sensorData.gyroZ );

        }

//        if(sendDataFlag){
//            sendData();
//            DEBUGOUT( "Sending Data!\n" );
//        }
//
//        if(recvDataFlag){
//            recvData();
//        	DEBUGOUT( "Receiving Data!\n" );
//        }
//
//        if(emergencyBrakeFlag){
//            emergencyBrake();
//        	DEBUGOUT( "Emergency brake signal received!\n" );
//        }

    }

    return 0;
 }
