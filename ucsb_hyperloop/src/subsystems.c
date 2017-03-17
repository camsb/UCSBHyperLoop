#include "subsystems.h"
#include "maglev_state_machine.h"
#include "HEMS.h"
#include "initialization.h"

#define IGNORE_FAULTS 1 	// For testing

void initializeSubsystemStateMachines(){
	initializeMaglevStateMachine();
}

void dispatch_signal_from_webapp(int signal){
	switch(signal){
		case CS_MAGLEV_ENGAGE:{
			// TODO: Add check that payload actuators are supporting payload in upright position
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_ENGAGE);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
		}
		case CS_MAGLEV_DISENGAGE:{
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_DISENGAGE);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
		}
	}
}

void generate_signals_from_sensor_data(){
#if MOTOR_BOARD_I2C_ACTIVE
	maglev_service_state_machine();
#endif

}

void maglev_service_state_machine(){
	if (Maglev_HSM.faulted == 0){
		// Nominal transitions - only evaluated/issued when the system is not faulted
		if (Maglev_HSM.send_spunup && (motors[0]->rpm > 500)){
			// Motors are spun up
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_SPUNUP);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			Maglev_HSM.send_spunup = 0;
		}
		else if (Maglev_HSM.send_spundown && (motors[0]->rpm < 500)){
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
