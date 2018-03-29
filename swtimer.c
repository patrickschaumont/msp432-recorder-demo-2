#include "swtimer.h"

void InitSWTimer(tSWTimer *T,
                 uint32_t  hwtimer,
                 uint32_t  period) {
    T->hwtimer = hwtimer;
    T->period  = period;
    T->bound   = Timer32_getValue(hwtimer) - period;
    T->expired = 0;
}

void StartSWTimer(tSWTimer *T) {
    T->bound   = Timer32_getValue(T->hwtimer) - T->period;
    T->expired = 0;
}

int SWTimerExpired(tSWTimer *T) {
    int expired;
    uint32_t delta = Timer32_getValue(T->hwtimer) - T->bound;
    expired = (delta > T->period);
    if (expired)
        T->bound = T->bound - T->period;
    return expired;
}

int SWTimerOneShotExpired(tSWTimer *T) {
    int expired;
    uint32_t delta = Timer32_getValue(T->hwtimer) - T->bound;
    expired = (delta > T->period);
    if (expired)
        T->expired = 1;
    return T->expired;
}
