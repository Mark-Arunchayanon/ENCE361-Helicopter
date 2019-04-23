//*****************************************************************************
//
// Yaw - Calculating yaw slot numbers and angles functions through an Interrupt
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   23.4.2019
//*****************************************************************************

#define NUM_SLOTS 112
#define TOTAL_ANGLE 360
#include "system.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"

volatile int32_t slot;
volatile int32_t phaseB;
volatile int32_t phaseA;

// *******************************************************
// getYaw: Uses the current slot number on the disk to
// return an angle in degrees from the original reference point.
// RETURNS a angle value between -180 < Yaw < 180 degrees.
int32_t getYaw(void) {
    int32_t angle = 0;

    //Slot number adjusted between -61 and 61.
    if (slot > NUM_SLOTS/2) {
        slot -= NUM_SLOTS;
    } else if (slot < -NUM_SLOTS/2) {
        slot += NUM_SLOTS;
    }
//  Slots converted into an angle and returned as an angle.
    angle = TOTAL_ANGLE * slot / NUM_SLOTS;
    return angle;
}

// *******************************************************
// resetYaw: Resets the reference point for slot number.
void resetYaw (void) {
    slot = 0;
}

// *******************************************************
//  YawIntHandler: Interrupt handler for the yaw interrupt.
//  Measures PhaseB. If PhaseB generated the interrupt, add 1 to slot.
//  If PhaseA generated the interrupt, minus 1 from slot.
void YawIntHandler (void) {
    phaseA = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);
    phaseB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
    if (phaseB == 2 && phaseA == 0) { //Rotating Clockwise
        slot++;
    } else if (phaseB == 0) { //Rotating Anti-clockwise
        slot--;
    }
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

// *******************************************************
//  YawIntHandler: Interrupt initialisation for the yaw interrupt.
//  Sets PB0 and PB1 to be inputs, enables interrupts on GPIOB.
//  An interrupt occurs on both edges of PB0 and PB1 and when triggered,
//  runs the YawIntHandler function
void initYaw (void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    IntEnable(INT_GPIOB);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_BOTH_EDGES);
    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
}


