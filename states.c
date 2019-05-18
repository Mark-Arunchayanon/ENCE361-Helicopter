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


#include "control.h"
#include "motor.h"
#include "yaw.c"

#define SWITCH_ON 128;
#define FIND_REF 15; //value for finding the reference point
#define INIT_ALT 40;

typedef enum {Initialising, Flying, Landing, Landed} mode_type;

int32_t previous_sw;
int32_t current_sw;
bool switch_off = false;

// Set up switch 1
void
initSwitch(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA,
           GPIO_PIN_TYPE_STD_WPD);
}

//
void
initStates(void)
{
    int32_t ref, currentYaw;
    mode_type mode = Landed;
    bool refFound = false;

    previous_sw = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7);

    if (previous_sw == SWITCH_ON) {
        switch_off = true;
    }
    //start to make the helicopter spin in a circle until the initial zero point is reached
    //when the zero point is reached set the yaw reference
    changeMainMotor(FIND_REF);
    while(!refFound) {
        currentYaw = getYaw()
        if(currentYaw == 0){
            setYawRef(currentYaw);
            refFound = true;
        }
    }
    //check to see where the motor is at and execute the PID fucntion
    //to return to the reference
    PIDControlYaw();
    //set the Altitude Reference and let the PID Control to get it up to intial altitude
    setAltRef(INIT_ALT);
    PIDControlAlt();

}

//
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

