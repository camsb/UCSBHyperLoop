/*****************************************************************************
* QHsmTst example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 07, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qpn_port.h"
#include "hyperloop_sm.h"

/* Hyperloop class -----------------------------------------------------*/
typedef struct HyperloopTag {
    QHsm super;                                        /* extend class QHsm */
                                             /* extended state variables... */
    uint8_t foo;
} Hyperloop;

void   Hyperloop_ctor(void);                                /* the ctor */
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
    HSM_Hyperloop.foo = 0;              /* initialize extended state variable */
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("forward-EXIT;");
            BSP_display("stopping;");
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
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            BSP_display("engines_on-EXIT;");
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
        case DISENGAGE_BRAKES_SIG: {
            BSP_display("disengaging brakes;");
            return Q_TRAN(&Hyperloop_hover);
        }
    }
    return Q_SUPER(&Hyperloop_hover);
}
/*..........................................................................*/
