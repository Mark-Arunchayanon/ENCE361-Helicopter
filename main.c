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
//  initAll: Initialises all buttons, interrupts, ADC, PWM, modes and controls
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

//*****************************************************************************
// Main:            Controls the altitude and yaw of a model helicopter
int main(void)
{
    initAll();
    while (1)
	{
        //start running through the states of the helicopter
        helicopterStates();
        OutputToUART();
	}
}

