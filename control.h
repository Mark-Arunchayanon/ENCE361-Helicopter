/*
 * control.h
 *
 *  Created on: 14/05/2019
 *      Author: par116
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include <stdint.h>

void
setAltRef(int32_t newAltRef);

void
setYawRef(int32_t newYawRef);


int32_t
AltError (void);

int32_t
YawError(void);

int32_t
PIDControlYaw(void);

int32_t
PIDControlAlt(void);


void
RefUpdate(void);

#endif /* CONTROL_H_ */
