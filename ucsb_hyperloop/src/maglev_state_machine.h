#ifndef hyperloop_sm_h
#define hyperloop_sm_h
#include "HEMS.h"
#include "qepn.h"

// Structure of data members to be shared between main and the state machine
typedef struct Maglev_HSM_data {
    QHsm super;
    uint8_t enable_motors;
    uint8_t update;
} Hyperloop;

// The global instance of the state machine object (with added data members defined above)
extern struct Maglev_HSM_data Maglev_HSM;

// Initialize function
void initializeMaglevStateMachine(void);

// Signals that can be sent to the state machine
enum Maglev_Signals {
    STOP_SIG = Q_USER_SIG,
	MAGLEV_ENGAGE,
	MAGLEV_DISENGAGE,
	MAGLEV_SPUNUP,
    MAGLEV_SPUNDOWN,
    MAGLEV_FAULT_REC,
    MAGLEV_FAULT_UNREC,
    MAGLEV_FAULT_REC_CLEAR,
    MAX_SIG
};

// Assertion function
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line);

#endif
