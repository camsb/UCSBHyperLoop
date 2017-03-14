#include "qpn_port.h"
#include "maglev_state_machine.h"
#include "logging.h"
#include "HEMS.h"

// Macro to re-direct the state machine debug test to DEBUGOUT only if SM_DEBUG is 1
#define SM_DEBUG 0
#define BSP_display(msg) if(SM_DEBUG){DEBUGOUT(msg);}

// The global instance of the state machine object
Hyperloop Maglev_HSM;

// Forward declaration of all the possible states
static QState Initial(Hyperloop *me);
static QState Nominal(Hyperloop *me);
static QState Nominal_motorsOff(Hyperloop *me);
static QState Nominal_motorsOff_idle(Hyperloop *me);
static QState Nominal_motorsOff_spinDown(Hyperloop *me);
static QState Nominal_motorsOn(Hyperloop *me);
static QState Nominal_motorsOn_spinUp(Hyperloop *me);
static QState Nominal_motorsOn_hover(Hyperloop *me);
static QState Fault(Hyperloop *me);
static QState Fault_recoverable(Hyperloop *me);
static QState Fault_unrecoverable(Hyperloop *me);

/*..........................................................................*/
void initializeMaglevStateMachine(void) {
    QHsm_ctor(&Maglev_HSM.super, (QStateHandler)&Initial);
    // Whether to provide throttle signal to maglev motors
    Maglev_HSM.enable_motors = 0;
    Maglev_HSM.update = 0; // Whether there was a change
    QHsm_init((QHsm *)&Maglev_HSM);
}

/*..........................................................................*/
static QState Initial(Hyperloop *me) {
    BSP_display("Initial-INIT;");
    return Q_TRAN(&Nominal);
}
/*..........................................................................*/
static QState Nominal(Hyperloop *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT;");
    		return Q_TRAN(&Nominal_motorsOff);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT;");
            return Q_HANDLED();
        }
        case MAGLEV_FAULT_REC: {
        	BSP_display("MAGLEV_FAULT_REC");
        	return Q_TRAN(&Fault_recoverable);
        }
        case MAGLEV_FAULT_UNREC: {
        	BSP_display("MAGLEV_FAULT_UNREC");
        	return Q_TRAN(&Fault_unrecoverable);
        }
//        default: {
//        	BSP_display("A signal got where it shouldn't.");
//        	return Q_HANDLED();
//        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Nominal_motorsOff(Hyperloop *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal_motorsOff-INIT;");
    		return Q_TRAN(&Nominal_motorsOff_idle);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOff-ENTRY;");
            logEventString("Maglev state machine state: Nominal_motorsOff");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOff-EXIT;");
            return Q_HANDLED();
        }
        case MAGLEV_ENGAGE: {
        	BSP_display("ENGAGE MOTORS!");
        	return Q_TRAN(&Nominal_motorsOn);
        }

    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_motorsOff_idle(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOff_idle-ENTRY;");
            logEventString("Maglev state machine state: Nominal_motorsOff_idle");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOff_idle-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOff_idle-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal_motorsOff);
}
/*..........................................................................*/
QState Nominal_motorsOff_spinDown(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOff_spinDown-ENTRY;");
            logEventString("Maglev state machine state: Nominal_motorsOff_spinDown");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
	        return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOff_spinDown-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOff_spinDown-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal_motorsOff);
}
/*..........................................................................*/
QState Nominal_motorsOn(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOn-ENTRY;");
            logEventString("Maglev state machine state: Nominal_motorsOn");
            Maglev_HSM.enable_motors = 1;
            Maglev_HSM.update = 1;
            return Q_TRAN(&Nominal_motorsOn_spinUp);
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOn-EXIT;");
            Maglev_HSM.enable_motors = 0;
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOn-INIT;");
            return Q_TRAN(&Nominal_motorsOn_spinUp);
        }
        case MAGLEV_DISENGAGE: {
        	BSP_display("MAGLEV_DISENGAGE");
        	return Q_TRAN(&Nominal_motorsOff_spinDown);
        }
    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_motorsOn_spinUp(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOn_spinUp-ENTRY;");
            logEventString("Maglev state machine state: Nominal_motorsOn_spinUp");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOn_spinUp-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOn_spinUp-INIT;");
            return Q_HANDLED();
        }
        case MAGLEV_SPUNUP: {
            BSP_display("MAGLEV_SPUNUP!;");
            return Q_TRAN(&Nominal_motorsOn_hover);
        }
    }
    return Q_SUPER(&Nominal_motorsOn);
}
/*..........................................................................*/
QState Nominal_motorsOn_hover(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
        	BSP_display("hover-ENTRY;");
            BSP_display("maintaining hovering conditions;");
            logEventString("Maglev state machine state: Nominal_motorsOn_hover");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("hover-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("hover-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal_motorsOn);
}
/*..........................................................................*/
QState Fault(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("braking-ENTRY;");
            BSP_display("engaging brakes;");
            logEventString("Maglev state machine state: Fault");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("braking-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("braking-INIT;");
            return Q_HANDLED();
        }
        case MAGLEV_FAULT_UNREC: {
        	BSP_display("MAGLEV_FAULT_UNREC");
        	return Q_TRAN(&Fault_unrecoverable);
        }
//        default: {
//        	BSP_display("A signal got where it shouldn't.");
//        	return Q_HANDLED();
//        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Fault_recoverable(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("forward-ENTRY;");
            logEventString("Maglev state machine state: Fault_recoverable");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("forward-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("forward-INIT;");
            return Q_HANDLED();
        }
        case MAGLEV_FAULT_REC_CLEAR: {
        	BSP_display("MAGLEV_FAULT_REC_CLEAR");
        	return Q_TRAN(&Nominal_motorsOff_spinDown);
        }
//        case STOP_SIG: {
//            return Q_TRAN(&Nominal_motorsOff_idle);
//        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/
QState Fault_unrecoverable(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("reverse-ENTRY;");
            BSP_display("moving in reverse;");
            logEventString("Maglev state machine state: Fault_unrecoverable");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("reverse-EXIT;");
            BSP_display("stopping;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("reverse-INIT;");
            return Q_HANDLED();
        }
//        case STOP_SIG: {
//            return Q_TRAN(&Nominal_motorsOff_idle);
//        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/

// This assertion function is required for the state machine. It's called if things go haywire.
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    DEBUGOUT("Assertion failed in %s, line %d", file, line);
    //exit(-1);
}
