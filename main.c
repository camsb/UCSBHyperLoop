/* UC Santa Barbara Hyperloop Team
 *
 * UCSB Hyperloop Controller
 *
 * Celeste Bean (minimal participation)
 * TV's Connor Buckland
 * Ben Hartl
 * TV's Cameron McCarthy
 * Connor Mulcahey
 *
 */

#include "pwm.h"
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
#include "ethernet.h"
#include "ranging.h"
#include "braking.h"
#include "battery.h"

 int main(void)
 {

    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    /* Initialize PWM for motor control. Note, PWM function is commented for usage later.*/
//    Init_PWM(LPC_PWM1);
//    Init_Channel(LPC_PWM1, 1);
//    // Set_Channel_PWM(LPC_PWM1, 1, 0.5);
//
//    /* Initialize timers for the delay function, gathering data, and sending data. */
//    delayTimerInit(LPC_TIMER0, TIMER0_IRQn, 1000);
//    gatherSensorDataTimerInit(LPC_TIMER1, TIMER1_IRQn, 8);
//    sendSensorDataTimerInit(LPC_TIMER2, TIMER2_IRQn, 4);
//
//    /* Initialize I2C and all sensors. */
    i2cInit(I2C0, SPEED_100KHZ);
//    temperaturePressureInit();
//    photoelectricInit();
//    ethernetInit(PROTO_UDP, 0);
//    rangingSensorsInit();
    DEBUGOUT("Printing works!\n");
    DEBUGOUT("Interrupt status now is: %d\n", Chip_GPIOINT_GetStatusRising(LPC_GPIOINT, 2));
    batteryInit();

    DEBUGOUT(" UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n\n");

    while( 1 )
    {

        if(stripDetectedFlag) {
            stripDetected();
        	DEBUGOUT("Strip %u, of %u in region %u!\n", strip_count, regional_strip_count, strip_region);
        }

        if(collectDataFlag){
            collectData();
            DEBUGOUT( "longRangingJ22 = %f\t", sensorData.longRangingJ22 );
            DEBUGOUT( "longRangingJ25 = %f\t", sensorData.longRangingJ25 );
            DEBUGOUT( "longRangingJ30 = %f\t", sensorData.longRangingJ30 );
            DEBUGOUT( "longRangingJ31 = %f\n", sensorData.longRangingJ31 );
            DEBUGOUT( "shortRangingJ34 = %f\t", sensorData.shortRangingJ34 );
            DEBUGOUT( "shortRangingJ35 = %f\t", sensorData.shortRangingJ35 );
            DEBUGOUT( "shortRangingJ36 = %f\t", sensorData.shortRangingJ36 );
            DEBUGOUT( "shortRangingJ37 = %f\t", sensorData.shortRangingJ37 );
            DEBUGOUT( "temperature = %d\n", sensorData.temp );
            DEBUGOUT( "pressure = %u\n", sensorData.pressure );
            DEBUGOUT( "accelX = %f\t", sensorData.accelX );
            DEBUGOUT( "accelY = %f\t", sensorData.accelY );
            DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
            DEBUGOUT( "gyroX = %f\t", sensorData.gyroX );
            DEBUGOUT( "gyroY = %f\t", sensorData.gyroY );
            DEBUGOUT( "gyroZ = %f\n", sensorData.gyroZ );
        }

        if(sendDataFlag){
            sendData();
            DEBUGOUT( "Sending Data!\n" );
        }

        if(recvDataFlag){
            recvData();
        	DEBUGOUT( "Receiving Data!\n" );
        }

        if(emergencyBrakeFlag){
            emergencyBrake();
        	DEBUGOUT( "Emergency brake signal received!\n" );
        }

        if(batteryFlag){
            getBatteryData();
            DEBUGOUT( "Battery signal received!, total voltage = %f\n",
            		battery.VC1 + battery.VC2 + battery.VC3 + battery.VC4 + battery.VC5  );
//        	DEBUGOUT( "VC1    = %f volts\n", battery.VC1 );
//        	DEBUGOUT( "VC2    = %f volts\n", battery.VC2 );
//        	DEBUGOUT( "VC3    = %f volts\n", battery.VC3 );
//        	DEBUGOUT( "VC4    = %f volts\n", battery.VC4 );
//        	DEBUGOUT( "VC5    = %f volts\n", battery.VC5 );
//        	DEBUGOUT( "GAIN   = %d micro volts\n", ( int ) ( battery.GAIN * 1000000 ) );
//        	DEBUGOUT( "OFFSET = %d milli volts\n", ( int ) ( battery.OFFSET * 1000 ) );
        }

    }

    return 0;
 }
