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
    ENGINES_REVED_SIG,
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
