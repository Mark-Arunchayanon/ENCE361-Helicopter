//*****************************************************************************
//
// Yaw - Calculating yaw slot numbers and angles functions through an Interrupt
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   23.4.2019
//*****************************************************************************

#define NUM_SLOTS               448
#define TOTAL_ANGLE             360
#define FIND_REF_MAIN           30 //duty cycle for finding the reference point
#define FIND_REF_TAIL           40

#include "system.h"
#include "driverlib/gpio.h"
#include "control.h"
#include "motor.h"


#include "inc/tm4c123gh6pm.h"


enum quad {A = 0, B = 1, C = 3, D = 2};
//int32_t State;
//int32_t nextState;
enum quad State;
enum quad nextState;
int32_t slot;
// *******************************************************
// getYaw: Uses the current slot number on the disk to
// return an angle in degrees from the original reference point.
// RETURNS a angle value between -180 < Yaw < 180 degrees.
int32_t getYaw(void) {
    int32_t angle = 0;
    int32_t refnum = slot;
    while (refnum > NUM_SLOTS / 2) {
        refnum -= NUM_SLOTS;
    }
    while (refnum < -NUM_SLOTS / 2) {
        refnum += NUM_SLOTS;
    }
//  Slots converted into an angle and returned as an angle.
    angle = TOTAL_ANGLE * refnum / NUM_SLOTS;
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
    //Tell the registers to clear the interrupt bits
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    //Do the state stuff
    nextState = (enum quad)GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    switch(State)
    {
    case A:
        switch (nextState)
        {
            case B:
                slot--;
                break;
            case D:
                slot++;
                break;
        }
        break;
    case B:
        switch (nextState)
        {
        case C:
            slot--;
            break;
        case A:
            slot++;
            break;
        }
        break;
    case C:
    {
        switch(nextState)
        {
        case D:
            slot--;
            break;
        case B:
            slot++;
            break;
        }
        break;
    }
    case D:
    {
        switch(nextState)
        {
        case A:
            slot--;
            break;
        case C:
            slot++;
            break;
        }
        break;
    }
    }
    State = nextState;
}

// *******************************************************
//  YawIntHandler: Interrupt initialisation for the yaw interrupt.
//  Sets PB0 and PB1 to be inputs, enables interrupts on GPIOB.
//  An interrupt occurs on both edges of PB0 and PB1 and when triggered,
//  runs the YawIntHandler function
void initYaw (void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
    IntEnable(INT_GPIOB);
}


