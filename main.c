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
#include "motor.h"
#include "control.h"
#include "uart.h"



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
    resetmotor();
    initButtonCheck();
    initClock();
    initADC();
    initYaw();
    initSysTick();
    initButtons();
    initSwitch_PC4();

    initDisplay();
    initialiseUSB_UART();
    initCircBuf(bufferLocation(), BUF_SIZE);
    initmotor();
    IntMasterEnable(); // Enable interrupts to the processor.
    SysCtlDelay(SysCtlClockGet() / 12);
    resetAltitude();
}

int main(void)
{
    initAll();
    char statusStr[MAX_STR_LEN + 1];



    while (1)
	{
        //start running through the states of the helicopter
        helicopterStates();

        OutputToDisplay();
        if (slowTick)
        {
            slowTick = false;
            // Form and send a status message to the console
            usprintf (statusStr, "YawRef=%2d Yaw=%2d | \r\n", GetYawRef(), getYaw()); // * usprintf
            UARTSend (statusStr);
            usprintf (statusStr, "AltRef=%2d Alt=%2d | \r\n", GetAltRef(), percentAltitude()); // * usprintf
            UARTSend (statusStr);
            usprintf (statusStr, "MDut=%2d TDuty=%2d | \r\n", getMainDuty(), getTailDuty()); // * usprintf
            UARTSend (statusStr);
            usprintf (statusStr, "Mode=%s | \r\n", getMode()); // * usprintf
            UARTSend (statusStr);
            landing();
        }

        // Is it time to send a message?

	}
}

