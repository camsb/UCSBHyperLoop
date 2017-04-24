#include "braking_state_machine.h"
#include "qpn_port.h"
#include "logging.h"
#include <stdio.h>

// Macro to re-direct the state machine debug test to DEBUGOUT only if SM_DEBUG is 1
#define SM_DEBUG 0
#define BSP_display(msg) if(SM_DEBUG){DEBUGOUT(msg);}

// The global instance of the state machine object
Braking_HSM_t Braking_HSM;

// Forward declaration of all the possible states
static QState Initial(Braking_HSM_t *me);
static QState Tube(Braking_HSM_t *me);
static QState Tube_lockout(Braking_HSM_t *me);
static QState Tube_lockout_nominal(Braking_HSM_t *me);
static QState Tube_lockout_pusherfault(Braking_HSM_t *me);
static QState Tube_lockout_accelfault(Braking_HSM_t *me);
static QState Tube_lockout_bothfault(Braking_HSM_t *me);
static QState Tube_permitted(Braking_HSM_t *me);
static QState Tube_permitted_idle(Braking_HSM_t *me);
static QState Tube_permitted_activated(Braking_HSM_t *me);
static QState Test(Braking_HSM_t *me);
static QState Test_idle(Braking_HSM_t *me);
static QState Test_activated(Braking_HSM_t *me);

/*..........................................................................*/
void initializeBrakingStateMachine(void) {
    QHsm_ctor(&Braking_HSM.super, (QStateHandler)&Initial);
    Braking_HSM.stationary_test = 0;
    Braking_HSM.using_pushsens = 1;
    Braking_HSM.using_accsens = 1;
    Braking_HSM.using_timer = 0;
    Braking_HSM.engage_1 = 0;
    Braking_HSM.engage_2 = 0;
    QHsm_init((QHsm *)&Braking_HSM);
}

