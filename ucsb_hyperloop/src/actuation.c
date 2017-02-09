#include "state_machine.h"
#include "initialization.h"

void performActuation(){
    // Actuate subsystems based on flags from state machine.

#if NO_OLD_PROTOTYPE_RUN
    // State machine test routine
    if(STATEMACHINE_TEST) {
        // Apply changes if a transition occurred
        if(HSM_Hyperloop.update) {

            // Set service motor behavior
            if(HSM_Hyperloop.service_flag) {
                if(HSM_Hyperloop.direction) {
                    DEBUGOUT("Service motor engaged, reverse.\n");
                }
                else {
                    DEBUGOUT("Service motor engaged, forward.\n");
                }
            }
            else {
                DEBUGOUT("Service motor disengaged.\n");
            }

            // Set engine behavior
            if(HSM_Hyperloop.engine_flag) {
                DEBUGOUT("Engines engaged.\n");
                //update and maintain engine throttle
            }
            else {
                DEBUGOUT("Engines disengaged\n.");
                // Set throttle to 0
                int i = 0;
                for(i = 0; i < NUM_MOTORS; i++) {
                    set_motor_target_throttle(i, 0);
                    set_motor_throttle(i, 0);
                }
            }

            // Set brake behavior
            if(HSM_Hyperloop.brake_flag) {
                DEBUGOUT("Brakes engaged\n");
                // TODO: Engage brakes here
            }
            else {
                DEBUGOUT("Brakes disengaged\n");
                // TODO: Disengage brakes here
            }
            HSM_Hyperloop.update = 0;
            DEBUGOUT("\n\n");
        }

        // Update engines, even if a transition did not occur
        if(HSM_Hyperloop.engine_flag) {
            // Update and maintain engine throttle
            int i;
            for(i = 0; i < NUM_MOTORS; i++) {
                set_motor_target_throttle(i, 4);
                set_motor_throttle(i, 4);
            }
        }
        else {
            // Set throttle to 0
            int i;
            for(i = 0; i<NUM_MOTORS; i++) {
                set_motor_target_throttle(i, 0);
                set_motor_throttle(i, 0);
            }
        }
        // TODO: Update HEMS here.
    }

#else
        // Prototype test run control routine
        if(PROTOTYPE_TEST) {
            uint8_t prototypeRunTempAlert = 0;

#if 0
            // Check if any temperature is over 80C.
            int i, j;
            for(i=0; i<NUM_MOTORS; i++) {
                for(j=0; j<NUM_THERMISTORS; j++) {
                    if(motors[i]->temperatures[j] > 80) {
                        prototypeRunFlag = 0;   // The (pre)run has ended due to over-temperature.
                        prototypeRunTempAlert = 1;
                        DEBUGOUT("OVER-TEMPERATURE ALERT\n");
                        break;
                    }
                }
                if(prototypeRunTempAlert == 1) break;
            }
#endif  // 0

            // Check if prototype test is running AND temperature is below 80C.
            if(prototypeRunTempAlert == 0 && prototypeRunFlag == 1) {
                int time_sec = getRuntime()/1000;

                if(PROTOTYPE_PRERUN) {  // PRERUN
                    if (time_sec < prototypeRunStartTime + 10) {    // Spin up to tenth power.
                        motors[0]->target_throttle_voltage = 0.8;
                        motors[1]->target_throttle_voltage = 0;
                        motors[2]->target_throttle_voltage = 0;
                        motors[3]->target_throttle_voltage = 0;
                    }
                    else if (time_sec < prototypeRunStartTime + 20) {   // Spin up to tenth power.
                        motors[0]->target_throttle_voltage = 0;
                        motors[1]->target_throttle_voltage = 0.8;
                        motors[2]->target_throttle_voltage = 0;
                        motors[3]->target_throttle_voltage = 0;
                    }
                    else if (time_sec < prototypeRunStartTime + 30) {   // Spin up to tenth power.
                        motors[0]->target_throttle_voltage = 0;
                        motors[1]->target_throttle_voltage = 0;
                        motors[2]->target_throttle_voltage = 0.8;
                        motors[3]->target_throttle_voltage = 0;
                    }
                    else if (time_sec < prototypeRunStartTime + 40) {   // Spin up to tenth power.
                        motors[0]->target_throttle_voltage = 0;
                        motors[1]->target_throttle_voltage = 0;
                        motors[2]->target_throttle_voltage = 0;
                        motors[3]->target_throttle_voltage = 0.8;
                    } else {    // Spin down.
                        motors[0]->target_throttle_voltage = 0;
                        motors[1]->target_throttle_voltage = 0;
                        motors[2]->target_throttle_voltage = 0;
                        motors[3]->target_throttle_voltage = 0;
                        motors[0]->throttle_voltage = 0;
                        motors[1]->throttle_voltage = 0;
                        motors[2]->throttle_voltage = 0;
                        motors[3]->throttle_voltage = 0;

                        prototypeRunFlag = 0;   // The prerun has ended.
                        DEBUGOUT("PRERUN HAS ENDED\n");
                    }
                }
                else {  // RUN
                    if (time_sec < prototypeRunStartTime + 60) {    // Spin up to half power.
                        motors[0]->target_throttle_voltage = 4;
                        motors[1]->target_throttle_voltage = 4;
                        motors[2]->target_throttle_voltage = 4;
                        motors[3]->target_throttle_voltage = 4;
#if 1
                        motors[0]->throttle_voltage = 4;
                        motors[1]->throttle_voltage = 4;
                        motors[2]->throttle_voltage = 4;
                        motors[3]->throttle_voltage = 4;
#endif  // 0

                    } else {    // Spin down.
                        motors[0]->target_throttle_voltage = 0;
                        motors[1]->target_throttle_voltage = 0;
                        motors[2]->target_throttle_voltage = 0;
                        motors[3]->target_throttle_voltage = 0;
                        motors[0]->throttle_voltage = 0;
                        motors[1]->throttle_voltage = 0;
                        motors[2]->throttle_voltage = 0;
                        motors[3]->throttle_voltage = 0;

                        prototypeRunFlag = 0;   // The run has ended.
                        DEBUGOUT("RUN HAS ENDED\n");
                    }
                }

            }
            else {  // The motors should not be spinning right now.
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
#endif // NO_OLD_PROTOTYPE_RUN


}
