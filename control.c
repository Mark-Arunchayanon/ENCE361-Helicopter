/*
 * control.c
 *
 *  Created on: 14/05/2019
 *      Author: par116
 */


#include "altitude.h"
#include "yaw.h"


#define ALT_REF_INIT    0
#define YAW_REF_INIT    0
#define PROP_CONTROL    0
#define INT_CONTROL     0

static AltRef = ALT_REF_INIT
static YawRef = YAW_REF_INIT

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

int PIDControlYaw(void)
{
    int error = YawError();
    int control;

}


