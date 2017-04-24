#ifndef maglev_sm_h
#define maglev_sm_h
#include <stdint.h>
#include "qepn.h"

// Structure of data members to be shared between main and the state machine
typedef struct Maglev_HSM_data {
    QHsm super;
    uint8_t enable_motors;
    uint8_t update;
    uint8_t send_spunup;
    uint8_t send_spundown;
    uint8_t fault;
} Maglev_HSM_t;

// The global instance of the state machine object (with added data members defined above)
extern Maglev_HSM_t Maglev_HSM;

// Initialize function
void initializeMaglevStateMachine(void);

// Signals that can be sent to the state machine
enum Maglev_Signals {
    MAGLEV_STOP_SIG = Q_USER_SIG,
	MAGLEV_ENGAGE,
	MAGLEV_DISENGAGE,
	MAGLEV_SPUNUP,
    MAGLEV_SPUNDOWN,
    MAGLEV_FAULT_REC,
    MAGLEV_FAULT_UNREC,
    MAGLEV_FAULT_REC_CLEAR,
    MAGLEV_MAX_SIG
};

#endif
