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
#include "yaw.h"
#include "altitude.h"

#define SWITCH_ON       128
#define INIT_ALTITUDE   10





int32_t switchState;
mode_type mode;
bool execute_flying = true;

// Set up switch 1
void
initSwitch(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA,
           GPIO_PIN_TYPE_STD_WPD);
}

uint32_t
GetSwitchState(void)
{
    uint32_t switchVal = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7);
    return switchVal;
}


//
void
initStates(void)
{
    //definitions to be used in the function
    mode = Landed;
//    bool firstCheck = true;
//    bool init_error = true;
//
//    //run a while loop until we are ready to start flying
//    while(!execute_flying) {
//        //read the switch and assign it to a variable
//        switchState = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7);
//        // if the switch state is on, and it is the first check
//        //set to an error state else if it's not in an error state start flying
//        if (switchState == SWITCH_ON) {
//            if(firstCheck) {
//                init_error = true;
//            } else if(!init_error) {
//                execute_flying = true;
//            }
//        }
//      //set the error state to being false as the switch is off
//      else {
//          init_error = false;
//      }
//      firstCheck = false;
//    }


    if (GetSwitchState() > 0) {
        //find the reference position once again
        findYawReference();
        //
        setAltRef(INIT_ALTITUDE);
        PIDControlAlt();
        mode = Flying;
    }

//    //find the reference position once again
//    findYawReference();
//    //
//    setAltRef(INIT_ALTITUDE);
//    PIDControlAlt();
//    mode = Flying;
}


void flying(void)
{
    if(mode == Flying) {
        RefUpdate();
    }

}


void initLanding(void)
{
    int32_t alt_percent = 10;
    //check the switch
    //rotate back to origin
    returntoReference();
    //go to zero altitude
    setAltRef(alt_percent);
    PIDControlAlt();
    //call landed to switch motors off
    for (alt_percent = 10; alt_percent <= 1; alt_percent--)
    {
    //while(percentAltitude() < 1) {
        setAltRef(alt_percent);
        alt_percent--;
    }
    SetMainPWM(0);
    SetTailPWM(0);
    mode = Landed;

}


//
void
checkSwitch(void)
{
    int32_t current_sw = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7);

//    if (current_sw != previous_sw) {
//        if (switch_off == 1) {
//            switch_off = 0;
//        } else {
//            switch_off = 1;
//        }
//        previous_sw = current_sw;
//    }
    if (current_sw > 0 && mode == Landed) {
        initStates();
    }
    else if (current_sw == 0 && mode == Flying) {
        initLanding();
    }
}

