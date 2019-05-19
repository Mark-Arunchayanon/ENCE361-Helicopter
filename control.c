/*
 * control.c
 *
 *  Created on: 14/05/2019
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

#include "system.h"
#include "altitude.h"
#include "display.h"
#include "yaw.h"
#include "motor.h"
#include "buttons4.h"

#define ALT_REF_INIT        10
#define ALT_STEP_RATE       10
#define ALT_MAX             100
#define ALT_MIN             0

#define YAW_REF_INIT        0
#define YAW_STEP_RATE       15

#define ALT_PROP_CONTROL    2
#define ALT_INT_CONTROL     0.1
#define ALT_DIF_CONTROL     0.8
#define YAW_PROP_CONTROL    1
#define YAW_INT_CONTROL     0.1
#define YAW_DIF_CONTROL     0.5
#define DELTA_T             0.01 // 1/SYS_TICK_RATE

#define TAIL_OFFSET         30
#define MAIN_OFFSET         20


//sets the intial value of the Altitude and
static int32_t AltRef =  ALT_REF_INIT;
static int32_t YawRef = YAW_REF_INIT;


static int32_t AltIntError = 0;
static int32_t AltPreviousError = 0;
static int32_t YawIntError = 0;
static int32_t YawPreviousError = 0;


uint32_t PC4Read = 0;
bool stable = false, paralysed = false;

typedef enum {TakeOff, Flying, Landing, Landed, Initialising} mode_type;
//landed, orientation

mode_type mode;



// Set up switch 1
void
initSwitch_PC4(void)
{
    // Initialise SW1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPadConfigSet (GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA,
           GPIO_PIN_TYPE_STD_WPD);

    // Initialise PC4 used to find yaw ref
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput (GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);

    //Initialise reset button
    GPIOPinTypeGPIOInput (GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_6, GPIO_DIR_MODE_IN);
}

void
updateReset(void)
{
    uint32_t reset = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_6);
    if(reset == 0) {
        SysCtlReset();
    }
}

bool GetSwitchState(void)
{
    uint32_t switchState = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7) / 128;
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_7);

    if(mode == Landed && switchState == 1 && paralysed == true) {
        paralysed = false;
    }

    if(switchState > 0) {
        return true;
    } else {
        return false;
    }
}



void
checkStability(void)
{
    if(percentAltitude() >= 45) {
        stable = true;
    }
}

//sets the altitude reference, takes parameter of the new altitude reference
void setAltRef(int32_t newAltRef)
{
    AltRef = newAltRef;
}


//sets the yaw reference, takes a parameter of the new yaw reference
void setYawRef(int32_t newYawRef)
{
    YawRef = newYawRef;
}


void take_Off(void)
{
    setAltRef(50);
    setYawRef(0);

}

bool findYawRef(void)
{
    bool ref_Found = false;
    SetMainPWM(25);
    SetTailPWM(35);

    PC4Read = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
    if(PC4Read < 15) {
        ref_Found = true;
        //reset the yaw as we have found the origin and set the reference
        resetYaw();
        setYawRef(0);
    }
    return ref_Found;
}


void landing(void)
{
    setYawRef(0);
    SetMainPWM(35);
}


//computes the altitude error by taking the reference and subtracting the current altitude
//returns the error once the calculation has been made
int32_t AltError (void)
{
    return AltRef - percentAltitude();
}

//computes the yaw error by taking the reference and subtracting the current angle
//returns the error once the calculation has been made
int32_t YawError(void)
{
    return  YawRef - getYaw();
}

//returns the current reference for the yaw
int32_t GetYawRef(void)
{
    return YawRef;
}

//returns the current reference for the altitude
int32_t GetAltRef(void)
{
    return AltRef;
}


void PIDControlYaw(void)
{
    if( mode == TakeOff || mode == Flying || mode == Landing) {
        int32_t error, YawDerivError;
        uint32_t YawControl;

        error = YawError();

        YawIntError += error * DELTA_T;
        YawDerivError  = error-YawPreviousError;

        YawControl = error * YAW_PROP_CONTROL + YawIntError * YAW_INT_CONTROL + YawDerivError * YAW_DIF_CONTROL;
        SetTailPWM(YawControl + TAIL_OFFSET);
        YawPreviousError = error;
    }
}

//Controls the
void PIDControlAlt(void)
{
    if( mode == TakeOff || mode == Flying || mode == Landing) {
        int32_t error = AltError();
        int32_t AltControl;

        AltIntError += error * DELTA_T;
        int32_t AltDerivError = error-AltPreviousError;

        AltControl = error * ALT_PROP_CONTROL + AltIntError * ALT_INT_CONTROL + AltDerivError * ALT_DIF_CONTROL;
        SetMainPWM(AltControl + MAIN_OFFSET);
        AltPreviousError = error;
    }
}


void
resetIntControl(void)
{
    AltIntError = 0;
    AltPreviousError = 0;
    YawIntError = 0;
    YawPreviousError = 0;
}

void
RefUpdate(void)
{
    if(mode == Flying) {
        if ((checkButton (UP) == PUSHED) && (AltRef < ALT_MAX))
        {
            AltRef += ALT_STEP_RATE;
        }
        if ((checkButton (DOWN) == PUSHED) && (AltRef > ALT_MIN))
        {
            AltRef -= ALT_STEP_RATE;
        }
        if (checkButton (LEFT) == PUSHED )
        {
            YawRef -= YAW_STEP_RATE;
        }
        if (checkButton (RIGHT) == PUSHED)
        {
            YawRef += YAW_STEP_RATE;
        }
        PIDControlAlt();
        PIDControlYaw();
    }
}


void helicopterStates(void){
    bool switchUp;

    switch(mode) {
    case Landed:
       switchUp = GetSwitchState();
        if (switchUp == true && !paralysed) {
            //Once all the motors are off can set the state to being Initialising
            mode = Initialising;
            resetIntControl();
            paralysed = true;
        }
        break;

    case Initialising:

        //wait here until button is slid up
//        bool foundRef = findYawRef();
        if(findYawRef() == true) {
            mode = TakeOff;
        }
        break;

    case TakeOff:

        //once the reference point is met and the correct altitude is reached set the state to flying
        take_Off();
        if(stable) {
            mode = Flying;
        }
        break;

    case Flying:
        RefUpdate();
        switchUp = GetSwitchState();
        //if the switch is flicked down then begin the landing process
        if(!switchUp) {
            mode = Landing;
        }
        break;

    case Landing:
        landing();
        //once it is back to the bottom then proceed to Landed where all the motors will turn off
        if(percentAltitude() < 3) {
            mode = Landed;
            paralysed = false;
        }
        break;
    }
}
