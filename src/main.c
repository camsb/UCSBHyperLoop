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

// Standard C/LPC40XX libraries
#include "stdlib.h"
#include "stdio.h"
#include "board.h"
#include "time.h"

// Hyperloop specific libraries
#include "initialization.h"
#include "sensor_data.h"
#include "communication.h"
#include "gpio.h"

int main(void)
{
    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    // Initialize LPC_TIMER0 as a runtime timer.
    runtimeTimerInit();

    initializeSensorsAndControls();
    initializeCommunications();

    DEBUGOUT("UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n\n");

    // Main control loop
    while( 1 ) {
        // 1. Gather data from sensors
        // 2. Evaluate state machine transition conditions and transition if necessary
        // 3. Perform actuation of systems as necessary
        // 4. Log data to web app, SD card, etc.

        // ** GATHER DATA FROM SENSORS **
        if(collectDataFlag){
            collectData(); // See sensor_data.c
        }

        // ** DATA LOGGING **
        if (ETHERNET_ACTIVE || SDCARD_ACTIVE){
            logData(); // See communications.c
        }

        // ** STATE MACHINE TRANSITIONS**
        // Do some state machining here.

        // ** DO ACTUATION OF SYSTEMS **
        // These will be based on what the state machine is.

        // Prototype test run control routine
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
					}
					else {	// Spin down
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;

						prototypeRunFlag = 0;	// The prerun has ended.
						DEBUGOUT("PRERUN HAS ENDED\n");
					}
				}
				else {  // RUN
					if (time_sec < prototypeRunStartTime + 60) {	// Spin up to half power.
						motors[0]->target_throttle_voltage = 2.5;
						motors[1]->target_throttle_voltage = 2.5;
						motors[2]->target_throttle_voltage = 2.5;
						motors[3]->target_throttle_voltage = 2.5;
					}
					else {	// Spin down.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;

						prototypeRunFlag = 0;	// The run has ended.
						DEBUGOUT("RUN HAS ENDED\n");
					}
				}

        	}
        	else {	// The motors should not be spinning right now.
				motors[0]->target_throttle_voltage = 0;
				motors[1]->target_throttle_voltage = 0;
				motors[2]->target_throttle_voltage = 0;
				motors[3]->target_throttle_voltage = 0;
        	}
        	DEBUGOUT("Throttle voltage: %0.2f\n", motors[0]->target_throttle_voltage);
        }

    // End of main control loop
    }

    return 0;
}