/*..........................................................................*/
static QState Initial(Braking_HSM_t *me) {
    BSP_display("Initial-INIT\n");
    return Q_TRAN(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube-INIT\n");
    		return Q_TRAN(&Tube_lockout);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_TEST_ENTER: {
        	BSP_display("Tube - BRAKES_TEST_ENTER\n");
        	return Q_TRAN(&Test);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/

static QState Tube_lockout(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_lockout-INIT\n");
    		return Q_TRAN(&Tube_lockout_nominal);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_lockout-ENTRY\n");
            Braking_HSM.engage_1 = 0;
            Braking_HSM.engage_2 = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_lockout-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube);
}
/*..........................................................................*/

static QState Tube_lockout_nominal(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_lockout_nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_lockout_nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_lockout_nominal-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_PUSHSENS_FAULT: {
        	BSP_display("Tube_lockout_nomial: PUSHSENS_FAULT\n");
        	return Q_TRAN(&Tube_lockout_pusherfault);
        }
        case BRAKES_ACC_FAULT: {
        	BSP_display("Tube_lockout_nominal: BRAKES_ACC_FAULT\n");
        	return Q_TRAN(&Tube_lockout_accelfault);
        }
        case BRAKES_BOTHSENS_GO: {
        	BSP_display("tube_lockout_nominal: BOTHSENS_GO\n");
        	return Q_TRAN(&Tube_permitted);
        }
    }
    return Q_SUPER(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube_lockout_pusherfault(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_lockout_pusherfault-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_lockout_pusherfault-ENTRY\n");
            Braking_HSM.using_pushsens = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_lockout_pusherfault-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_ACC_FAULT: {
        	BSP_display("Tube_lockout_pusherfault: BRAKES_ACC_FAULT\n");
        	return Q_TRAN(&Tube_lockout_bothfault);
        }
        case BRAKES_PUSHSENS_GO: {
        	BSP_display("Tube_lockout_pusherfault: PUSHSENS_GO\n");
        	return Q_TRAN(&Tube_permitted);
        }
        case BRAKES_BOTHSENS_GO: {
        	BSP_display("Tube_lockout_pusherfault: BOTHSENS_GO\n");
        	return Q_TRAN(&Tube_permitted);
        }
    }
    return Q_SUPER(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube_lockout_accelfault(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_lockout_accelfault-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_lockout_accelfault-ENTRY\n");
            Braking_HSM.using_accsens = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_lockout_accelfault-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_PUSHSENS_FAULT: {
        	BSP_display("Tube_lockout_accelfault: BRAKES_PUSHSENS_FAULT\n");
        	return Q_TRAN(&Tube_lockout_bothfault);
        }
        case BRAKES_ACC_GO: {
        	BSP_display("Tube_lockout_accelfault: ACC_GO\n");
        	return Q_TRAN(&Tube_permitted);
        }
        case BRAKES_BOTHSENS_GO: {
        	BSP_display("Tube_lockout_accelfault: BOTHSENS_GO\n");
        	return Q_TRAN(&Tube_permitted);
        }
    }
    return Q_SUPER(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube_lockout_bothfault(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_lockout_bothfault-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_lockout_bothfault-ENTRY\n");
            Braking_HSM.using_pushsens = 0;
            Braking_HSM.using_accsens = 0;
            Braking_HSM.using_timer = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_lockout_bothfault-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_TIMER_PERMIT: {
        	BSP_display("Tube_lockout_bothfault: BRAKES_TIMER_PERMIT\n");
        	return Q_TRAN(&Tube_permitted);
        }
    }
    return Q_SUPER(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube_permitted(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_permitted-INIT\n");
            return Q_TRAN(&Tube_permitted_idle);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_permitted-ENTRY\n");
    		return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_permitted-EXIT\n");
            Braking_HSM.engage_1 = 0;
            Braking_HSM.engage_2 = 0;
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube);
}
/*..........................................................................*/

static QState Tube_permitted_idle(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_permitted_idle-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_permitted_idle-ENTRY\n");
            Braking_HSM.engage_1 = 0;
            Braking_HSM.engage_2 = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_permitted_idle-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_ENGAGE: {
        	BSP_display("Tube_permitted_idle - BRAKES_ENGAGE\n");
        	return Q_TRAN(&Tube_permitted_activated);
        }
    }
    return Q_SUPER(&Tube_permitted);
}
/*..........................................................................*/

static QState Tube_permitted_activated(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Tube_permitted_activated-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Tube_permitted_activated-ENTRY\n");
            Braking_HSM.engage_1 = 1;
            Braking_HSM.engage_2 = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Tube_permitted_activated-EXIT\n");
            Braking_HSM.engage_1 = 0;
            Braking_HSM.engage_2 = 0;
            return Q_HANDLED();
        }
        case BRAKES_DISENGAGE: {
        	BSP_display("Tube_permitted_activated - BRAKES_DISENGAGE\n");
        	return Q_TRAN(&Tube_permitted_idle);
        }
    }
    return Q_SUPER(&Tube_permitted);
}
/*..........................................................................*/

static QState Test(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Test-INIT\n");
            return Q_TRAN(&Test_idle);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Test-ENTRY\n");
    		return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Test-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_TEST_EXIT: {
        	BSP_display("Test - BRAKES_TEST_EXIT\n");
        	return Q_TRAN(&Tube);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/

static QState Test_idle(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Test_idle-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Test_idle-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Test_idle-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_ENGAGE: {
        	BSP_display("Test_idle - BRAKES_ENGAGE\n");
        	return Q_TRAN(&Test_activated);
        }
    }
    return Q_SUPER(&Test);
}
/*..........................................................................*/

static QState Test_activated(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Test_activated-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Test_activated-ENTRY\n");
            Braking_HSM.engage_1 = 1;
            Braking_HSM.engage_2 = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Test_activated-EXIT\n");
            Braking_HSM.engage_1 = 0;
            Braking_HSM.engage_2 = 0;
            return Q_HANDLED();
        }
        case BRAKES_DISENGAGE: {
        	BSP_display("Test_activated - BRAKES_DISENGAGE\n");
        	return Q_TRAN(&Test_idle);
        }
    }
    return Q_SUPER(&Test);
}
/*..........................................................................*/
