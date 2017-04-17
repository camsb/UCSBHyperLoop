#include "braking_state_machine.h"
#include "qpn_port.h"
#include "logging.h"

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
static QState Tube_permitted_regular(Braking_HSM_t *me);
static QState Tube_permitted_regular_idle(Braking_HSM_t *me);
static QState Tube_permitted_regular_activated(Braking_HSM_t *me);
static QState Tube_permitted_regular_activated_pair1(Braking_HSM_t *me);
static QState Tube_permitted_regular_activated_pair2(Braking_HSM_t *me);
static QState Tube_permitted_emergency(Braking_HSM_t *me);
static QState Test(Braking_HSM_t *me);
static QState Test_idle(Braking_HSM_t *me);
static QState Test_pair1(Braking_HSM_t *me);
static QState Test_pair2(Braking_HSM_t *me);
static QState Test_emergency(Braking_HSM_t *me);

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
    		BSP_display("Nominal-INIT\n");
    		return Q_TRAN(&Tube_lockout);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
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
    		BSP_display("Nominal-INIT\n");
    		return Q_TRAN(&Tube_lockout_nominal);
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube);
}
/*..........................................................................*/

static QState Tube_lockout_nominal(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
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
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            Braking_HSM.using_pushsens = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_ACC_FAULT: {
        	BSP_display("lockout_pusherfault: BRAKES_ACC_FAULT");
        	return Q_TRAN(&Tube_lockout_bothfault);
        }
        case BRAKES_PUSHSENS_GO: {
        	BSP_display("lockout_pusherfault: PUSHSENS_GO");
        	return Q_TRAN(&Tube_permitted);
        }
    }
    return Q_SUPER(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube_lockout_accelfault(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            Braking_HSM.using_accsens = 0;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_PUSHSENS_FAULT: {
        	BSP_display("lockout_ccfault: BRAKES_PUSHSENS_FAULT");
        	return Q_TRAN(&Tube_lockout_bothfault);
        }
        case BRAKES_ACC_GO: {
        	BSP_display("lockout_accfault: ACC_GO");
        	return Q_TRAN(&Tube_permitted);
        }
    }
    return Q_SUPER(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube_lockout_bothfault(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            Braking_HSM.using_pushsens = 0;
            Braking_HSM.using_accsens = 0;
            Braking_HSM.using_timer = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_TIMER_PERMIT: {
        	BSP_display("Tube_lockout_bothfault: BRAKES_TIMER_PERMIT");
        	return Q_TRAN(&Tube_permitted);
        }
    }
    return Q_SUPER(&Tube_lockout);
}
/*..........................................................................*/

static QState Tube_permitted(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_TRAN(&Tube_permitted_regular);
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube);
}
/*..........................................................................*/

static QState Tube_permitted_regular(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_TRAN(&Tube_permitted_regular_idle);
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube_permitted);
}
/*..........................................................................*/

static QState Tube_permitted_regular_idle(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube_permitted_regular);
}
/*..........................................................................*/

static QState Tube_permitted_regular_activated(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube_permitted_regular);
}
/*..........................................................................*/

static QState Tube_permitted_regular_activated_pair1(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube_permitted_regular_activated);
}
/*..........................................................................*/

static QState Tube_permitted_regular_activated_pair2(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube_permitted_regular_activated);
}
/*..........................................................................*/

static QState Tube_permitted_emergency(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Tube_permitted);
}
/*..........................................................................*/

static QState Test(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_TRAN(&Test_idle);
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_TEST_EXIT: {
        	BSP_display("Test - BRAKES_TEST_EXIT");
        	return Q_TRAN(&Tube);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/

static QState Test_idle(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_ENGAGE: {
        	BSP_display("Test_idle - BRAKES_ENGAGE\n");
        	return Q_TRAN(&Test_pair1);
        }
        case BRAKES_EMERGENCY: {
        	BSP_display("Test_idle - BRAKES_EMERGENCY\n");
        	return Q_TRAN(&Test_emergency);
        }
    }
    return Q_SUPER(&Test);
}
/*..........................................................................*/

static QState Test_pair1(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            Braking_HSM.engage_1 = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            Braking_HSM.engage_1 = 0;
            return Q_HANDLED();
        }
        case BRAKES_ENGAGE: {
        	BSP_display("Test_pair1 - ENGAGE\n");
        	return Q_TRAN(&Test_pair2);
        }
        case BRAKES_DISENGAGE: {
        	BSP_display("Test_pair1 - DISENGAGE\n");
        	return Q_TRAN(&Test_idle);
        }
    }
    return Q_SUPER(&Test);
}
/*..........................................................................*/

static QState Test_pair2(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            Braking_HSM.engage_1 = 0;
            Braking_HSM.engage_2 = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
        case BRAKES_ENGAGE: {
        	BSP_display("Test_pair2 - ENGAGE\n");
        	return Q_TRAN(&Test_idle);
        }
        case BRAKES_DISENGAGE: {
        	BSP_display("Test_pair2 - DISENGAGE\n");
        	return Q_TRAN(&Test_idle);
        }
    }
    return Q_SUPER(&Test);
}
/*..........................................................................*/

static QState Test_emergency(Braking_HSM_t *me) {
    switch (Q_SIG(me)) {
    	case Q_INIT_SIG: {
    		BSP_display("Nominal-INIT\n");
    		return Q_HANDLED();
    	}
        case Q_ENTRY_SIG: {
            BSP_display("Nominal-ENTRY\n");
            Braking_HSM.engage_1 = 1;
            Braking_HSM.engage_2 = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            Braking_HSM.engage_1 = 0;
            Braking_HSM.engage_2 = 0;
            return Q_HANDLED();
        }
        case BRAKES_EMERGENCY_RELEASE: {
        	BSP_display("Test_emergency - EMERGENCY_RELEASE\n");
        	return Q_TRAN(&Test_idle);
        }
    }
    return Q_SUPER(&Test);
}
/*..........................................................................*/
