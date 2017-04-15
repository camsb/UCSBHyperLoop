#ifndef braking_sm_h
#define braking_sm_h
#include <stdint.h>
#include "qepn.h"

// Structure of data members to be shared between main and the state machine
typedef struct Braking_HSM_data {
    QHsm super;
    uint8_t enable_motors;
    uint8_t update;
    uint8_t send_spunup;
    uint8_t send_spundown;
    uint8_t faulted;
} Braking_HSM_t;

// The global instance of the state machine object (with added data members defined above)
extern Braking_HSM_t Braking_HSM;

// Initialize function
void initializeBrakingStateMachine(void);

// Signals that can be sent to the state machine
enum Braking_Signals {
    BRAKES_STOP_SIG = Q_USER_SIG,
	BRAKES_ENGAGE,
	BRAKES_DISENGAGE,
	BRAKES_EMERGENCY,
	BRAKES_EMERGENCY_RELEASE,
	BRAKES_TEST_ENTER,
	BRAKES_TEST_EXIT,
    BRAKES_FAULT_REC,
    BRAKES_FAULT_UNREC,
    BRAKES_FAULT_REC_CLEAR,
    BRAKES_MAX_SIG,
};

#endif
