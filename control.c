/*
 * control.c
 *
 *  Created on: 14/05/2019
 *      Author: par116
 */

#include <stdint.h>
#include <stdbool.h>

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

#define ALT_PROP_CONTROL    2.5
#define ALT_INT_CONTROL     0.1
#define ALT_DIF_CONTROL     0.5
#define YAW_PROP_CONTROL    2
#define YAW_INT_CONTROL     0.1
#define YAW_DIF_CONTROL     0.3
#define DELTA_T             0.01 // 1/SYS_TICK_RATE

#define TAIL_OFFSET         30
#define MAIN_OFFSET         60


//sets the intial value of the Altitude and
static int32_t AltRef =  ALT_REF_INIT;
static int32_t YawRef = YAW_REF_INIT;


static int32_t AltIntError = 0;
static int32_t AltPreviousError = 0;
static int32_t YawIntError = 0;
static int32_t YawPreviousError = 0;



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
    int32_t error, YawDerivError;
    uint32_t YawControl;

    error = YawError();

    YawIntError += error * DELTA_T;
    YawDerivError  = error-YawPreviousError;

    YawControl = error * YAW_PROP_CONTROL + YawIntError * YAW_INT_CONTROL + YawDerivError * YAW_DIF_CONTROL;
    SetTailPWM(YawControl + TAIL_OFFSET);
    YawPreviousError = error;
}

//Controls the
void PIDControlAlt(void)
{
    int32_t error = AltError();
    int32_t AltControl;

    AltIntError += error * DELTA_T;
    int32_t AltDerivError = error-AltPreviousError;

    AltControl = error * ALT_PROP_CONTROL + AltIntError * ALT_INT_CONTROL + AltDerivError * ALT_DIF_CONTROL;
    SetMainPWM(AltControl + MAIN_OFFSET);
    AltPreviousError = error;
}

void
RefUpdate(void)
{
    //Will change a lot
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

