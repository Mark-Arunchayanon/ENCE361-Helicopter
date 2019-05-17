/*
 * states.c
 *
 *  Created on: 17/05/2019
 *      Author: par116
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "buttons4.h"

#include "inc/hw_ints.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#define SWITCH_ON 128;

typedef enum {Initialising, Flying, Landing, Landed} mode_type;

int32_t previous_sw;
int32_t current_sw;
int switch_off = 0;

// Set up switch 1
void
initSwitch(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA,
           GPIO_PIN_TYPE_STD_WPD);
}

void
initStates(void)
{
    mode_type mode = Initialising;
    previous_sw = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7);

    if (previous_sw == SWITCH_ON) {
        switch_off = 1;
    }

}


void
checkSwitch(void)
{
    current_sw = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7);

    if (current_sw != previous_sw) {
        if (switch_off == 1) {
            switch_off = 0;
        } else {
            switch_off = 1;
        }
        previous_sw = current_sw;
    }
}

