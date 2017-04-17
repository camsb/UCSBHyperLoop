#include "payload_actuator_sm.h"
#include "qpn_port.h"
#include "logging.h"

// Macro to re-direct the state machine debug test to DEBUGOUT only if SM_DEBUG is 1
#define SM_DEBUG 0
#define BSP_display(msg) if(SM_DEBUG){DEBUGOUT(msg);}

/* The global instance of the state machine object -------------------------*/
Payload_Actuator_HSM_t Payload_Actuator_HSM;

// Forward declaration of all the possible states
static QState Initial(Payload_Actuator_HSM_t *me);
static QState Nominal(Payload_Actuator_HSM_t *me);
static QState Nominal_lowered(Payload_Actuator_HSM_t *me);
static QState Nominal_lowered_disengaged(Payload_Actuator_HSM_t *me);
static QState Nominal_lowered_advancing(Payload_Actuator_HSM_t *me);
static QState Nominal_lowered_retracting(Payload_Actuator_HSM_t *me);
static QState Nominal_raised(Payload_Actuator_HSM_t *me);
static QState Nominal_raised_supporting(Payload_Actuator_HSM_t *me);
static QState Nominal_raised_supporting_engaged(Payload_Actuator_HSM_t *me);
static QState Nominal_raised_not_supporting(Payload_Actuator_HSM_t *me);
static QState Nominal_raised_not_supporting_engaged(Payload_Actuator_HSM_t *me);
static QState Nominal_raised_not_supporting_advancing(Payload_Actuator_HSM_t *me);
static QState Nominal_raised_not_supporting_retracting(Payload_Actuator_HSM_t *me);
static QState Nominal_raised_not_supporting_disengaged(Payload_Actuator_HSM_t *me);
static QState Fault(Payload_Actuator_HSM_t *me);
static QState Fault_recoverable(Payload_Actuator_HSM_t *me);
static QState Fault_unrecoverable(Payload_Actuator_HSM_t *me);

int i;
/*..........................................................................*/
void initializePayloadActuatorStateMachine(void) {
    QHsm_ctor(&Payload_Actuator_HSM.super, (QStateHandler)&Initial);
    int i;
    for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++)
    {
        Payload_Actuator_HSM.actuator_direction[i] = 0;
        Payload_Actuator_HSM.actuator_enable[i] = 0;
    }
    Payload_Actuator_HSM.fault = 0;
    QHsm_init((QHsm *)&Payload_Actuator_HSM);
}

