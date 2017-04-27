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
#include "subsystems.h"
#include "actuation.h"
#include "ethernet.h"

int main(void)
{
    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    // Initialize LPC_TIMER0 as a runtime timer.
    runtimeTimerInit();

    initializeCommunications();
    initializeSensorsAndControls();
    initializeSubsystemStateMachines();

    DEBUGOUT("_______________________________________\n");
    DEBUGOUT("UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n");

    // Main control loop
    while( 1 ) {
    	// 1. Service any flags set by Web App interrupts
        // 2. Gather data from sensors
        // 3. Log data to web app, SD card, etc.
        // 4. Send signals to state machine to induce transitions as necessary
        // 5. Based on flags from state machine, do actuation of subsystems
	
    	/* Handle all Wiznet Interrupts, including RECV */
		if(wizIntFlag) {
			wizIntFunction();	// See ethernet.c
		}

        // ** GATHER DATA FROM SENSORS **
        if(collectDataFlag){
            collectData(); 		// See sensor_data.c
        }
	
        // ** DATA LOGGING **
        if (COMMUNICATION_ACTIVE){
            logData(); 			// See communications.c
        }
	
        // Service high-level command routines ('go', 'all stop', 'emergency stop')

        // ** DETERMINE STATE MACHINE TRANSITIONS**
        if (collectDataFlag){
        	generate_signals_from_sensor_data(); // See subsystems.c
        }

        // ** DO ACTUATIONS FROM STATE MACHINE FLAGS **
        performActuation(); // See actuation.c

    }  // End of main control loop

    return 0;
}
