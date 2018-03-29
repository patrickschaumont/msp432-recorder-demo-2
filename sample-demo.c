#include <stdlib.h>
#include "swtimer.h"
#include "hwtimer.h"
#include "microphone.h"
#include "button.h"
#include "led.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// - This application demonstrates how a PWM signal can be used as a simulated
//   D/A converter.
//
// - Samples are recorded in a memory buffer (2 seconds, truncated to 8 bit/sample).
//
// - Next, they are used to define the duty cycle from a PWM signal with
//   a 64KHz carrier.
//
// - This application works best if you put your thumb over the buzzer during
//   playback. A buzzer is not designed to render analog signals, but
//   it helps to muffle the buzzer sound to 'demodulate' the PWM back into audio

#define SAMPLESPAN 16000
char sample[SAMPLESPAN];
unsigned glbSamplePointer = 0;
unsigned glbListening = 0;
unsigned glbPlaying = 0;

char scaleSample(unsigned vx) {
    // microphone sample is 0x1000 to 0x2FFF midpoint 0x1FFF
    // output is 0x00 to 0xFF midpoint 0x80
    // set gain to 4
    int c = (vx - 0x1FFF) * 0x100 * 4 / 0x2000 + 0x80;
    return (char) c;
}

#define CARRIER ((int) (SYSTEMCLOCK / 64000))
#define SAMPLERATE ((int) (SYSTEMCLOCK / 8000))

Timer_A_UpModeConfig upConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,
    TIMER_A_CLOCKSOURCE_DIVIDER_1,       // 3 MHz
    SAMPLERATE,
    TIMER_A_TAIE_INTERRUPT_DISABLE,
    TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
    TIMER_A_SKIP_CLEAR
};

Timer_A_PWMConfig pwmConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,       // 3 MHz
        CARRIER,                             // 64 Khz Carrier
        TIMER_A_CAPTURECOMPARE_REGISTER_4,
        TIMER_A_OUTPUTMODE_RESET_SET,
        CARRIER / 2
};

void TA1_0_IRQHandler() {
    unsigned vx;
    if (glbListening) {
        vx = GetSampleMicrophone();
        sample[glbSamplePointer++] = scaleSample(vx);
        if (glbSamplePointer == SAMPLESPAN) {
            glbListening = 0;
            glbSamplePointer = 0;
        }
    }
    if (glbPlaying) {
        pwmConfig.dutyCycle = CARRIER * sample[glbSamplePointer++] / 256;
        Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
        if (glbSamplePointer == SAMPLESPAN) {
            glbPlaying = 0;
            glbSamplePointer = 0;
            Timer_A_stopTimer(TIMER_A0_BASE);
        }
    }
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
                                         TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

void InitSound() {
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P2,
            GPIO_PIN7,
            GPIO_PRIMARY_MODULE_FUNCTION);
}

int main(void) {

    InitTimer();
    InitMicrophone();
    InitButtonS1();
    InitButtonS2();
    InitLEDs();
    InitSound();

    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    Interrupt_enableInterrupt(INT_TA1_0);
    Interrupt_enableMaster();

    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    while (1) {
        if (ButtonS1Pressed())
            glbListening = 1;

        if (ButtonS2Pressed())
            glbPlaying = 1;

        if (glbListening | glbPlaying)
            TurnON_Launchpad_Left_LED();
        else
            TurnOFF_Launchpad_Left_LED();

    }
}
