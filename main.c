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
#include "yaw.h"


//*****************************************************************************
//
// Main function - Declare variables, initialise functions.
//               - Constantly fills buffer and calculates mean samples and Altitude percentage
//               - Change the display on OLED screen between displaying altitude, raw ADC value and turning screen off
//               - Update display every 4Hz
//
//*****************************************************************************

//*****************************************************************************
//  initAll: Initialises all important functions
void initAll (void) {
    initButtonCheck();
    initClock ();
    initADC ();
    initYaw();
    initButtons ();
    initSysTick ();
    initDisplay ();
    initCircBuf (bufferLocation(), BUF_SIZE);
    IntMasterEnable(); // Enable interrupts to the processor.
    SysCtlDelay (SysCtlClockGet() / 12);
    resetAltitude();
}

int main(void)
{
	initAll();
	while (1)
	{
        OutputToDisplay();
	}
}

