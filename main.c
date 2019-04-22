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

#include "system.h"
#include "altitude.h"
#include "display.h"


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
	SysCtlPeripheralReset (LEFT_BUT_PERIPH);//setting up the LEFT button GPIO
	SysCtlPeripheralReset (UP_BUT_PERIPH);//setting the UP button GPIO

	//Initialisation
	initClock ();
	initADC ();

	initButtons ();
	initSysTick ();

	initDisplay ();
	initCircBuf (bufferLocation(), BUF_SIZE);
    //
    // Enable interrupts to the processor.
    IntMasterEnable();
    SysCtlDelay (SysCtlClockGet() / 12);  // Update display at ~ 4 Hz
    resetAltitude();

	while (1)
	{
        OutputToDisplay();

	}
}

