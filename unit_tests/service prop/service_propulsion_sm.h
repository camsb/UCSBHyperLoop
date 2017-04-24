#ifndef service_propulsion_sm_h
#define service_propulsion_sm_h
#include <stdint.h>
#include "qepn.h"

// Structure of data members to be passed between main and the state machine
typedef struct Service_Propulsion_HSM_data {
    QHsm super;
    uint8_t actuator_direction;
    uint8_t actuator_enable;
    uint8_t motor_direction;
    uint8_t motor_enable;
    uint8_t fault;
} Service_Propulsion_HSM_t;

// The global instance of the state machine object (with added data members defined above)
extern Service_Propulsion_HSM_t Service_Propulsion_HSM;

// Initialize function
void initializeServicePropulsionStateMachine(void);

// Signals that can be sent to the state machine
enum Service_Propulsion_HSM_t_Signals {
    SP_ADVANCE_SIG = Q_USER_SIG,
    SP_RETRACT_SIG,
    SP_ADVANCE_DONE,
    SP_RETRACT_DONE,
    SP_FORWARD_SIG,
    SP_REVERSE_SIG,
    SP_STOP_SIG,
    SP_FAULT_REC,
    SP_FAULT_UNREC,
    SP_FAULT_REC_CLEAR,
    SP_MAX_SIG
};

// Assertion function
//void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line);

#endif
