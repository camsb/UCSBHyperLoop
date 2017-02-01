#ifndef hyperloop_sm_h
#define hyperloop_sm_h
#include "HEMS.h"

// Signals that can be sent to the state machine
enum Hyperloop_Signals {
    STOP_SIG = Q_USER_SIG,
    FORWARD_SIG,
    REVERSE_SIG,
    ENGAGE_ENGINES_SIG,
    DISENGAGE_ENGINES_SIG,
    ENGAGE_BRAKES_SIG,
    DISENGAGE_BRAKES_SIG,
    ENGINES_REVED_SIG,
    ENGINES_STOPPED_SIG,
    TERMINATE_SIG,
    IGNORE_SIG,
    MAX_SIG
};

// Structure of data members to be passed between main and the state machine
typedef struct HyperloopTag {
    QHsm super;
    uint8_t engine_throttle[NUM_MOTORS];
    uint8_t engine_flag;
    uint8_t brake_flag;
    uint8_t service_flag;
    uint8_t direction;
    uint8_t update;
} Hyperloop;

// The global instance of the state machine object (with added data members defined above)
extern struct HyperloopTag HSM_Hyperloop;

// Initialize function
void Hyperloop_ctor(void);

#endif
