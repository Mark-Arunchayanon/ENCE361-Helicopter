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

#define ALT_REF_INIT        10
#define ALT_STEP_RATE       10
#define ALT_MAX             100
#define ALT_MIN             0

#define YAW_REF_INIT        0
#define YAW_STEP_RATE       15

#define ALT_PROP_CONTROL    1.5
#define ALT_INT_CONTROL     0.2
#define ALT_DIF_CONTROL     0.5
#define YAW_PROP_CONTROL    0.5
#define YAW_INT_CONTROL     0.2
#define YAW_DIF_CONTROL     0.9
#define DELTA_T             0.01 // 1/SYS_TICK_RATE

#define TAIL_OFFSET         30
#define MAIN_OFFSET         60


static int32_t AltRef =  ALT_REF_INIT;
static int32_t YawRef = YAW_REF_INIT;
static int32_t AltIntError = 0;
static int32_t AltPreviousError = 0;
static int32_t YawIntError = 0;
static int32_t YawPreviousError = 0;


void setAltRef(int32_t newAltRef)
{
    AltRef = newAltRef;
}



void setYawRef(int32_t newYawRef)
{
    YawRef = newYawRef;
}



int32_t AltError (void)
{
    return AltRef - percentAltitude();
}

int32_t YawError(void)
{
    return  YawRef - getYaw();
}



void PIDControlYaw(void)
{
    int32_t error = YawError();
    uint32_t YawControl;

    YawIntError += error * DELTA_T;
    int32_t YawDerivError = error-YawPreviousError;

    YawControl = error * YAW_PROP_CONTROL + YawIntError * YAW_INT_CONTROL + YawDerivError * YAW_DIF_CONTROL;
    SetTailPWM(YawControl + TAIL_OFFSET);
    YawPreviousError = error;
}

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
    if (checkButton (LEFT) == PUSHED)
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

