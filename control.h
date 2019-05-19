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
helicopterStates(void);


// Set up switch 1
void
initSwitch_PC4(void);

void
updateReset(void);

bool
GetSwitchState(void);

bool
findYawRef(void);

void
take_Off(void);


void
landing(void);


void
checkStability(void);

//sets the altitude reference, takes parameter of the new altitude reference
void setAltRef(int32_t newAltRef);

//sets the yaw reference, takes a parameter of the new yaw reference
void setYawRef(int32_t newYawRef);


//computes the altitude error by taking the reference and subtracting the current altitude
//returns the error once the calculation has been made
int32_t AltError (void);


//computes the yaw error by taking the reference and subtracting the current angle
//returns the error once the calculation has been made
int32_t YawError(void);


//returns the current reference for the yaw
int32_t GetYawRef(void);

//returns the current reference for the altitude
int32_t
GetAltRef(void);


void
PIDControlYaw(void);


//Controls the
void PIDControlAlt(void);


void
resetIntControl(void);


void
RefUpdate(void);


#endif /* CONTROL_H_ */
