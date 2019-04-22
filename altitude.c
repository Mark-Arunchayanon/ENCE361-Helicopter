//*****************************************************************************
//
// Milestone 1 - Displaying helicopter altitude as a percentage
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   20.4.2019
//
//*****************************************************************************

#include "system.h"

//*****************************************************************************
// Altitude Constants
//*****************************************************************************
#define RANGE_ALTITUDE 800*4095/3300

//*****************************************************************************
// Altitude Global variables
//*****************************************************************************

static uint32_t min_Altitude;
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void
ADCIntHandler(void)
{
    uint32_t ulValue;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

void
initADC (void)
{
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 9 (ADC_CTL_CH9) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

int32_t
computeAltitude (void)
{
    int AltSum = 0;
    int i = 0;

    for (i = 0; i < BUF_SIZE; i++) {
            AltSum = AltSum + readCircBuf (&g_inBuffer);
    }
    //ADC_Altitude = ((2 * AltSum + BUF_SIZE) / 2 / BUF_SIZE);   // Calculates the mean from 10 samples
    //Change_Altitude = min_Altitude-ADC_Altitude;       // Calculate the change in height
    //Percent_Altitude = 100*Change_Altitude/RANGE_ALTITUDE;       // Calculate the altitude percentage
    return ((2 * AltSum + BUF_SIZE) / 2 / BUF_SIZE);
}


void
resetAltitude (void)
{
    min_Altitude = computeAltitude();
}

int32_t
percentAltitude(void)
{
    int32_t percent = 0;
    percent = 100*(min_Altitude-computeAltitude());
    return percent/RANGE_ALTITUDE;
}

circBuf_t*
bufferLocation(void)
{
    return &g_inBuffer;
}
