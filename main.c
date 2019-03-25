//*****************************************************************************
//
// Milestone 1 - Displaying helicopter altitude as a percentage
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:	26.3.2019
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 40
#define MAX_STR_LEN 16
#define UART_USB_BASE           UART0_BASE
#define CHANGE 0.8
#define SYSTICK_RATE_HZ    100
#define range_value 800*4095/3300

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;	// Counter for the interrupts



//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
    updateButtons();
}

void
initSysTick (void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick
    // timer period is set as a function of the system clock.
    SysTickPeriodSet (SysCtlClockGet() / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister (SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable ();
    SysTickEnable ();
}


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

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
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

//*****************************************************************************
//
// Initialise Display
//
//*****************************************************************************
void
initDisplay (void)
{
    // initialise the Orbit OLED display
    OLEDInitialise ();
}

//*****************************************************************************
//
// Function to display the mean ADC value (10-bit value, note) and sample count.
//
//*****************************************************************************
void
displayMeanVal(int16_t meanVal, bool percent)
{
	char string[17];  // 16 characters across the display

    OLEDStringDraw ("Milestone 1", 0, 0);
	
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    if(percent){
        usnprintf (string, sizeof(string), "Height = %4d%%", meanVal);
    } else {
        usnprintf (string, sizeof(string), "ADC Value = %4d", meanVal);
    }
    // Update line on display.
    OLEDStringDraw (string, 0, 1);
}

//*****************************************************************************
//
// Main function - Declare variables, initialise functions.
//               - Constantly fills buffer and calculates mean samples and Altitude percentage
//               - Change the display on OLED screen between displaying altitude, raw ADC value and turning screen off
//               - Update display every 4Hz
//
//*****************************************************************************
int main(void)
{
    // Declaring Variables
	uint16_t count = 0;     // Used as a case trigger
	int32_t sum;            // Sum of samples in buffer
	int32_t minHeight;      // Reference height (0%)
	int32_t ADC_Altitude;   // Altitude of helicopter (0-4095)
	int percent = 0;        // Stores the calculated percentage of altitude
	int change;             // Change in Altitude (minHeight - ADC_Altitude)
	int i;
	bool firstRun = true;   // Used as a flag to say that initial height has been calibrated

	SysCtlPeripheralReset (LEFT_BUT_PERIPH);//setting up the LEFT button GPIO
	SysCtlPeripheralReset (UP_BUT_PERIPH);//setting the UP button GPIO

	//Initialisation
	initClock ();
	initADC ();

	initButtons ();
	initSysTick ();

	initDisplay ();
	initCircBuf (&g_inBuffer, BUF_SIZE);

    //
    // Enable interrupts to the processor.
    IntMasterEnable();

	while (1)
	{
	    sum = 0;    // Resets the sum to 0 before summing another buffer load

		// Background task: calculate the (approximate) mean of the values in the
		// circular buffer and display it, together with the sample number.
		for (i = 0; i < BUF_SIZE; i++) {
			sum = sum + readCircBuf (&g_inBuffer);
		}
		ADC_Altitude = ((2 * sum + BUF_SIZE) / 2 / BUF_SIZE);   // Calculates the mean from 10 samples

		if(firstRun) {      // Sets the initial Height as the minHeight
		    minHeight = ADC_Altitude;
		    if(minHeight > 0){
		        firstRun = false;
		    }
		} else{
		    if(checkButton(LEFT) == PUSHED){        // If left button pushed, set the current Height as the minHeight
		        minHeight = ADC_Altitude;
		    } if(checkButton(UP) == PUSHED){        // If up button pushed, increment count
		        count++;                            // Count goes from 0 to 3 and resets back to 0
		    }
		    change =  minHeight-ADC_Altitude;       // Calculate the change in height
		    percent = 100*change/range_value;       // Calculate the altitude percentage
		}
		switch(count%3) {
            case 0:         // If count is changed to 0, display the Altitude in percentage
                displayMeanVal (percent, true);
                break;
            case 1:         // If count is changed to 1, display the raw ADC value
                displayMeanVal(ADC_Altitude, false);
                break;
            case 2:         // If count is changed to 2, turn off display
                OrbitOledClear();
		}

		SysCtlDelay (SysCtlClockGet() / 12);  // Update display at ~ 4 Hz
	}
}

