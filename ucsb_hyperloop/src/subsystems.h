#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H

// All control signals that can be issued from the web-app
// CS_ prefixes are to differentiate these values from the signals *internal* to the state machines
enum control_signals{
	// Special sequences
	CS_GO,
	CS_ALL_STOP,
	// Brakes
	CS_BRAKES_ENGAGE,
	CS_BRAKES_DISENGAGE,
	CS_BRAKES_EMERGENCY,
	CS_BRAKES_EMERGENCY_RELEASE,
	CS_BRAKES_TEST_ENTER,
	CS_BRAKES_TEST_EXIT,
	// Payload Actuators
	CS_ACTUATORS_RAISE,
	CS_ACTUATORS_LOWER,
	// Maglev motors
	CS_MAGLEV_ENGAGE,
	CS_MAGLEV_DISENGAGE,
	// Surface propulsion
	CS_SURFPROP_ACTUATOR_LOWER,
	CS_SURFPROP_ACTUATOR_RAISE,
	CS_SURFPROP_ENGAGE_FORWARD,
	CS_SURFPROP_ENGAGE_REVERSE,
	CS_SURFPROP_DISENGAGE
};

void initializeSubsystemStateMachines();
void dispatch_signal_from_webapp(int signal);
void maglev_service_state_machine();
int maglev_fault_from_sensors();

#endif
