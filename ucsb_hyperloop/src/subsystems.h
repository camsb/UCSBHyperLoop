#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H
#include "qepn.h"

// All control signals that can be issued from the web-app
// CS_ prefixes are to differentiate these values from the signals *internal* to the state machines
// Changing these signals requires changes to subsystems.c to make sure that they are properly dispatched!!!
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

// Control signals
void dispatch_signal_from_webapp(int signal);

// Sensor signals
void generate_signals_from_sensor_data();
void braking_service_state_machine();
void maglev_service_state_machine();
void payload_service_state_machine();
void surface_propulsion_service_state_machine();

// Fault signals
void generate_faults_from_sensor_data();
void braking_fault_from_sensors();
int maglev_fault_from_sensors();
void payload_fault_from_sensors();
void surface_fault_from_sensors();

void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line);

#endif
