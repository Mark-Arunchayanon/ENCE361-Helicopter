//*****************************************************************************
//
// System - System Related Files and includes
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   20.4.2019
//*****************************************************************************

#define NUM_SLOTS 112
#define TOTAL_ANGLE 360
#include "system.h"
#include "inc/tm4c123gh6pm.h"

volatile int32_t slot;

int32_t getYaw(void) {
    return slot;
}



void YawIntHandler (void) {

    uint32_t status = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);
    /*if (status == 1) {
        slot++;
    } else if (status == 2) {
        slot--;
    }*/
    count++;
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}

void initYaw (void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    IntEnable(INT_GPIOB);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_BOTH_EDGES);
    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler);
}


