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
    Braking_HSM.enable_motors = 0;			// Whether to provide throttle signal to maglev motors
    Braking_HSM.update = 0; 				// Whether there was a change in enable flag
    Braking_HSM.send_spunup = 0;
    Braking_HSM.send_spundown = 0;
    Braking_HSM.faulted = 0;
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
        // ADD TRANS TO SENSOR FAULT STATES HERE

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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
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

static QState Tube_permitted_regular(Braking_HSM_t *me) {
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("Nominal-EXIT\n");
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Test);
}
/*..........................................................................*/
