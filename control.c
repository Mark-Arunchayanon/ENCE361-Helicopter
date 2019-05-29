/*
 * control.c
 *
 * Includes PID control for the Altitude and Yaw, and 5 helicopter modes.
 * Landed, Initialising, TakeOff, Flying and Landing
 *
 * Created on: 14/05/2019
 * Author:  N. James
 *          L. Trenberth
 *          M. Arunchayanon
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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

#define ALT_REF_INIT        0    //Initial altitude reference
#define ALT_STEP_RATE       10   //Altitude step rate
#define ALT_MAX             100  //Maximum altitude
#define ALT_MIN             0    //Minimum altitude

#define YAW_REF_INIT        0    //Initial yaw reference
#define YAW_STEP_RATE       15   //Yaw step rate

#define ALT_PROP_CONTROL    0.7  //Altitude PID control
#define ALT_INT_CONTROL     0.2
#define ALT_DIF_CONTROL     0.2

#define YAW_PROP_CONTROL    0.4  //Yaw PID control
#define YAW_INT_CONTROL     0.1
#define YAW_DIF_CONTROL     0.5

#define DELTA_T             0.01 // 1/SYS_TICK_RATE

#define TAIL_OFFSET         30   //Tail offset
#define MAIN_OFFSET         40   //Main offset


//sets the intial value of the Altitude and
static int32_t AltRef =  ALT_REF_INIT;
static int32_t YawRef = YAW_REF_INIT;

//Sets integral error variables
static int32_t AltIntError = 0;
static int32_t AltPreviousError = 0;
static int32_t YawIntError = 0;
static int32_t YawPreviousError = 0;

//Yaw error and control variables
int32_t Yaw_error, YawDerivError;
uint32_t YawControl;

//Altitude error and control variables
int32_t Alt_error = 0, AltDerivError;
int32_t AltControl;

//Main and tail duty cycle
uint32_t mainDuty = 0, tailDuty = 0;

//Reading from PC4 to find reference
uint32_t PC4Read = 0;
uint32_t switchState = 0;
bool stable = false, paralysed = true, ref_Found = false;

// *******************************************************
// Declaring modes Landed, Initialising, TakeOff, Flying and Landing
// *******************************************************
typedef enum {Landed, Initialising, TakeOff, Flying, Landing} mode_type;

mode_type mode = Landed;  //Initial mode is landed

// *******************************************************
// Initialise and set up switch, PC4
// *******************************************************
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


// *******************************************************
// Reads the reset button, reset system if reset is pushed
// *******************************************************
void
updateReset(void)
{
    uint32_t reset = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_6);
    if(reset == 0) {
        SysCtlReset();
    }
}


// *******************************************************
// Reads the switch, if the program starts with the switch on,
// the helicopter will be paralysed (not be able to take of)
// *******************************************************
void
GetSwitchState(void)
{
    switchState = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7) / 128;
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_7);

    if((mode == Landed) && (switchState == 0) && paralysed) {
        paralysed = false;

    }

}

// *******************************************************
// Checks if the helicopter has taken off, sets stable to true
// *******************************************************
void
checkStability(void)
{
    if(percentAltitude() >= 30) {
        stable = true;
    }
}


// *******************************************************
// sets the altitude reference, takes parameter of the new altitude reference
// *******************************************************
void setAltRef(int32_t newAltRef)
{
    AltRef = newAltRef;
}


// *******************************************************
// sets the yaw reference, takes a parameter of the new yaw reference
// *******************************************************
void setYawRef(int32_t newYawRef)
{
    YawRef = newYawRef;
}


// *******************************************************
// Take off, checks if yaw is at zero before setting altitude to 50%
// *******************************************************
void take_Off(void)
{
    if (getYaw() == 0) {
        setAltRef(50);
    }
}

// *******************************************************
// Turns on main and tail motor to spin the helicopter clockwise
// constantly reads PC4 to check if the helicopter is at the reference
// Once the reference is found, set reset yaw to be zero and set yaw reference to 0
// *******************************************************
void findYawRef(void)
{
    SetMainPWM(25);
    SetTailPWM(30);

    PC4Read = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
    if(PC4Read < 16) {
        ref_Found = true;
        //reset the yaw as we have found the origin and set the reference
        resetYaw();
        setYawRef(0);
    }
}


// *******************************************************
// Once yaw is at 0 degrees, give or take 5, decrease altitude by 5% if over 10%
// If altitude is under 10%, shut off motors
// *******************************************************
void landing(void)
{
    uint32_t alt = AltRef;
    if ((getYaw() <= 5) && (getYaw() >= -5)) {
        if (mode == Landing) {
            if (percentAltitude() >= 10) {
                if (AltRef <= 0) {
                    setAltRef(0);
                } else {
                    setAltRef(alt - 5);
                }
            } else {
                SetMainPWM(0);
                SetTailPWM(0);
            }
        }
    }
//    if (mode == Landing && yaw_at0 == true) {
//
//        if (percentAltitude() >= 15) {
//            if (AltRef <>
//            setAltRef(alt - 5);
//        } else {
//            SetMainPWM(0);
//            SetTailPWM(0);
//        }
//    }
//    main_offset = 0;
//    setYawRef(0);
//    if (getYaw()  -5) && (getYaw() <5)) {
//        setAltRef(12);
//    }

}


// *******************************************************
// Yaw PID control
// Constantly gets called, only runs during takeoff, flying and landing mode
// *******************************************************
void PIDControlYaw(void)
{
    if( (mode == TakeOff) || (mode == Flying) || (mode == Landing)) {

        Yaw_error = YawRef - getYaw();  // Calculates the yaw error

        YawIntError += Yaw_error * DELTA_T;  //Integral error
        YawDerivError  = Yaw_error-YawPreviousError;  //Derivative error

        YawControl = Yaw_error * YAW_PROP_CONTROL      //yaw control based on PID terms
                    + YawIntError * YAW_INT_CONTROL
                    + YawDerivError * YAW_DIF_CONTROL
                    + TAIL_OFFSET;

        if (YawControl > 85) {  //Maximum is 85%
            YawControl -= 25;
        }
        SetTailPWM(YawControl);  //Sets the tail duty cycle
        YawPreviousError = Yaw_error;
        tailDuty = YawControl;
    }
}


// *******************************************************
// Altitude PID control
// Constantly gets called, only runs during takeoff, flying and landing mode
// *******************************************************
void PIDControlAlt(void)
{
    if ((mode == TakeOff) || (mode == Flying) || (mode == Landing)) {

        Alt_error = AltRef - percentAltitude();  //Calculates altitude error

        AltIntError += Alt_error * DELTA_T;  //Integral error
        AltDerivError = (Alt_error-AltPreviousError) * 100;  //Derivative error

        AltControl = Alt_error * ALT_PROP_CONTROL  //Altitude control based on the PID terms
                    + AltIntError * ALT_INT_CONTROL
                    + AltDerivError * ALT_DIF_CONTROL
                    + MAIN_OFFSET;

        if (AltControl > 85) {  //Maximum duty cycle of 85%
            AltControl -= 25;
        }
        SetMainPWM(AltControl);  //Sets the main duty cycle
        AltPreviousError = Alt_error;
        mainDuty = AltControl;
    }
}


// *******************************************************
// Returns main duty cycle
// *******************************************************
uint32_t getMainDuty(void)
{
    return mainDuty;
}


// *******************************************************
// Returns tail duty cycle
// *******************************************************
uint32_t getTailDuty(void)
{
    return tailDuty;
}



// *******************************************************
// Returns main duty cycle
// *******************************************************
char* getMode(void)
{
    switch(mode)
    {
    case Landed: return "Landed";
    case Initialising: return "Initialising";
    case TakeOff:  return "TakeOff";
    case Flying: return"Flying";
    case Landing: return "Landing";
    }

   return NULL;
}


// *******************************************************
// Reset all error and integral error to 0
// *******************************************************
void
resetIntControl(void)
{
    Alt_error = 0;
    AltIntError = 0;
    AltPreviousError = 0;
    Yaw_error = 0;
    YawIntError = 0;
    YawPreviousError = 0;
}


// *******************************************************
// Only runs when the helicopter is in flying mode
// Checks if any of the buttons have been pushed
// UP and DOWN are used to increase/decrease altitude reference
// LEFT and RIGHT are used to increase/decrease yaw reference
// *******************************************************
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

    }
}


// *******************************************************
// Switches between the 5 modes
// *******************************************************
void helicopterStates(void){

    switch(mode) {
    case Landed:

        if (switchState == 1 && !paralysed) {  //If switch is flicked on and the helicopter is not paralysed
            mode = Initialising;               // Change mode to Initialising
            resetIntControl();                 //Reset any previous error terms
        }
        break;

    case Initialising:

        findYawRef();                          //Spins clockwise until the reference point is found
        if(ref_Found) {
            mode = TakeOff;                    //Change mode to takeoff once the reference point is found
        }
        break;

    case TakeOff:

        take_Off();                            //Set yaw to 0 and raises the helicopter up to 50% altitude
        checkStability();
        if(stable) {
            mode = Flying;                     //Once the reference point is met and the correct altitude is reached set the state to flying
        }
        break;

    case Flying:

        RefUpdate();                           //Checks if for button pushes and alter references

        if(switchState == 0) {                 //If the switch is flicked down then begin the landing process
            mode = Landing;
            setYawRef(0);                      //Set yaw reference to 0
        }
        break;

    case Landing:

        if (percentAltitude() == 0) {          //If altitude is at 0, change mode to landed
            mode = Landed;
        }
        break;
    }
}
