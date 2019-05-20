/*
 * control.c
 *
 *  Created on: 14/05/2019
 *      Author: par116
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

#define ALT_REF_INIT        0
#define ALT_STEP_RATE       10
#define ALT_MAX             100
#define ALT_MIN             0

#define YAW_REF_INIT        0
#define YAW_STEP_RATE       15

#define ALT_PROP_CONTROL    0.7
#define ALT_INT_CONTROL     0.2
#define ALT_DIF_CONTROL     0.3
#define YAW_PROP_CONTROL    0.9
#define YAW_INT_CONTROL     0.3
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

int32_t Yaw_error, YawDerivError;
uint32_t YawControl;

int32_t Alt_error = 0, AltDerivError;
int32_t AltControl;

uint32_t mainDuty = 0, tailDuty = 0;
uint32_t PC4Read = 0;
uint32_t switchState = 0;
bool stable = false, paralysed = true, ref_Found = false;
uint32_t main_offset = MAIN_OFFSET;

typedef enum {Landed, Initialising, TakeOff, Flying, Landing,} mode_type;
//landed, orientation

mode_type mode = Landed;



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


    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    //SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    SetMainPWM (PWM_MAIN_START_DUTY);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
}

//void
//updateReset(void)
//{
//    uint32_t reset = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_6);
//    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_6);
//    if(reset == 0) {
//        SysCtlReset();
//    }
//}

void
GetSwitchState(void)
{
    switchState = GPIOPinRead (GPIO_PORTA_BASE, GPIO_PIN_7) / 128;
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_7);

    if((mode == Landed) && (switchState == 0) && paralysed) {
        paralysed = false;

    }

}


void
checkStability(void)
{
    if(percentAltitude() >= 5) {
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
//    SetMainPWM(60);
//    SetTailPWM(50);
}

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
    }
}


void landing(void)
{
    main_offset = 0;
    setYawRef(0);
    setAltRef(14);
}


////computes the altitude error by taking the reference and subtracting the current altitude
////returns the error once the calculation has been made
//int32_t AltError (void)
//{
//    return AltRef - percentAltitude();
//}
//
////computes the yaw error by taking the reference and subtracting the current angle
////returns the error once the calculation has been made
//int32_t YawError(void)
//{
//    return  YawRef - getYaw();
//}

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
    if( (mode == TakeOff) || (mode == Flying) || (mode == Landing)) {


        Yaw_error = YawRef - getYaw();

        YawIntError += Yaw_error * DELTA_T;
        YawDerivError  = Yaw_error-YawPreviousError;

        YawControl = Yaw_error * YAW_PROP_CONTROL
                    + YawIntError * YAW_INT_CONTROL
                    + YawDerivError * YAW_DIF_CONTROL
                    + TAIL_OFFSET;

        if (YawControl > 85) {
            YawControl -= 25;
        }
        SetTailPWM(YawControl);
        YawPreviousError = Yaw_error;
        tailDuty = YawControl;
    }
}

//Controls the
void PIDControlAlt(void)
{
    if ((mode == TakeOff) || (mode == Flying) || (mode == Landing)) {
        Alt_error = AltRef - percentAltitude();


        AltIntError += Alt_error * DELTA_T;
        AltDerivError = (Alt_error-AltPreviousError) * 100;

        AltControl = Alt_error * ALT_PROP_CONTROL
                    + AltIntError * ALT_INT_CONTROL
                    + AltDerivError * ALT_DIF_CONTROL
                    + main_offset;
        if (AltControl > 85) {
            AltControl -= 25;
        }
        SetMainPWM(AltControl);
        AltPreviousError = Alt_error;
        mainDuty = AltControl;
    }
}

uint32_t getMainDuty(void)
{
    return mainDuty;
}

uint32_t getTailDuty(void)
{
    return tailDuty;
}

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


void helicopterStates(void){

    switch(mode) {
    case Landed:
        if (switchState == 1 && !paralysed) {
            //Once all the motors are off can set the state to being Initialising
            mode = Initialising;
            resetIntControl();
            main_offset = MAIN_OFFSET;
        }
        break;

    case Initialising:

        //wait here until button is slid up
//        bool foundRef = findYawRef();
        findYawRef();
        if(ref_Found) {
            mode = TakeOff;
        }
        break;

    case TakeOff:

        //once the reference point is met and the correct altitude is reached set the state to flying
        take_Off();
        checkStability();
        if(stable) {
            mode = Flying;
        }
        break;

    case Flying:
        RefUpdate();
        //if the switch is flicked down then begin the landing process
        if(switchState == 0) {
            mode = Landing;
        }
        break;

    case Landing:
        landing();
        //once it is back to the bottom then proceed to Landed where all the motors will turn off
        if(percentAltitude() < 15) {
            SetMainPWM(10);
            SetTailPWM(10);
        } else if (percentAltitude() < 8) {
            SetMainPWM(0);
            SetTailPWM(0);
            mode = Landed;
        }
        break;
    }
}
