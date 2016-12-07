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
#include "sdcard.h"
#include "communication.h"
#include "rtc.h"

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
    	gatherSensorDataTimerInit(LPC_TIMER1, TIMER1_IRQn, 10);
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

    if(SDCARD_ACTIVE) {
    	sdcardInit();
    }

    if(MOTOR_BOARD_I2C_ACTIVE) {
    	//i2cInit(I2C0, SPEED_100KHZ);
    	i2cInit(I2C1, SPEED_100KHZ);
    	i2cInit(I2C2, SPEED_100KHZ);
    	motors[0] = initialize_HEMS(I2C1,0);
    	motors[1] = initialize_HEMS(I2C1,0b01001001);
    	motors[2] = initialize_HEMS(I2C2,0);
    	motors[3] = initialize_HEMS(I2C2,1);

    	// Enable GPIO interrupt.
    	Chip_GPIOINT_SetIntRising(LPC_GPIOINT, 2, 1 << 11);
    	NVIC_ClearPendingIRQ(GPIO_IRQn);
    	NVIC_EnableIRQ(GPIO_IRQn);

    	prototypeRunFlag = 0;
    }

    DEBUGOUT("UCSB Hyperloop Controller Initialized\n");
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
        	DEBUGOUT("here\n");
    		wizIntFunction();
    	}

        /* If Data Send Requested, Send Data */
        /* Function to write to SD card and Web App will be here*/
    	if((sendDataFlag && connectionOpen)) {
    		//sendToWebAppSDCard();
    	}

    	/* Handle Photoelectric Strip Detected */
        if(stripDetectedFlag) {
            stripDetected();
        	DEBUGOUT("Strip %u, of %u in region %u!\n", stripCount, regionalStripCount, stripRegion);

        }

        if(collectDataFlag){

            collectData();

#if 0
            if (sensorData.dataPrintFlag == 2) { // Print every 2/1 = 2 seconds.
				DEBUGOUT( "temperature1 = %f\n", sensorData.temp1 );
				DEBUGOUT( "temperature2 = %f\n", sensorData.temp2 );
				DEBUGOUT( "pressure1 = %f\n", sensorData.pressure1 );
				DEBUGOUT( "pressure2 = %f\n", sensorData.pressure2 );
				DEBUGOUT( "accelX = %f\t", sensorData.accelX );
				DEBUGOUT( "accelY = %f\t", sensorData.accelY );
				DEBUGOUT( "accelZ = %f\n", sensorData.accelZ );
				DEBUGOUT( "velocityX = %f\t", sensorData.velocityX );
				DEBUGOUT( "velocityY = %f\t", sensorData.velocityY );
				DEBUGOUT( "velocityZ = %f\n", sensorData.velocityZ );
				DEBUGOUT( "positionX = %f\t", sensorData.positionX );
	            DEBUGOUT( "positionY = %f\t", sensorData.positionY );
	            DEBUGOUT( "positionZ = %f\n", sensorData.positionZ );
	            DEBUGOUT( "Roll = %f\t", sensorData.roll );
	            DEBUGOUT( "Pitch = %f\t", sensorData.pitch );
	            DEBUGOUT( "Yaw = %f\n", sensorData.yaw );
				DEBUGOUT( "\n" );
				sensorData.dataPrintFlag = 0;
            }
#endif	// 0
        }
        if(PROTOTYPE_TEST) {
    		uint8_t prototypeRunTempAlert = 0;

    		// Check if any temperature is over 80C.
    		int i, j;
    		for(i=0; i<NUM_MOTORS; i++) {
    			for(j=0; j<NUM_THERMISTORS; j++) {
    				if(motors[i]->temperatures[j] > 80) {
    					prototypeRunFlag = 0;	// The (pre)run has ended due to over-temperature.
    					prototypeRunTempAlert = 1;
    					DEBUGOUT("OVER-TEMPERATURE ALERT\n");
    					break;
    				}
    			}
    			if(prototypeRunTempAlert == 1) break;
    		}

    		// Check if prototype test is running AND temperature is below 80C.
        	if(prototypeRunTempAlert == 0 && prototypeRunFlag == 1) {
        		int time_sec = getRuntime()/1000;

				if(PROTOTYPE_PRERUN) {	// PRERUN
					if (time_sec < prototypeRunStartTime + 10) {	// Spin up to tenth power.
						motors[0]->target_throttle_voltage = 0.5;
						motors[1]->target_throttle_voltage = 0.5;
						motors[2]->target_throttle_voltage = 0.5;
						motors[3]->target_throttle_voltage = 0.5;
					} else {	// Spin down.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;

						prototypeRunFlag = 0;	// The prerun has ended.
						DEBUGOUT("PRERUN HAS ENDED\n");
					}
				} else {				// RUN
					if (time_sec < prototypeRunStartTime + 60) {	// Spin up to half power.
						motors[0]->target_throttle_voltage = 2.5;
						motors[1]->target_throttle_voltage = 2.5;
						motors[2]->target_throttle_voltage = 2.5;
						motors[3]->target_throttle_voltage = 2.5;
					} else {	// Spin down.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;

						prototypeRunFlag = 0;	// The run has ended.
						DEBUGOUT("RUN HAS ENDED\n");
					}
				}

        	} else {	// The motors should not be spinning right now.
				motors[0]->target_throttle_voltage = 0;
				motors[1]->target_throttle_voltage = 0;
				motors[2]->target_throttle_voltage = 0;
				motors[3]->target_throttle_voltage = 0;
        	}
        	DEBUGOUT("Throttle voltage: %0.2f\n", motors[0]->target_throttle_voltage);
        }

        if(SDCARD_ACTIVE) {
        	DEBUGOUT("Starting sendToWebAppSDCard\n");
        	sendToWebAppSDCard();
        	DEBUGOUT("Completed sendToWebAppSDCard\n");
        	while(1);
        }

    }

    return 0;
}
