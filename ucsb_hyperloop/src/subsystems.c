#include "subsystems.h"
#include "maglev_state_machine.h"

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
