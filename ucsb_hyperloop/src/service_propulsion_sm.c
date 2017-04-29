#include "service_propulsion_sm.h"
#include "qpn_port.h"
#include "logging.h"
#include <stdio.h>

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
    BSP_display("Initial-INIT;\n");
    return Q_TRAN(&Nominal);
}
/*..........................................................................*/
QState Nominal(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY;\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal-INIT;\n");
            return Q_TRAN(&Nominal_raised);
        }
        case SP_FAULT_REC: {
        	BSP_display("Nominal - SP_FAULT_REC\n");
        	return Q_TRAN(&Fault_recoverable);
        }
        case SP_FAULT_UNREC: {
        	BSP_display("Nominal - SP_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Nominal_lowered(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_lowered-ENTRY;\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_lowered-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_lowered-INIT;\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_lowered_idle(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_lowered_idle-ENTRY;\n");
	    Service_Propulsion_HSM.actuator_enable = 0;
	    Service_Propulsion_HSM.motor_enable = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_lowered_idle-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_lowered_idle-INIT;\n");
            return Q_HANDLED();
        }
        case SP_FORWARD_SIG: {
            BSP_display("Nominal_lowered_idle - SP_FORWARD_SIG\nMoving forward;\n");
            return Q_TRAN(&Nominal_lowered_forward);
        }
        case SP_REVERSE_SIG: {
            BSP_display("Nominal_lowered_idle - SP_REVERSE_SIG\nMoving in reverse;\n");
            return Q_TRAN(&Nominal_lowered_reverse);
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Nominal_lowered_idle - SP_ADVANCE_SIG\nCannot advance further;\n");
            return Q_HANDLED();
        }
        case SP_RETRACT_SIG: {
            BSP_display("Nominal_lowered_idle - SP_RETRACT_SIG\nRetracting;\n");
            return Q_TRAN(&Nominal_raised_retracting);
        }
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_forward(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_lowered_forward-ENTRY;\n");
	    Service_Propulsion_HSM.motor_direction = 1;
	    Service_Propulsion_HSM.motor_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_lowered_forward-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_lowered_forward-INIT;\n");
            return Q_HANDLED();
        }
        case SP_STOP_SIG: {
            BSP_display("Nominal_lowered_forward - SP_STOP_SIG\nStopping;\n");
            return Q_TRAN(&Nominal_lowered_idle);
        }
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_reverse(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_lowered_reverse-ENTRY;\n");
	    Service_Propulsion_HSM.motor_direction = 0;
	    Service_Propulsion_HSM.motor_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_lowered_reverse-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_lowered_reverse-INIT;\n");
            return Q_HANDLED();
        }
        case SP_STOP_SIG: {
            BSP_display("Nominal_lowered_reverse - SP_STOP_SIG Stopping;\n");
            return Q_TRAN(&Nominal_lowered_idle);
        }
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_raised(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised-ENTRY;\n");
            Service_Propulsion_HSM.motor_enable = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised-INIT;\n");
            return Q_TRAN(&Nominal_raised_idle);
        }
    }
    return Q_SUPER(&Nominal);
}
/*.........................................................................*/
QState Nominal_raised_idle(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_idle-ENTRY;\n");
            Service_Propulsion_HSM.actuator_enable = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_idle-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_idle-INIT;\n");
            return Q_HANDLED();
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Nominal_raised_idle - SP_ADVANCE_SIG\nAdvancing;\n");
            return Q_TRAN(&Nominal_raised_advancing);
        }
		case SP_RETRACT_SIG: {
            BSP_display("Nominal_raised_idle - SP_RETRACT_SIG\nRetracting;\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*.........................................................................*/
QState Nominal_raised_advancing(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_advancing-ENTRY;\n");
            Service_Propulsion_HSM.actuator_direction = 1;
            Service_Propulsion_HSM.actuator_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_advancing-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_advancing-INIT;\n");
            return Q_HANDLED();
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Nominal_raised_advancing - SP_ADVANCE_SIG\nContinuing to advance;\n");
            return Q_HANDLED();
        }
        case SP_RETRACT_SIG: {
            BSP_display("Nominal_raised_advancing - SP_RETRACT_SIG\nHalting advance, retracting;\n");
            return Q_TRAN(&Nominal_raised_retracting);
        }
        case SP_ADVANCE_DONE: {
        	BSP_display("Nominal_raised_advancing - SP_ADVANCE_DONE\n");
        	return Q_TRAN(&Nominal_lowered_idle);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*.........................................................................*/
QState Nominal_raised_retracting(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_retracting-ENTRY;\n");
            Service_Propulsion_HSM.actuator_direction = 0;
            Service_Propulsion_HSM.actuator_enable = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_retracting-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_retracting-INIT;\n");
            return Q_HANDLED();
        }
        case SP_ADVANCE_SIG: {
            BSP_display("Nominal_raised_retracting - SP_ADVANCE_SIG\nHalting retraction, advancing;\n");
            return Q_TRAN(&Nominal_raised_advancing);
        }
        case SP_RETRACT_SIG: {
            BSP_display("Nominal_raised_retracting - SP_RETRACT_SIG\nContinuing to reverse;\n");
            return Q_HANDLED();
        }
        case SP_RETRACT_DONE: {
        	BSP_display("Nominal_raised_retracting - SP_RETRACT_DONE\n");
        	return Q_TRAN(&Nominal_raised_idle);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*.........................................................................*/
QState Fault(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault-ENTRY;\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Fault-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Fault-INIT;\n");
            return Q_HANDLED();
        }
        case SP_FAULT_UNREC: {
        	BSP_display("Fault - SP_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Fault_recoverable(Service_Propulsion_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault_recoverable-ENTRY;\n");
            Service_Propulsion_HSM.fault = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Fault_recoverable-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Fault_recoverable-INIT;\n");
            return Q_HANDLED();
        }
        case SP_FAULT_REC_CLEAR: {
        	BSP_display("Fault_recoverable - SP_FAULT_REC_CLEAR\n");
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
            BSP_display("Fault_unrecoverable-ENTRY;\n");
            Service_Propulsion_HSM.fault = 2;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Fault_unrecoverable-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Fault_unrecoverable-INIT;\n");
            return Q_HANDLED();
        }
        case SP_FAULT_UNREC: {
        	BSP_display("Fault_unrecoverable - SP_FAULT_UNREC\n");
        	return Q_HANDLED();
        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/