/*..........................................................................*/
static QState Initial(Payload_Actuator_HSM_t *me) {
    BSP_display("top-INIT;");
    return Q_TRAN(&Nominal_lowered_disengaged);
}
/*..........................................................................*/
QState Nominal(Payload_Actuator_HSM_t *me) {
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
        case PA_FAULT_REC: {
        	BSP_display("PA_FAULT_REC\n");
        	return Q_TRAN(&Fault_recoverable);
        }
        case PA_FAULT_UNREC: {
        	BSP_display("PA_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Nominal_lowered(Payload_Actuator_HSM_t *me) {
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
QState Nominal_lowered_disengaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Lowered and disengaged-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Lowered and disengaged-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Lowered and disengaged-INIT;");
            return Q_HANDLED();
        }
		case PA_ADVANCE: {
				BSP_display("PA_ADVANCE;");
				return Q_TRAN(&Nominal_lowered_advancing);
			}
		case PA_RETRACT: {
				BSP_display("Cannot retract;");
				return Q_HANDLED();
			}
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_advancing(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Lowered and advancing-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 1;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Lowered and advancing-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Lowered and advancing-INIT;");
            return Q_HANDLED();
        }
		case PA_ADVANCE: {
			BSP_display("Continuing to advance;");
			return Q_TRAN(&Nominal_raised_supporting_engaged);
		}
		case PA_RETRACT: {
			BSP_display("Halting advance and retracting;");
			return Q_TRAN(&Nominal_lowered_retracting);
		}
		case PA_ADVANCE_DONE: {
			BSP_display("lowered_advancing - ADVANCE_DONE");
			return Q_TRAN(&Nominal_raised_supporting);
		}
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_retracting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Lowered and retracting-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Lowered and retracting-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Lowered and retracting-INIT;");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Halting retraction and advancing;");
            return Q_TRAN(&Nominal_lowered_advancing);
        }
		case PA_RETRACT: {
            BSP_display("Continuing to retract;");
            return Q_TRAN(&Nominal_lowered_disengaged);
        }
		case PA_RETRACT_DONE: {
			BSP_display("lowered_retracting - RETRACT_DONE");
			return Q_TRAN(&Nominal_lowered_disengaged);
		}
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_raised(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Raised-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Raised-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Raised-INIT;");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_raised_supporting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Supporting-ENTRY;");
            return Q_TRAN(&Nominal_raised_supporting_engaged);
        }
        case Q_EXIT_SIG: {
            BSP_display("Supporting-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Supporting-INIT;");
            return Q_HANDLED();
        }
        case PA_ML_SUPPORT_GAINED: {
        	BSP_display("Supporting - ML_SUPPORT_GAINED\n");
        	return Q_TRAN(&Nominal_raised_not_supporting);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*..........................................................................*/
QState Nominal_raised_supporting_engaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Actuators engaged-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Actuators engaged-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Actuators engaged-INIT;");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Cannot advance further;");
            return Q_HANDLED();
        }
        case PA_RETRACT: {
            BSP_display("Retracting;");
            return Q_TRAN(&Nominal_lowered_retracting);
        }
    }
    return Q_SUPER(&Nominal_raised_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Not supporting-ENTRY;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Not supporting-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Not supporting-INIT;");
            return Q_HANDLED();
        }
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Not_supporting - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_raised_supporting);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_engaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("stationary-ENTRY;");
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
        case PA_ADVANCE: {
            BSP_display("PA_ADVANCE;");
            return Q_HANDLED();
        }
        case PA_RETRACT: {
            BSP_display("PA_RETRACT;");
            return Q_HANDLED();
        }
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Not_supporting_engaged - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_raised_supporting);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_advancing(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Advancing-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 1;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
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
        case PA_ADVANCE: {
            BSP_display("Continuing to advance;");
            return Q_TRAN(Nominal_raised_supporting_engaged);
        }
        case PA_RETRACT: {
            BSP_display("Halting advance, retracting;");
            return Q_TRAN(&Nominal_raised_not_supporting_retracting);
        }
		case PA_ADVANCE_DONE: {
			BSP_display("not_supporting_advancing - ADVANCE_DONE");
			return Q_TRAN(&Nominal_raised_not_supporting_engaged);
		}
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Not_supporting_advancing - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_lowered_advancing);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_retracting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Retracting-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
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
        case PA_ADVANCE: {
            BSP_display("Halting retraction, advancing;");
            return Q_TRAN(&Nominal_raised_not_supporting_advancing);
        }
        case PA_RETRACT: {
            BSP_display("Continuing to retract;");
            return Q_TRAN(&Nominal_raised_not_supporting_disengaged);
        }
		case PA_RETRACT_DONE: {
			BSP_display("not_supporting_retracting - RETRACT_DONE");
			return Q_TRAN(&Nominal_raised_not_supporting_disengaged);
		}
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Not_supporting_retracting - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_lowered_retracting);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_disengaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Disengaged-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Disengaged-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Disengaged-INIT;");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Advancing actuators;");
            return Q_TRAN(Nominal_raised_not_supporting_advancing);
        }
        case PA_RETRACT: {
            BSP_display("Cannot retract further;");
            return Q_HANDLED();
        }
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Not_supporting_disengaged - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_lowered_disengaged);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Fault(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault-ENTRY;");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
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
        case PA_FAULT_UNREC: {
        	BSP_display("PA_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Fault_recoverable(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Payload_Actuator_HSM.fault = 1;
            BSP_display("stationary-ENTRY;");
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
        case PA_FAULT_REC_CLEAR: {
        	BSP_display("PA_FAULT_REC_CLEAR\n");
            Payload_Actuator_HSM.fault = 0;
        	return Q_TRAN(&Nominal_lowered_retracting);
        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/
QState Fault_unrecoverable(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("stationary-ENTRY;");
            Payload_Actuator_HSM.fault = 2;
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
