/*****************************************************************************
* QHsmTst example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 05, 2008
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
#ifndef hyperloop_sm_h
#define hyperloop_sm_h

enum Hyperloop_Signals {
    STOP_SIG = Q_USER_SIG,
    FORWARD_SIG,
    REVERSE_SIG,
    ENGAGE_ENGINES_SIG,
    DISENGAGE_ENGINES_SIG,
    ENGAGE_BRAKES_SIG,
    DISENGAGE_BRAKES_SIG,
    TERMINATE_SIG,
    IGNORE_SIG,
    MAX_SIG
};

extern struct HyperloopTag HSM_Hyperloop;   /* sole instance of hsm_hyperloop */

void Hyperloop_ctor(void);              /* instantiate and initialize the HSM */

/* Board Support Package */
void BSP_display(char const *msg);
void BSP_exit(void);

#endif                                                      /* hyperloop_sm_h */
