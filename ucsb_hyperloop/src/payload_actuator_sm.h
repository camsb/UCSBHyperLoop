#ifndef payload_actuator_sm_h
#define payload_actuator_sm_h
#include <stdint.h>
#include "qepn.h"
#define NUM_PAYLOAD_ACTUATORS 2

// Structure of data members to be passed between main and the state machine
typedef struct Payload_Actuator_HSM_data {
    QHsm super;
    uint8_t actuator_direction[NUM_PAYLOAD_ACTUATORS];
    uint8_t actuator_enable[NUM_PAYLOAD_ACTUATORS];
    uint8_t actuator_support[NUM_PAYLOAD_ACTUATORS];
} Payload_Actuator_HSM_t;

// The global instance of the state machine object (with added data members defined above)
extern Payload_Actuator_HSM_t Payload_Actuator_HSM;

// Initialize function
void initializePayloadActuatorStateMachine(void);

// Signals that can be sent to the state machine
enum Payload_Actuator_HSM_t_Signals {
    PA_ADVANCE = Q_USER_SIG,
    PA_RETRACT,
    PA_TERMINATE_SIG,
    PA_IGNORE_SIG,
    PA_MAX_SIG
};

// Assertion function
//void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line);

#endif
