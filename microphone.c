#include "microphone.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void InitADC() {
    ADC14_enableModule();

    // This sets the conversion clock to SYSTEMCLOCK
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC,
                     ADC_PREDIVIDER_1,
                     ADC_DIVIDER_1,
                     0
                     );

    // This configures the ADC to store output results
    // in ADC_MEM0 (single-channel conversion, repeat mode)
    ADC14_configureSingleSampleMode(ADC_MEM0, true);

    // This configures the ADC in automatic conversion mode
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
}

void InitMicrophone() {

    InitADC();

    // This configures ADC_MEM0 to store the result from
    // input channel A10 (Microphone), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A10,
                                    ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A9 is multiplexed on GPIO port P4 pin PIN4
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN3,
                                               GPIO_TERTIARY_MODULE_FUNCTION);
}

unsigned GetSampleMicrophone() {
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    // We wait for the ADC to complete
    while (ADC14_isBusy()) ;

    // and we read the output result from buffer ADC_MEM0
    return ADC14_getResult(ADC_MEM0);
}
