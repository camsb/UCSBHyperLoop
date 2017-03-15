#include "subsystems.h"
#include "maglev_state_machine.h"
#include "HEMS.h"
#include "initialization.h"

void initializeSubsystemStateMachines(){
	initializeMaglevStateMachine();
}

// Some temp flags
int send_spunup = 0;
int send_spundown = 0;

void dispatch_signal_from_webapp(int signal){
	switch(signal){
		case CS_MAGLEV_ENGAGE:{
			// TODO: Add check that payload actuators are supporting payload in upright position
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_ENGAGE);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			send_spunup = 1;
		}
		case CS_MAGLEV_DISENGAGE:{
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_DISENGAGE);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			send_spundown = 1;
		}
	}
}

void generate_signals_from_sensor_data(){
	if (MOTOR_BOARD_I2C_ACTIVE){
		if (send_spunup && (motors[0]->rpm > 500)){
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_SPUNUP);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			send_spunup = 0;
		}
		else if (send_spundown && (motors[0]->rpm < 500)){
			Q_SIG((QHsm *)&Maglev_HSM) = (QSignal)(MAGLEV_SPUNDOWN);
			QHsm_dispatch((QHsm *)&Maglev_HSM);
			send_spundown = 0;
		}
	}
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
