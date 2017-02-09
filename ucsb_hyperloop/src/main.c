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
#include "actuation.h"

int main(void)
{
    /* Initialize the board and clock */
    SystemCoreClockUpdate();
    Board_Init();

    // Initialize LPC_TIMER0 as a runtime timer.
    runtimeTimerInit();

    initializeSensorsAndControls();
    initializeCommunications();
    initializeStateMachine();

    DEBUGOUT("UCSB Hyperloop Controller Initialized\n");
    DEBUGOUT("_______________________________________\n");

    int dispatch = 0;

    int oldRuntime = 0;
    int step = 0;
    //enum Hyperloop_Signals profile[10] = {FORWARD_SIG, STOP_SIG, REVERSE_SIG, STOP_SIG, ENGAGE_ENGINES_SIG, ENGINES_REVED_SIG, ENGAGE_BRAKES_SIG, DISENGAGE_BRAKES_SIG, DISENGAGE_ENGINES_SIG, ENGINES_STOPPED_SIG};
    enum Hyperloop_Signals profile[2] = {ENGAGE_ENGINES_SIG, ENGINES_REVED_SIG};

    int done = 0;

    // Main control loop
    while( 1 ) {
        // 1. Gather data from sensors
        // 2. Log data to web app, SD card, etc.
        // 3. Send signals to state machine to induce transitions as necessary
        // 4. Based on flags from state machine, do actuation of subsystems
	
        // ** GATHER DATA FROM SENSORS **
        if(collectDataFlag){
            collectData(); // See sensor_data.c
        }
	
        // ** DATA LOGGING **
        if (COMMUNICATION_ACTIVE){
            logData(); // See communications.c
        }
	
        // ** DETERMINE STATE MACHINE TRANSITIONS**
        // Look at sensor data to determine if a state machine transition signal should be sent.
        // Set 'dispatch' to 1 if a signal was generated.

        // Simulate transitions to the state machine from "test profiles"
        // This section will probably get replaced with signal thresholds from actual sensors soon.
        int newRuntime = getRuntime();
        if (newRuntime > 3000 + oldRuntime){
            oldRuntime = newRuntime;
            if (step < 2){ // Don't go past the end of the array
                Q_SIG((QHsm *)&HSM_Hyperloop) = (QSignal)(profile[step]);
                step++;
                dispatch = 1;
            }
            else if (!done){
                DEBUGOUT("Test profile finished.\n");
                done = 1;
            }
        }

        // If there is a state transition signal to dispatch, do so.
        if (dispatch){
          // Dispatch the signal
          QHsm_dispatch((QHsm *)&HSM_Hyperloop);
          dispatch = 0;
        }

        // ** DO ACTUATIONS FROM STATE MACHINE FLAGS **
        performActuation();


    }  // End of main control loop

    return 0;
}
