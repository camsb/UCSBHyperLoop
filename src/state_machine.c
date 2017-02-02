#include "qpn_port.h"
#include "state_machine.h"
#include "HEMS.h"

// Macro to re-direct the state machine debug test to DEBUGOUT only if SM_DEBUG is 1
#define SM_DEBUG 0
#define BSP_display(msg) if(SM_DEBUG){DEBUGOUT(msg);}

/* The global instance of the state machine object -------------------------*/
Hyperloop HSM_Hyperloop;

// Forward declaration of all the possible states
static QState Hyperloop_initial(Hyperloop *me);
static QState Hyperloop_stationary(Hyperloop *me);
static QState Hyperloop_idle(Hyperloop *me);
static QState Hyperloop_forward(Hyperloop *me);
static QState Hyperloop_reverse(Hyperloop *me);
static QState Hyperloop_engines_on(Hyperloop *me);
static QState Hyperloop_rev_engines(Hyperloop *me);
static QState Hyperloop_power_down(Hyperloop *me);
static QState Hyperloop_hover(Hyperloop *me);
static QState Hyperloop_braking(Hyperloop *me);

/*..........................................................................*/
void initializeStateMachine(void) {
    QHsm_ctor(&HSM_Hyperloop.super, (QStateHandler)&Hyperloop_initial);
    HSM_Hyperloop.brake_flag = 0;       // are brakes engaged?
    HSM_Hyperloop.engine_flag = 0;	    // are engines on?
    HSM_Hyperloop.service_flag = 0;	    // are service motors in use?
    HSM_Hyperloop.direction = 0;	    // service motor direction, 0 for forward, 1 for reverse
    HSM_Hyperloop.update = 1;		    // has a transition occurred?
    int i;
    for(i = 0; i < NUM_MOTORS; i++)
	    HSM_Hyperloop.engine_throttle[i] = 0;
    QHsm_init((QHsm *)&HSM_Hyperloop);
}

/*..........................................................................*/
static QState Hyperloop_initial(Hyperloop *me) {
    BSP_display("top-INIT;");
    return Q_TRAN(&Hyperloop_idle);
}
/*..........................................................................*/
QState Hyperloop_stationary(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("stationary-ENTRY;");
	    HSM_Hyperloop.brake_flag = 0;
    	    HSM_Hyperloop.engine_flag = 0;
    	int i;
	    for(i = 0; i < NUM_MOTORS; i++)
	    	HSM_Hyperloop.engine_throttle[i] = 0;
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
        case TERMINATE_SIG: {
            //BSP_exit();
            // TODO: Handle this.
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Hyperloop_idle(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("idle-ENTRY;");
    	    HSM_Hyperloop.service_flag = 0;
    	    HSM_Hyperloop.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("idle-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("idle-INIT;");
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            //BSP_exit();
            // TODO: Handle this.
            return Q_HANDLED();
        }
        case FORWARD_SIG: {
            return Q_TRAN(&Hyperloop_forward);
        }
        case REVERSE_SIG: {
            return Q_TRAN(&Hyperloop_reverse);
        }
        case ENGAGE_ENGINES_SIG: {
            return Q_TRAN(&Hyperloop_engines_on);
        }
    }
    return Q_SUPER(&Hyperloop_stationary);
}
/*..........................................................................*/
QState Hyperloop_forward(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("forward-ENTRY;");
            BSP_display("moving forward;");
	    HSM_Hyperloop.service_flag = 1;
    	    HSM_Hyperloop.direction = 0;
	    HSM_Hyperloop.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("forward-EXIT;");
            BSP_display("stopping;");
	    HSM_Hyperloop.service_flag = 0;
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("forward-INIT;");
            return Q_HANDLED();
        }
        case STOP_SIG: {
            return Q_TRAN(&Hyperloop_idle);
        }
    }
    return Q_SUPER(&Hyperloop_stationary);
}
/*..........................................................................*/
QState Hyperloop_reverse(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("reverse-ENTRY;");
            BSP_display("moving in reverse;");
	    HSM_Hyperloop.service_flag = 1;
    	    HSM_Hyperloop.direction = 1;
	    HSM_Hyperloop.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("reverse-EXIT;");
            BSP_display("stopping;");
	    HSM_Hyperloop.service_flag = 0;
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("reverse-INIT;");
            return Q_HANDLED();
        }
        case STOP_SIG: {
            return Q_TRAN(&Hyperloop_idle);
        }
    }
    return Q_SUPER(&Hyperloop_stationary);
}
/*..........................................................................*/
QState Hyperloop_engines_on(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("engines_on-ENTRY;");
	    HSM_Hyperloop.engine_flag = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("engines_on-EXIT;");
	    HSM_Hyperloop.engine_flag = 0;
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("engines_on-INIT;");
            return Q_TRAN(&Hyperloop_rev_engines);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Hyperloop_rev_engines(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("rev_engines-ENTRY;");
            BSP_display("revving engines;");
        int i;
	    for(i = 0; i < NUM_MOTORS; i++)
	    	HSM_Hyperloop.engine_throttle[i] = 0.8;		//rev up to a tenth of throttle
	    HSM_Hyperloop.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("rev_engines-EXIT;");
            BSP_display("engines revved;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("rev_engines-INIT;");
            return Q_HANDLED();
        }
        case ENGINES_REVED_SIG: {
            BSP_display("Rev-sig!;");
            return Q_TRAN(&Hyperloop_hover);
        }
    }
    return Q_SUPER(&Hyperloop_engines_on);
}
/*..........................................................................*/
QState Hyperloop_power_down(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("power_down-ENTRY;");
        int i;
	    for(i = 0; i < NUM_MOTORS; i++)
	    	HSM_Hyperloop.engine_throttle[i] = 0;	
	    HSM_Hyperloop.update = 1;

	        return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("power_down-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("power_down-INIT;");
            return Q_HANDLED();
        }
	case ENGINES_STOPPED_SIG: {
            BSP_display("stopped-sig!;");
            return Q_TRAN(&Hyperloop_idle);
        }
    }
    return Q_SUPER(&Hyperloop_engines_on);
}
/*..........................................................................*/
QState Hyperloop_hover(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
	    BSP_display("hover-ENTRY;");
            BSP_display("maintaining hovering conditions;");
	    HSM_Hyperloop.update = 1;	
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
        case ENGAGE_BRAKES_SIG: {
            return Q_TRAN(&Hyperloop_braking);
        }
        case DISENGAGE_ENGINES_SIG: {
	    return Q_TRAN(&Hyperloop_power_down);
        }
    }
    return Q_SUPER(&Hyperloop_engines_on);
}
/*..........................................................................*/
QState Hyperloop_braking(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("braking-ENTRY;");
            BSP_display("engaging brakes;");
	    HSM_Hyperloop.brake_flag = 1;
	    HSM_Hyperloop.update = 1;
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("braking-EXIT;");
	    HSM_Hyperloop.brake_flag = 0;
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("braking-INIT;");
            return Q_HANDLED();
        }
        case DISENGAGE_BRAKES_SIG: {
            BSP_display("disengaging brakes;");
            return Q_TRAN(&Hyperloop_hover);
        }
    }
    return Q_SUPER(&Hyperloop_engines_on);
}
/*..........................................................................*/

// This assertion function is required for the state machine. It's called if things go haywire.
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    DEBUGOUT("Assertion failed in %s, line %d", file, line);
    //exit(-1);
}
