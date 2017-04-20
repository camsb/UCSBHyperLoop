#include "service_propulsion_sm.h"
#include "qpn_port.h"
#include "logging.h"

// Macro to re-direct the state machine debug test to DEBUGOUT only if SM_DEBUG is 1
#define SM_DEBUG 0
#define BSP_display(msg) if(SM_DEBUG){DEBUGOUT(msg);}

/* The global instance of the state machine object -------------------------*/
Service_Propulsion_HSM_t Service_Propulsion_HSM;

// Forward declaration of all the possible states
static QState Initial(Service_Propulsion_HSM_t *me);
static QState Nominal(Service_Propulsion_HSM_t *me);
static QState Nominal_lowered(Service_Propulsion_HSM_t *me);
static QState Nominal_lowered_idle(Service_Propulsion_HSM_t *me);
static QState Nominal_lowered_forward(Service_Propulsion_HSM_t *me);
static QState Nominal_lowered_reverse(Service_Propulsion_HSM_t *me);
static QState Nominal_raised(Service_Propulsion_HSM_t *me);
static QState Nominal_raised_idle(Service_Propulsion_HSM_t *me);
static QState Nominal_raised_advancing(Service_Propulsion_HSM_t *me);
static QState Nominal_raised_retracting(Service_Propulsion_HSM_t *me);
static QState Fault(Service_Propulsion_HSM_t *me);
static QState Fault_recoverable(Service_Propulsion_HSM_t *me);
static QState Fault_unrecoverable(Service_Propulsion_HSM_t *me);
/*..........................................................................*/
void initializeServicePropulsionStateMachine(void) {
    QHsm_ctor(&Service_Propulsion_HSM.super, (QStateHandler)&Initial);
    Service_Propulsion_HSM.actuator_direction = 0;
    Service_Propulsion_HSM.actuator_enable = 0;
    Service_Propulsion_HSM.motor_direction = 0;
    Service_Propulsion_HSM.motor_enable = 0;
    Service_Propulsion_HSM.fault = 0;
    QHsm_init((QHsm *)&Service_Propulsion_HSM);
}

/*..........................................................................*/
static QState Initial(Service_Propulsion_HSM_t *me) {
    BSP_display("top-INIT;");
    return Q_TRAN(&Nominal);
}
/*..........................................................................*/
QState Nominal(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal-INIT;");
            return Q_HANDLED();
        }
        case SP_FAULT_REC: {
        	BSP_display("SP_FAULT_REC\n");
        	return Q_TRAN(&Fault_recoverable);
        }
        case SP_FAULT_UNREC: {
        	BSP_display("SP_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Nominal_lowered(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Lowered-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Lowered-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Lowered-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_lowered_idle(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Idle-ENTRY;");
	    Service_Propulsion_HSM.actuator_enable = 0;
	    Service_Propulsion_HSM.motor_enable = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Idle-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Idle-INIT;");
            return Q_HANDLED();
        }
        case SP_FORWARD_SIG: {
            BSP_display("Moving forward;");
            return Q_TRAN(&Nominal_lowered_forward);
        }
        case SP_REVERSE_SIG: {
            BSP_display("Moving in reverse;");
            return Q_TRAN(&Nominal_lowered_reverse);
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Cannot advance further;");
            return Q_HANDLED();
        }
        case SP_RETRACT_SIG: {
            BSP_display("Retracting;");
            return Q_TRAN(&Nominal_raised_retracting);
        }
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_forward(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Forward-ENTRY;");
	    Service_Propulsion_HSM.motor_direction = 1;
	    Service_Propulsion_HSM.motor_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Forward-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Forward-INIT;");
            return Q_HANDLED();
        }
        case SP_STOP_SIG: {
            BSP_display("Stopping;");
            return Q_TRAN(&Nominal_lowered_idle);
        }
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_reverse(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Reverse-ENTRY;");
	    Service_Propulsion_HSM.motor_direction = 0;
	    Service_Propulsion_HSM.motor_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Reverse-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Reverse-INIT;");
            return Q_HANDLED();
        }
        case SP_STOP_SIG: {
            BSP_display("Stopping;");
            return Q_TRAN(&Nominal_lowered_idle);
        }
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_raised(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("stationary-ENTRY;");
            Service_Propulsion_HSM.motor_enable = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("stationary-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("stationary-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal);
}
/*.........................................................................*/
QState Nominal_raised_idle(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Idle-ENTRY;");
            Service_Propulsion_HSM.actuator_enable = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Idle-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Idle-INIT;");
            return Q_HANDLED();
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Advancing;");
            return Q_TRAN(&Nominal_raised_advancing);
        }
		case SP_RETRACT_SIG: {
            BSP_display("Retracting;");
            return Q_TRAN(&Nominal_raised_retracting);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*.........................................................................*/
QState Nominal_raised_advancing(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Advancing-ENTRY;");
            Service_Propulsion_HSM.actuator_direction = 1;
            Service_Propulsion_HSM.actuator_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Advancing-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Advancing-INIT;");
            return Q_HANDLED();
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Continuing to advance;");
            return Q_HANDLED();
        }
        case SP_RETRACT_SIG: {
            BSP_display("Halting advance, retracting;");
            return Q_TRAN(&Nominal_raised_retracting);
        }
        case SP_ADVANCE_DONE: {
        	BSP_display("Nominal_raised_advancing - SP_ADVANCE_DONE");
        	return Q_TRAN(&Nominal_lowered_idle);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*.........................................................................*/
QState Nominal_raised_retracting(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Retracting-ENTRY;");
            Service_Propulsion_HSM.actuator_direction = 0;
            Service_Propulsion_HSM.actuator_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Retracting-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Retracting-INIT;");
            return Q_HANDLED();
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Halting retraction, advancing;");
            return Q_TRAN(&Nominal_raised_advancing);
        }
        case SP_RETRACT_SIG: {
            BSP_display("Continuing to reverse;");
            return Q_HANDLED();
        }
        case SP_RETRACT_DONE: {
        	BSP_display("Nominal_raised_retracting - SP_RETRACT_DONE");
        	return Q_TRAN(&Nominal_raised_idle);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*.........................................................................*/
QState Fault(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Fault-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Fault-INIT;");
            return Q_HANDLED();
        }
        case SP_FAULT_UNREC: {
        	BSP_display("SP_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Fault_recoverable(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("stationary-ENTRY;");
            Service_Propulsion_HSM.fault = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("stationary-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("stationary-INIT;");
            return Q_HANDLED();
        }
        case SP_FAULT_REC_CLEAR: {
        	BSP_display("SP_FAULT_REC_CLEAR\n");
        	Service_Propulsion_HSM.fault = 0;
        	return Q_TRAN(Nominal_raised_retracting);
        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/
QState Fault_unrecoverable(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("stationary-ENTRY;");
            Service_Propulsion_HSM.fault = 2;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("stationary-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("stationary-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/
// This assertion function is required for the state machine. It's called if things go haywire.
//void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
//    DEBUGOUT("Assertion failed in %s, line %d", file, line);
//    //exit(-1);
//}
