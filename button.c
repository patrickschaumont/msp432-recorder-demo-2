#include "button.h"
#include "swtimer.h"
#include "hwtimer.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

static tSWTimer S1bouncedelay;
static tSWTimer S2bouncedelay;

void InitButtonS1() {
    GPIO_setAsInputPin (GPIO_PORT_P5, GPIO_PIN1); // upper switch S1 on BoostXL
    InitSWTimer(&S1bouncedelay, TIMER32_1_BASE, 100 * (SYSTEMCLOCK/1000)); // 100ms
}

void InitButtonS2() {
    GPIO_setAsInputPin (GPIO_PORT_P3, GPIO_PIN5); // upper switch S1 on BoostXL
    InitSWTimer(&S2bouncedelay, TIMER32_1_BASE, 100 * (SYSTEMCLOCK/1000)); // 100ms
}

int ButtonS1Pressed() {
    typedef enum {stable0, trans0, stable1, trans1} state_t;
    static state_t S = stable0;

    int rval, b;

    // button S1 is pressed
    b = (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 0);

    switch (S) {
    case stable0:
        rval = false;
        if (b) {
            StartSWTimer(&S1bouncedelay);
            S = trans0;
        }
        break;
    case trans0:
        rval = false;
        if (b && SWTimerOneShotExpired(&S1bouncedelay))
            S = stable1;
        if (!b)
            S = stable0;
        break;
    case stable1:
        if (!b) {
            StartSWTimer(&S1bouncedelay);
            S = trans1;
        }
        rval = true;
        break;
    case trans1:
        if (!b && SWTimerOneShotExpired(&S1bouncedelay))
            S = stable0;
        if (b)
            S = stable1;
        rval = true;
        break;
    }

    return rval;
}

int ButtonS2Pressed() {
    typedef enum {stable0, trans0, stable1, trans1} state_t;
    static state_t S = stable0;

    int rval, b;

    // button S2 is pressed
    b = (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5) == 0);

    switch (S) {
    case stable0:
        rval = false;
        if (b) {
            StartSWTimer(&S2bouncedelay);
            S = trans0;
        }
        break;
    case trans0:
        rval = false;
        if (b && SWTimerOneShotExpired(&S2bouncedelay))
            S = stable1;
        if (!b)
            S = stable0;
        break;
    case stable1:
        if (!b) {
            StartSWTimer(&S2bouncedelay);
            S = trans1;
        }
        rval = true;
        break;
    case trans1:
        if (!b && SWTimerOneShotExpired(&S2bouncedelay))
            S = stable0;
        if (b)
            S = stable1;
        rval = true;
        break;
    }

    return rval;
}

