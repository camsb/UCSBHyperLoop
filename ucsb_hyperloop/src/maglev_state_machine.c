#include "maglev_state_machine.h"
#include "qpn_port.h"
#include "logging.h"

// Macro to re-direct the state machine debug test to DEBUGOUT only if SM_DEBUG is 1
#define SM_DEBUG 0
#define BSP_display(msg) if(SM_DEBUG){DEBUGOUT(msg);}

// The global instance of the state machine object
Maglev_HSM_t Maglev_HSM;

// Forward declaration of all the possible states
static QState Initial(Maglev_HSM_t *me);
static QState Nominal(Maglev_HSM_t *me);
static QState Nominal_motorsOff(Maglev_HSM_t *me);
static QState Nominal_motorsOff_idle(Maglev_HSM_t *me);
static QState Nominal_motorsOff_spinDown(Maglev_HSM_t *me);
static QState Nominal_motorsOn(Maglev_HSM_t *me);
static QState Nominal_motorsOn_spinUp(Maglev_HSM_t *me);
static QState Nominal_motorsOn_hover(Maglev_HSM_t *me);
static QState Fault(Maglev_HSM_t *me);
static QState Fault_recoverable(Maglev_HSM_t *me);
static QState Fault_unrecoverable(Maglev_HSM_t *me);

/*..........................................................................*/
void initializeMaglevStateMachine(void) {
    QHsm_ctor(&Maglev_HSM.super, (QStateHandler)&Initial);
    Maglev_HSM.enable_motors = 0;		// Whether to provide throttle signal to maglev motors
    Maglev_HSM.update = 0; 				// Whether there was a change in enable flag
    Maglev_HSM.send_spunup = 0;
    Maglev_HSM.send_spundown = 0;
    Maglev_HSM.fault = 0;
    QHsm_init((QHsm *)&Maglev_HSM);
}

/*..........................................................................*/
static QState Initial(Maglev_HSM_t *me) {
    BSP_display("Initial-INIT\n");
    return Q_TRAN(&Nominal);
}
/*..........................................................................*/
static QState Nominal(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_TRAN(&Nominal_motorsOff);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
        case MAGLEV_FAULT_REC: {
        	BSP_display("MAGLEV_FAULT_REC\n");
        	return Q_TRAN(&Fault_recoverable);
        }
        case MAGLEV_FAULT_UNREC: {
        	BSP_display("MAGLEV_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Nominal_motorsOff(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal_motorsOff-INIT\n");
    		return Q_TRAN(&Nominal_motorsOff_idle);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOff-ENTRY\n");
            logEventString("Maglev state machine state: Nominal_motorsOff\n");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOff-EXIT\n");
            return Q_HANDLED();
        }
        case MAGLEV_ENGAGE: {
        	BSP_display("ENGAGE MOTORS!\n");
        	return Q_TRAN(&Nominal_motorsOn);
        }
    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_motorsOff_idle(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOff_idle-ENTRY\n");
            logEventString("Maglev state machine state: Nominal_motorsOff_idle\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOff_idle-EXIT\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOff_idle-INIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal_motorsOff);
}
/*..........................................................................*/
QState Nominal_motorsOff_spinDown(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOff_spinDown-ENTRY\n");
            logEventString("Maglev state machine state: Nominal_motorsOff_spinDown\n");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
            Maglev_HSM.send_spundown = 1;
	        return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOff_spinDown-EXIT\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOff_spinDown-INIT\n");
            return Q_HANDLED();
        }
        case MAGLEV_SPUNDOWN: {
        	BSP_display("MAGLEV_SPUNDOWN\n");
        	return Q_TRAN(&Nominal_motorsOff_idle);
        }
    }
    return Q_SUPER(&Nominal_motorsOff);
}
/*..........................................................................*/
QState Nominal_motorsOn(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOn-ENTRY\n");
            logEventString("Maglev state machine state: Nominal_motorsOn\n");
            Maglev_HSM.enable_motors = 1;
            Maglev_HSM.update = 1;
            return Q_TRAN(&Nominal_motorsOn_spinUp);
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOn-EXIT\n");
            Maglev_HSM.enable_motors = 0;
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOn-INIT\n");
            return Q_TRAN(&Nominal_motorsOn_spinUp);
        }
        case MAGLEV_DISENGAGE: {
        	BSP_display("MAGLEV_DISENGAGE\n");
        	return Q_TRAN(&Nominal_motorsOff_spinDown);
        }
    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_motorsOn_spinUp(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_motorsOn_spinUp-ENTRY\n");
            logEventString("Maglev state machine state: Nominal_motorsOn_spinUp\n");
            Maglev_HSM.send_spunup = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOn_spinUp-EXIT\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOn_spinUp-INIT\n");
            return Q_HANDLED();
        }
        case MAGLEV_SPUNUP: {
            BSP_display("MAGLEV_SPUNUP!\n");
            return Q_TRAN(&Nominal_motorsOn_hover);
        }
    }
    return Q_SUPER(&Nominal_motorsOn);
}
/*..........................................................................*/
QState Nominal_motorsOn_hover(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
        	BSP_display("Nominal_motorsOn_hover-ENTRY\n");
            logEventString("Maglev state machine state: Nominal_motorsOn_hover\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_motorsOn_hover-EXIT\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_motorsOn_hover-INIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal_motorsOn);
}
/*..........................................................................*/
QState Fault(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault-ENTRY\n");
            logEventString("Maglev state machine state: Fault\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Fault-EXIT\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Fault-INIT\n");
            return Q_HANDLED();
        }
        case MAGLEV_FAULT_UNREC: {
        	BSP_display("MAGLEV_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Fault_recoverable(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault_recoverable-ENTRY\n");
            logEventString("Maglev state machine state: Fault_recoverable\n");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
            Maglev_HSM.fault = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Fault_recoverable-EXIT\n");
            Maglev_HSM.fault = 1;
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Fault_recoverable-INIT\n");
            return Q_HANDLED();
        }
        case MAGLEV_FAULT_REC_CLEAR: {
        	BSP_display("MAGLEV_FAULT_REC_CLEAR\n");
        	return Q_TRAN(&Nominal_motorsOff_spinDown);
        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/
QState Fault_unrecoverable(Maglev_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault_unrecoverable-ENTRY\n");
            logEventString("Maglev state machine state: Fault_unrecoverable\n");
            Maglev_HSM.enable_motors = 0;
            Maglev_HSM.update = 1;
            Maglev_HSM.fault = 2;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Fault_unrecoverable-EXIT\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Fault_unrecoverable-INIT\n");
            return Q_HANDLED();
        }
        case MAGLEV_FAULT_UNREC: {
        	BSP_display("Fault_unrecoverable -MAGLEV_FAULT_UNREC\n");
        	return Q_HANDLED();
        }
    }
    return Q_SUPER(&Fault);
}
