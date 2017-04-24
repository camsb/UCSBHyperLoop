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
    BSP_display("Initial-INIT;\n");
    return Q_TRAN(&Nominal_lowered_disengaged);
}
/*..........................................................................*/
QState Nominal(Payload_Actuator_HSM_t *me) {
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
            return Q_HANDLED();
        }
        case PA_FAULT_REC: {
        	BSP_display("Nominal - PA_FAULT_REC\n");
        	return Q_TRAN(&Fault_recoverable);
        }
        case PA_FAULT_UNREC: {
        	BSP_display("Nominal - PA_FAULT_UNREC\n");
        	return Q_TRAN(&Fault_unrecoverable);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Nominal_lowered(Payload_Actuator_HSM_t *me) {
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
QState Nominal_lowered_disengaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_lowered_disengaged-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_lowered_disengaged - EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_lowered_disengaged - INIT;\n");
            return Q_HANDLED();
        }
		case PA_ADVANCE: {
				BSP_display("Nominal_lowered_disengaged - PA_ADVANCE;\n");
				return Q_TRAN(&Nominal_lowered_advancing);
			}
		case PA_RETRACT: {
				BSP_display("Nominal_lowered_disengaged - PA_RETRACT\n");
				return Q_HANDLED();
			}
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_advancing(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_lowered_advancing-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 1;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_lowered_advancing-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_lowered_advancing-INIT;\n");
            return Q_HANDLED();
        }
		case PA_ADVANCE: {
			BSP_display("Nominal_lowered_advancing - PA_ADVANCE \nContinuing to advance;\n");
			return Q_TRAN(&Nominal_raised_supporting_engaged);
		}
		case PA_RETRACT: {
			BSP_display("Nominal_lowered_advancing - PA_RETRACT \nHalting advance and retracting;\n");
			return Q_TRAN(&Nominal_lowered_retracting);
		}
		case PA_ADVANCE_DONE: {
			BSP_display("Nominal_lowered_advancing - ADVANCE_DONE\n");
			return Q_TRAN(&Nominal_raised_supporting);
		}
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_lowered_retracting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Lowered and retracting-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Lowered and retracting-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Lowered and retracting-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Nominal_lowered_retracting - PA_ADVANCE \nHalting retraction and advancing;\n");
            return Q_TRAN(&Nominal_lowered_advancing);
        }
		case PA_RETRACT: {
            BSP_display("Nominal_lowered_retracting - PA_RETRACT \nContinuing to retract;\n");
            return Q_TRAN(&Nominal_lowered_disengaged);
        }
		case PA_RETRACT_DONE: {
			BSP_display("Nominal_lowered_retracting - RETRACT_DONE\n");
			return Q_TRAN(&Nominal_lowered_disengaged);
		}
    }
    return Q_SUPER(&Nominal_lowered);
}
/*..........................................................................*/
QState Nominal_raised(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised-ENTRY;\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised-INIT;\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Nominal);
}
/*..........................................................................*/
QState Nominal_raised_supporting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_supporting-ENTRY;\n");
            return Q_TRAN(&Nominal_raised_supporting_engaged);
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_supporting-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_supporting-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ML_SUPPORT_GAINED: {
        	BSP_display("Nominal_raised_supporting - ML_SUPPORT_GAINED\n");
        	return Q_TRAN(&Nominal_raised_not_supporting);
        }
        case PA_RETRACT: {
        	BSP_display("Nominal_raised_supporting - PA_RETRACT\n");
        	return Q_TRAN(&Nominal_lowered_retracting);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*..........................................................................*/
QState Nominal_raised_supporting_engaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_supporting_engaged-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_supporting_engaged-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_supporting_engaged-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Nominal_raised_supporting_engaged  - PA_ADVANCE\nCannot advance further;\n");
            return Q_HANDLED();
        }
        case PA_RETRACT: {
            BSP_display("Nominal_raised_supporting_engaged - PA_RETRACT\nRetracting;\n");
            return Q_TRAN(&Nominal_lowered_retracting);
        }
    }
    return Q_SUPER(&Nominal_raised_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_not_supporting-ENTRY;\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_not_supporting-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_not_supporting-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Nominal_raised_not_supporting - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_raised_supporting);
        }
        case PA_ADVANCE: {
            BSP_display("Nominal_raised_not_supporting_engaged - PA_ADVANCE;\n");
            return Q_HANDLED();
        }
        case PA_RETRACT: {
            BSP_display("Nominal_raised_not_supporting_engaged - PA_RETRACT;\n");
            return Q_TRAN(&Nominal_raised_not_supporting_retracting);
        }
    }
    return Q_SUPER(&Nominal_raised);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_engaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_not_supporting_engaged-ENTRY;\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_engaged-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_engaged-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Nominal_raised_not_supporting_engaged - PA_ADVANCE;\n");
            return Q_HANDLED();
        }
        case PA_RETRACT: {
            BSP_display("Nominal_raised_not_supporting_engaged - PA_RETRACT;\n");
            return Q_TRAN(&Nominal_raised_not_supporting_retracting);
        }
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Nominal_raised_not_supporting_engaged - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_raised_supporting);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_advancing(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_not_supporting_advancing-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 1;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_advancing-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_advancing-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Nominal_raised_not_supporting_advancing - PA_ADVANCE\nContinuing to advance;\n");
            return Q_HANDLED();
        }
        case PA_RETRACT: {
            BSP_display("Nominal_raised_not_supporting_advancing  - PA_RETRACT\nHalting advance, retracting;\n");
            return Q_TRAN(&Nominal_raised_not_supporting_retracting);
        }
		case PA_ADVANCE_DONE: {
			BSP_display("Nominal_raised_not_supporting_advancing - ADVANCE_DONE\n");
			return Q_TRAN(&Nominal_raised_not_supporting_engaged);
		}
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Nominal_raised_not_supporting_advancing - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_lowered_advancing);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_retracting(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_not_supporting_retracting-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 1;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_retracting-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_retracting-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Nominal_raised_not_supporting_retracting - PA_ADVANCE\nHalting retraction, advancing;\n");
            return Q_TRAN(&Nominal_raised_not_supporting_advancing);
        }
        case PA_RETRACT: {
            BSP_display("Nominal_raised_not_supporting_retracting - PA_RETRACT\nContinuing to retract;\n");
            return Q_TRAN(&Nominal_raised_not_supporting_disengaged);
        }
		case PA_RETRACT_DONE: {
			BSP_display("Nominal_raised_not_supporting_retracting - RETRACT_DONE\n");
			return Q_TRAN(&Nominal_raised_not_supporting_disengaged);
		}
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Nominal_raised_not_supporting_retracting - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_lowered_retracting);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Nominal_raised_not_supporting_disengaged(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Nominal_raised_not_supporting_disengaged-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_disengaged-EXIT;\n");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("Nominal_raised_not_supporting_disengaged-INIT;\n");
            return Q_HANDLED();
        }
        case PA_ADVANCE: {
            BSP_display("Nominal_raised_not_supporting_disengaged - PA_ADVANCE \nAdvancing actuators;\n");
            return Q_TRAN(Nominal_raised_not_supporting_advancing);
        }
        case PA_RETRACT: {
            BSP_display("Nominal_raised_not_supporting_disengaged - PA_RETRACT\nCannot retract further;\n");
            return Q_HANDLED();
        }
        case PA_ML_SUPPORT_LOST: {
        	BSP_display("Nominal_raised_not_supporting_disengaged - ML_SUPPORT_LOST\n");
        	return Q_TRAN(&Nominal_lowered_disengaged);
        }
    }
    return Q_SUPER(&Nominal_raised_not_supporting);
}
/*..........................................................................*/
QState Fault(Payload_Actuator_HSM_t *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("Fault-ENTRY;\n");
            for(i = 0; i < NUM_PAYLOAD_ACTUATORS; i++){
                Payload_Actuator_HSM.actuator_direction[i] = 0;
                Payload_Actuator_HSM.actuator_enable[i] = 0;
    	    }
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
        case PA_FAULT_UNREC: {
        	BSP_display("Fault - PA_FAULT_UNREC\n");
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
            BSP_display("Fault_recoverable-ENTRY;\n");
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
        case PA_FAULT_REC_CLEAR: {
        	BSP_display("Fault_recoverable - PA_FAULT_REC_CLEAR\n");
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
            BSP_display("Fault_unrecoverable-ENTRY;\n");
            Payload_Actuator_HSM.fault = 2;
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
        case PA_FAULT_UNREC: {
        	BSP_display("Fault_unrecoverable - PA_FAULT_UNREC\n");
        	return Q_HANDLED();
        }
    }
    return Q_SUPER(&Fault);
}
/*..........................................................................*/
