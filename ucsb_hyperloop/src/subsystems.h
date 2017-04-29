#ifndef SUBSYSTEMS_H
#define SUBSYSTEMS_H
#include "qepn.h"

// All control signals that can be issued from the web-app
// CS_ prefixes are to differentiate these values from the signals *internal* to the state machines
enum control_signals{
	// Special sequences
	CS_GO,
	CS_ALL_STOP,
	CS_EMERGENCY_POD_STOP,
	// Brakes
	CS_BRAKES_ENGAGE,
	CS_BRAKES_DISENGAGE,
	CS_BRAKES_TEST_ENTER,
	CS_BRAKES_TEST_EXIT,
	// Payload Actuators
	CS_ACTUATORS_RAISE,
	CS_ACTUATORS_LOWER,
	// Maglev motors
	CS_MAGLEV_ENGAGE,
	CS_MAGLEV_DISENGAGE,
	// Service propulsion
	CS_SERVPROP_ACTUATOR_LOWER,
	CS_SERVPROP_ACTUATOR_RAISE,
	CS_SERVPROP_ENGAGE_FORWARD,
	CS_SERVPROP_ENGAGE_REVERSE,
	CS_SERVPROP_DISENGAGE,
	// Changing these signals requires changes to subsystems.c to make sure that they are properly dispatched!!!
	CS_SIZE // Used for sizing array of names below
};

// Names of control signals, as strings. Used for communications (web app and logging)
extern const char * control_signal_names[];

void initializeSubsystemStateMachines();

// Control signals
void dispatch_signal_from_webapp(int signal);

// Sensor signals
void generate_signals_from_sensor_data();
void braking_service_state_machine();
void maglev_service_state_machine();
void payload_service_state_machine();
void service_propulsion_service_state_machine();

// Fault signals
void generate_faults_from_sensor_data();
void braking_fault_from_sensors();
int maglev_fault_from_sensors();
int payload_fault_from_sensors();
int service_fault_from_sensors();

// Assertion function
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line);

#endif
