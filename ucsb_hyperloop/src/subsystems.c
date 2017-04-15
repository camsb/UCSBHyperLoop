#include "subsystems.h"
#include "maglev_state_machine.h"
#include "braking_state_machine.h"
#include "payload_actuator_sm.h"
#include "service_propulsion_sm.h"
#include "HEMS.h"
#include "initialization.h"

#define IGNORE_FAULTS 1 	// For testing

void initializeSubsystemStateMachines(){
	initializeBrakingStateMachine();
	initializeMaglevStateMachine();
	initializePayloadActuatorStateMachine();
	initializeServicePropulsionStateMachine();
}

void dispatch_signal_from_webapp(int signal){
	switch(signal){

		// BRAKES
		case CS_BRAKES_ENGAGE:{
			Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_ENGAGE);
			QHsm_dispatch((QHsm *)&Braking_HSM);
			break;
		}
		case CS_BRAKES_DISENGAGE:{
			Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_DISENGAGE);
			QHsm_dispatch((QHsm *)&Braking_HSM);
			break;
		}
		case CS_BRAKES_EMERGENCY:{
			Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_EMERGENCY);
			QHsm_dispatch((QHsm *)&Braking_HSM);
			break;
		}
		case CS_BRAKES_EMERGENCY_RELEASE:{
			Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_EMERGENCY_RELEASE);
			QHsm_dispatch((QHsm *)&Braking_HSM);
			break;
		}
		case CS_BRAKES_TEST_ENTER:{
			Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_TEST_ENTER);
			QHsm_dispatch((QHsm *)&Braking_HSM);
			break;
		}
		case CS_BRAKES_TEST_EXIT:{
			Q_SIG((QHsm *)&Braking_HSM) = (QSignal)(BRAKES_TEST_EXIT);
			QHsm_dispatch((QHsm *)&Braking_HSM);
			break;
		}

		// MAGLEV
		case CS_MAGLEV_ENGAGE:{
			// TODO: Add check that payload actuators are supporting payload in upright position
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_ENGAGE);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			break;
		}
		case CS_MAGLEV_DISENGAGE:{
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_DISENGAGE);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			break;
		}

		// PAYLOAD ACUTATOR
		case CS_ACTUATORS_RAISE:{
			Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_ADVANCE);
			QHsm_dispatch((QHsm *)&Payload_Actuator_HSM);
			break;
		}
		case CS_ACTUATORS_LOWER:{
			Q_SIG((QHsm *)&Payload_Actuator_HSM) = (QSignal)(PA_RETRACT);
			QHsm_dispatch((QHsm *)&Payload_Actuator_HSM);
			break;
		}

		// SURFACE PROPULSION
	}
}

void generate_signals_from_sensor_data(){
    // Look at sensor data to determine if a state machine transition signal should be sent.

#if MOTOR_BOARD_I2C_ACTIVE
	maglev_service_state_machine();
#endif

}

void maglev_service_state_machine(){
	if (Maglev_HSM.faulted == 0){
		// Nominal transitions - only evaluated/issued when the system is not faulted
		if (Maglev_HSM.send_spunup && (motors[0]->rpm[1] > 500)){
			// Motors are spun up
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_SPUNUP);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			Maglev_HSM.send_spunup = 0;
		}
		else if (Maglev_HSM.send_spundown && (motors[0]->rpm[1] < 500)){
			// Motors are spun down
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_SPUNDOWN);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			Maglev_HSM.send_spundown = 0;
		}

		// Fault transitions
		int fault = maglev_fault_from_sensors();
		if (fault == 1){
			// Recoverable fault
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_FAULT_REC);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
		}
		else if (fault == 2){
			// Unrecoverable fault
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_FAULT_UNREC);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
		}

	}
	else if (Maglev_HSM.faulted == 1){
		// Check if the recoverable fault conditions are all cleared
		int fault = maglev_fault_from_sensors();
		if (fault == 0){
			// Recoverable fault is CLEARED
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_FAULT_REC_CLEAR);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
		}
		else if (fault == 2){
			// Unrecoverable fault
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_FAULT_UNREC);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
		}
	}
}

int maglev_fault_from_sensors(){
#if !IGNORE_FAULTS
	// Calculates and returns whether the motors have a fault condition
	int i, j;
	// Unrecoverable faults
	/*
	• Motor batteries have too low voltage / state of charge
	• Motor batteries have a significant cell voltage imbalance
	• Feedback or control signals are inoperable on one or more motors
	*/

	// Recoverable faults
	// Find the average RPM of all the motors
	int avg_rpm;
	for (i = 0; i < NUM_MOTORS; i++){
		avg_rpm += motors[i].rpm;
	}
	avg_rpm /= NUM_MOTORS;
	for (i = 0; i < NUM_MOTORS; i++){
		// Check current readings
		if (motors[i].amps > SAFE_CURRENT){
			return 1;
		}

		// Check that no motors have an RPM > 25% different from the others
		// TODO: Check that tolerance
		if (motors[i].rpm < (0.75 * avg_rpm) || (motors[i].rpm) > (1.25 * avg_rpm)){
			return 1;
		}

		// Check that no thermistors are above the safe temperature
		for (j = 0; j < NUM_THERMISTORS; j++){
			if (motors[i].temperatures[j] >= SAFE_TEMPERATURE){
				return 1;
			}
		}
	}
	return 0;
#else
	return 0;
#endif
}

void go_routine(){
	// Do some rad stuff.
	/*
	• Raise payload actuators
	• Ensure flag indicating payload is support by actuators has been set
	• Engage magnetic levitation motors
	• Ensure magnetic levitation motors are operating correctly
	• Lower payload actuators
	• Ensure pod is hovering (magnetic levitation motors are operating and payload linear actuators are lowered)
	• Ensure pod location or test run time is past predetermined braking threshold
	• Engage brakes
	• Ensure pod has come to a stop
	• Disengage brakes
	• Disengage magnetic levitation motors
	*/
}


// This assertion function is required for the state machine. It's called if things go haywire.
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    DEBUGOUT("Assertion failed in %s, line %d", file, line);
	//exit(-1);
}
