#ifndef SWTIMER_H_
#define SWTIMER_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef struct {
    uint32_t hwtimer;  // hardware timer used as basis for this software timer
    uint32_t period;   // period of the software timer
    uint32_t bound;    // next expiration time for software timer
    int      expired;
} tSWTimer;

void InitSWTimer          (tSWTimer *T,
                           uint32_t  hwtimer,
                           uint32_t  period);
void StartSWTimer         (tSWTimer *T);
int  SWTimerExpired       (tSWTimer *T);
int  SWTimerOneShotExpired(tSWTimer *T);

#endif /* SWTIMER_H_ */
