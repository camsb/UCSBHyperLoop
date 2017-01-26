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
#include "qpn_port.h"
#include "state_machine.h"

void BSP_display(char const *);
void BSP_exit(void);

int main(void)
{
    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    // Initialize LPC_TIMER0 as a runtime timer.
    runtimeTimerInit();

    initializeSensorsAndControls();
    initializeCommunications();
    Hyperloop_ctor();

    DEBUGOUT("UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n\n");

    int dispatch = 0;

    // Main control loop
    while( 1 ) {
        // 1. Gather data from sensors
        // 2. Log data to web app, SD card, etc.
        // 3. Evaluate state machine transition conditions and transition if necessary. Also do actuations.

        // ** GATHER DATA FROM SENSORS **
        if(collectDataFlag){
            collectData(); // See sensor_data.c
        }

        // ** DATA LOGGING **
        if (COMMUNICATION_ACTIVE){
            logData(); // See communications.c
        }

        // ** STATE MACHINE TRANSITIONS**
        // Do some state machining here.
        // Look at sensor data to determine if a state machine transition signal should be sent.
        // Set 'dispatch' to 1 if a signal was generated.
        
        // If there is a state transition signal to dispatch, do so.
        if (dispatch){
          // Dispatch the signal
          QHsm_dispatch((QHsm *)&HSM_Hyperloop);
          dispatch = 0;
        }

        // Prototype test run control routine
        if(PROTOTYPE_TEST) {
    		uint8_t prototypeRunTempAlert = 0;

#if 0
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
#endif	// 0

    		// Check if prototype test is running AND temperature is below 80C.
        	if(prototypeRunTempAlert == 0 && prototypeRunFlag == 1) {
        		int time_sec = getRuntime()/1000;

				if(PROTOTYPE_PRERUN) {	// PRERUN
					if (time_sec < prototypeRunStartTime + 10) {	// Spin up to tenth power.
						motors[0]->target_throttle_voltage = 0.8;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;
					}
					else if (time_sec < prototypeRunStartTime + 20) {	// Spin up to tenth power.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0.8;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;
					}
					else if (time_sec < prototypeRunStartTime + 30) {	// Spin up to tenth power.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0.8;
						motors[3]->target_throttle_voltage = 0;
					}
					else if (time_sec < prototypeRunStartTime + 40) {	// Spin up to tenth power.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0.8;
					} else {	// Spin down.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;
						motors[0]->throttle_voltage = 0;
						motors[1]->throttle_voltage = 0;
						motors[2]->throttle_voltage = 0;
						motors[3]->throttle_voltage = 0;

						prototypeRunFlag = 0;	// The prerun has ended.
						DEBUGOUT("PRERUN HAS ENDED\n");
					}
				}
				else {  // RUN
					if (time_sec < prototypeRunStartTime + 60) {	// Spin up to half power.
						motors[0]->target_throttle_voltage = 4;
						motors[1]->target_throttle_voltage = 4;
						motors[2]->target_throttle_voltage = 4;
						motors[3]->target_throttle_voltage = 4;
#if 1
						motors[0]->throttle_voltage = 4;
						motors[1]->throttle_voltage = 4;
						motors[2]->throttle_voltage = 4;
						motors[3]->throttle_voltage = 4;
#endif	// 0

					} else {	// Spin down.
						motors[0]->target_throttle_voltage = 0;
						motors[1]->target_throttle_voltage = 0;
						motors[2]->target_throttle_voltage = 0;
						motors[3]->target_throttle_voltage = 0;
						motors[0]->throttle_voltage = 0;
						motors[1]->throttle_voltage = 0;
						motors[2]->throttle_voltage = 0;
						motors[3]->throttle_voltage = 0;

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
				motors[0]->throttle_voltage = 0;
				motors[1]->throttle_voltage = 0;
				motors[2]->throttle_voltage = 0;
				motors[3]->throttle_voltage = 0;
        	}
        	//DEBUGOUT("Target throttle voltages: FR%0.2f BR%0.2f FL%0.2f BL%0.2f\n", motors[0]->target_throttle_voltage, motors[1]->target_throttle_voltage, motors[2]->target_throttle_voltage, motors[3]->target_throttle_voltage);
        	//DEBUGOUT("Throttle voltages: FR%0.2f BR%0.2f FL%0.2f BL%0.2f\n", motors[0]->throttle_voltage, motors[1]->throttle_voltage, motors[2]->throttle_voltage, motors[3]->throttle_voltage);
        }

    // End of main control loop
    }

    return 0;
}

// Print a message from the state machine. TODO: Integrate into the normal printout mechanisms.
void BSP_display(char const *msg) {
    printf("%s", msg);
}

// The state machine has received an EXIT_SIG somehow. TODO: Add a guard to prevent this from happening, or remove the reference to it.
void BSP_exit(void) {
    printf("State machine has received an EXIT_SIG!");
    //exit(0);
}
