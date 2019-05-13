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


#define ALT_REF_INIT    0
#define YAW_REF_INIT    0
#define PROP_CONTROL    0
#define INT_CONTROL     0

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

int32_t PIDControlYaw(void)
{
    int32_t error = YawError();
    int32_t YawControl;

    YawControl = YawError();
    return YawControl;

}

int32_t PIDControlAlt(void)
{
    int32_t error = AltError();
    int32_t AltControl;

    AltControl = AltError();
    return AltControl;

}

