#include "qpn_port.h"
#include "state_machine.h"

/* Hyperloop class -----------------------------------------------------*/
typedef struct HyperloopTag {
    QHsm super;                                        	 /* extend class QHsm */                                         
    uint8_t engine_throttle[NUM_ENGINES];		 /* extended state variables... */
    uint8_t engine_flag = 0;
    uint8_t brake_flag = 0;
    uint8_t service_flag = 0;
    uint8_t direction = 0;
} Hyperloop;

void   Hyperloop_ctor(void);                             /* the ctor */
static QState Hyperloop_initial(Hyperloop *me);          /* initial handler */
static QState Hyperloop_stationary(Hyperloop *me);       /* state handler */
static QState Hyperloop_idle(Hyperloop *me);            
static QState Hyperloop_forward(Hyperloop *me);
static QState Hyperloop_reverse(Hyperloop *me);
static QState Hyperloop_engines_on(Hyperloop *me); 
static QState Hyperloop_rev_engines(Hyperloop *me);
static QState Hyperloop_power_down(Hyperloop *me);
//static QState Hyperloop_hovering(Hyperloop *me);
static QState Hyperloop_hover(Hyperloop *me);
static QState Hyperloop_braking(Hyperloop *me);

/* global objects ----------------------------------------------------------*/
Hyperloop HSM_Hyperloop;              /* the sole instance of the QHsmTst HSM */

/*..........................................................................*/
void Hyperloop_ctor(void) {
    QHsm_ctor(&HSM_Hyperloop.super, (QStateHandler)&Hyperloop_initial);
    HSM_Hyperloop.brake_flag = 0;       // are brakes engaged?
    HSM_Hyperloop.engine_flag = 0;	// are engines on?
    HSM_Hyperloop.service_flag = 0;	// are service motors in use?
    HSM_Hyperloop.direction = 0;	// service motor direction, 0 for forward, 1 for reverse
    for(int i = 0; i < NUM_ENGINES; i++)
	    HSM_Hyperloop.engine_throttle[i] = 0;
}

/*..........................................................................*/
QState Hyperloop_initial(Hyperloop *me) {
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
	    for(int i = 0; i < NUM_ENGINES; i++)
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
            BSP_exit();
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
            BSP_exit();
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
	    for(int i = 0; i < NUM_ENGINES; i++)
	    	HSM_Hyperloop.engine_throttle[i] = 0.8;		//rev up to a tenth of throttle
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
    }
    return Q_SUPER(&Hyperloop_engines_on);
}
/*..........................................................................*/
/*
QState Hyperloop_hovering(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_display("hovering-ENTRY;");
            BSP_display("hovering at constant engine rpm;");
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("hovering-EXIT;");
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            BSP_display("hovering-INIT;");
            return Q_TRAN(&Hyperloop_hover);

            //return Q_HANDLED();
        }
    }
    return Q_SUPER(&Hyperloop_engines_on);
}
*/
/*..........................................................................*/
QState Hyperloop_hover(Hyperloop *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
	    BSP_display("hover-ENTRY;");
            BSP_display("maintaining hovering conditions;");
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
    return Q_SUPER(&Hyperloop_hover);
}
/*..........................................................................*/
