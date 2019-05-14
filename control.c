/*
 * control.c
 *
 *  Created on: 14/05/2019
 *      Author: par116
 */

#include <stdint.h>
#include "altitude.h"
#include "yaw.h"
#include "motor.h"


#define ALT_REF_INIT        0
#define ALT_STEP_RATE       10
#define ALT_MAX             100
#define ALT_MIN             0

#define YAW_REF_INIT        0
#define YAW_STEP_RATE       15

#define ALT_PROP_CONTROL    1
#define ALT_INT_CONTROL     0
#define ALT_DIF_CONTROL     0
#define YAW_PROP_CONTROL    1
#define YAW_INT_CONTROL     0
#define YAW_DIF_CONTROL     0
#define DELTA_T             1/500 // 1/SYS_TICK_RATE


static int32_t AltRef =  ALT_REF_INIT;
static int32_t YawRef = YAW_REF_INIT;



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
    return percentAltitude() - AltRef;
}

int32_t YawError(void)
{
    return getYaw() - YawRef;
}



void PIDControlYaw(void)
{
    int32_t error = YawError();
    int32_t YawControl;

    YawControl = error * YAW_PROP_CONTROL;
    SetTailPWM(YawControl);

}



void PIDControlAlt(void)
{
    int32_t error = AltError();
    int32_t AltControl;

    AltControl = error * ALT_PROP_CONTROL();
    SetMainPWM(AltControl);
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
        AltRef -= ALT_STEP_RATE
    }
    if (checkButton (LEFT) == PUSHED)
    {
        YawRef -= YAW_STEP_RATE;
    }
    if (checkButton (RIGHT) == PUSHED)
    {
        YawRef += YAW_STEP_RATE;
    }
}

