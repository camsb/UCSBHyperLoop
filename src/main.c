/* UC Santa Barbara Hyperloop Team
 *
 * UCSB Hyperloop Controller
 *
 * Celeste "Not Again" Bean
 * Connor "TCP/IP Expert" Buckland
 * "Big" Ben Hartl
 * Cameron "Deep Fried Board" McCarthy
 * Connor "Funny Guy" Mulcahey
 *
 */

#include "initialization.h"
#include "time.h"
#include "board.h"
#include "temp_press.h"
#include "stdlib.h"
#include "accelerometer.h"
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

#include "pcb_arduino_i2c.h"
#include "HEMS.h"

int main(void)
{
	/* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    // Initialize LPC_TIMER0 as a runtime timer.
    runtimeTimerInit();

    if(PWM_ACTIVE){
        Init_PWM(LPC_PWM1);
        Init_Channel(LPC_PWM1, 1);
        Set_Channel_PWM(LPC_PWM1, 1, 0.5);
    }
    if(GATHER_DATA_ACTIVE){
    	gatherSensorDataTimerInit(LPC_TIMER1, TIMER1_IRQn, 1);
    }
    if(SMOOSHED_ONE_ACTIVE){
    	i2cInit(I2C1, SPEED_100KHZ);
    	smooshedOne = temperaturePressureInit(I2C1);
    	collectCalibrationData(I2C1);
    	getPressure(smooshedOne, I2C1);
    	getPressureFlag = 0;
    }
    if(SMOOSHED_TWO_ACTIVE){
    	i2cInit(I2C2, SPEED_100KHZ);
    	smooshedTwo = temperaturePressureInit(I2C2);
    }
    if(PHOTO_ELECTRIC_ACTIVE){
    	photoelectricInit();
    }
    if(ETHERNET_ACTIVE){
    	ethernetInit(PROTO_TCP, 0);
        sendSensorDataTimerInit(LPC_TIMER2, TIMER2_IRQn, 4);
    }
    if(RANGING_SENSORS_ACTIVE){
    	rangingSensorsInit();
    }
    if(GPIO_INT_ACTIVE){
		/* Enable GPIO Interrupts */
		GPIO_Interrupt_Enable();
    }

    /* Handle all Wiznet Interrupts, including RECV */
    if(wizIntFlag) {
		wizIntFunction();
	}

    if(PCB_ARDUINO_I2C_ACTIVE) {
    	i2cInit(I2C0, SPEED_100KHZ);
    }

    if(MOTOR_BOARD_I2C_ACTIVE) {
    //	initialize_HEMS();
    	i2cInit(I2C0, SPEED_100KHZ);
    	i2cInit(I2C1, SPEED_100KHZ);
    	i2cInit(I2C2, SPEED_100KHZ);
    	motor0 = initialize_HEMS(I2C1,0);
    	motor1 = initialize_HEMS(I2C1,0b01001001);
    	motor2 = initialize_HEMS(I2C2,0);
    	motor3 = initialize_HEMS(I2C2,1);
    }

    DEBUGOUT("\n UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n\n");

    while( 1 )
    {

    	/* Need to do this cleanly, on a timer to prevent multiple attempts before a response */
    	if(!connectionOpen && !connectionClosed && sendDataFlag) {
    		sendDataFlag = 0;
    		ethernetInit(PROTO_TCP, 0);
    	}

        /* Handle all Wiznet Interrupts, including RECV */
        if(wizIntFlag) {
    		wizIntFunction();
    	}

        /* If Data Send Requested, Send Data */
    	if((sendDataFlag && connectionOpen)) {
    		sendDataPacket();
    	}

    	/* Handle Photoelectric Strip Detected */
        if(stripDetectedFlag) {
            stripDetected();
        	DEBUGOUT("Strip %u, of %u in region %u!\n", stripCount, regionalStripCount, stripRegion);
        }

        if(collectDataFlag){

            collectData();

            if (sensorData.dataPrintFlag == 2) { // Print every 2/1 = 2 seconds.
//				DEBUGOUT( "temperature1 = %f\n", sensorData.temp1 );
//				DEBUGOUT( "temperature2 = %f\n", sensorData.temp2 );
//				DEBUGOUT( "pressure1 = %f\n", sensorData.pressure1 );
//				DEBUGOUT( "pressure2 = %f\n", sensorData.pressure2 );
//				DEBUGOUT( "accelX = %f\t", sensorData.accelX );
//				DEBUGOUT( "accelY = %f\t", sensorData.accelY );
//				DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
//				DEBUGOUT( "velocityX = %f\t", sensorData.velocityX );
//				DEBUGOUT( "velocityY = %f\t", sensorData.velocityY );
//				DEBUGOUT( "velocityZ = %f\n", sensorData.velocityZ );
//				DEBUGOUT( "positionX = %f\t", sensorData.positionX );
//	            DEBUGOUT( "positionY = %f\t", sensorData.positionY );
//	            DEBUGOUT( "positionZ = %f\n", sensorData.positionZ );
//	            DEBUGOUT( "Roll = %f\t", sensorData.roll );
//	            DEBUGOUT( "Pitch = %f\t", sensorData.pitch );
//	            DEBUGOUT( "Yaw = %f\n", sensorData.yaw );


				DEBUGOUT( "Front Left = %f\n", sensorData.shortRangingData.frontLeft );
				DEBUGOUT( "Frong Right = %f\n", sensorData.shortRangingData.frontRight );
				DEBUGOUT( "Back Left = %f\n", sensorData.shortRangingData.backLeft );
				DEBUGOUT( "Back Right = %f\n", sensorData.shortRangingData.backRight );
				DEBUGOUT( "\n" );

				sensorData.dataPrintFlag = 0;
            }

        }
        if(PCB_ARDUINO_I2C_ACTIVE) {
        	// Send print command to Arduino Slave.
        	uint8_t msg[] = "Hello world!\n";
        	sendPrintCmd(I2C0, 2, msg, 14);
        	sendPrint2Cmd(I2C0, 2);
        }
        if(MOTOR_BOARD_I2C_ACTIVE) {
            int i;
//            for(i=0; i< NUM_THERMISTORS; i++) {
//            	DEBUGOUT("Motor 0 Thermistor #%d: %d\n", i, motor0->temperatures[i]);
//            }
//            //DEBUGOUT("Motor 0 Tachometer Counter: %d\n", motor0->tachometer_counter);
//
//            for(i=0; i< NUM_THERMISTORS; i++) {
//            	DEBUGOUT("Motor 1 Thermistor #%d: %d\n", i, motor1->temperatures[i]);
//            }
//            //DEBUGOUT("Motor 1 Tachometer Counter: %d\n", motor1->tachometer_counter);
//
//            for(i=0; i< NUM_THERMISTORS; i++) {
//            	DEBUGOUT("Motor 2 Thermistor #%d: %d\n", i, motor2->temperatures[i]);
//            }
//            //DEBUGOUT("Motor 2 Tachometer Counter: %d\n", motor2->tachometer_counter);

//            for(i=0; i< NUM_THERMISTORS; i++) {
//            	DEBUGOUT("Motor 3 Thermistor #%d: %d\n", i, motor3->temperatures[i]);
//            }
//            DEBUGOUT("Motor 3 Tachometer Counter: %d\n", motor3->tachometer_counter);
        }

        if(PROTOTYPE_TEST) {
        	//DEBUGOUT("motor0->throttle_voltage = %f\n", motor0->throttle_voltage);
        	int time_sec = getRuntime()/1000;
        	if(time_sec < 10) {	// Wait.
        		motor0->target_throttle_voltage = 0;
        		motor1->target_throttle_voltage = 0;
        		motor2->target_throttle_voltage = 0;
        		motor3->target_throttle_voltage = 0;
        	} else if (time_sec < 40) {	// Spin up to half power.
        		motor0->target_throttle_voltage = 2.5;
        		motor1->target_throttle_voltage = 2.5;
        		motor2->target_throttle_voltage = 2.5;
        		motor3->target_throttle_voltage = 2.5;
//        	} else if (time_sec < 50) {	// Spin up to full power.
//        		motor0->target_throttle_voltage = 5;
//        		motor1->target_throttle_voltage = 5;
//        		motor2->target_throttle_voltage = 5;
//        		motor3->target_throttle_voltage = 5;
        	} else {	// Spin down.
        		motor0->target_throttle_voltage = 0;
        		motor1->target_throttle_voltage = 0;
        		motor2->target_throttle_voltage = 0;
        		motor3->target_throttle_voltage = 0;
        	}
        }

    }

    return 0;
}
