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

#ifndef CONTROL_H_
#define CONTROL_H_

#include <stdint.h>


void
helicopterStates(void);

// *******************************************************
// Initialise and set up switch, PC4
// *******************************************************
void
initSwitch_PC4(void);

// *******************************************************
// Reads the reset button, reset system if reset is pushed
// *******************************************************
void
updateReset(void);

// *******************************************************
// Reads the switch, if the program starts with the switch on,
// the helicopter will be paralysed (not be able to take of)
// *******************************************************
bool
GetSwitchState(void);

// *******************************************************
// Checks if the helicopter has taken off, sets stable to true
// *******************************************************
void
checkStability(void);

// *******************************************************
// sets the altitude reference, takes parameter of the new altitude reference
// *******************************************************
void
setAltRef(int32_t newAltRef);

// *******************************************************
// sets the yaw reference, takes a parameter of the new yaw reference
// *******************************************************
void
setYawRef(int32_t newYawRef);

// *******************************************************
// Take off, checks if yaw is at zero before setting altitude to 50%
// *******************************************************
void
take_Off(void);

// *******************************************************
// Turns on main and tail motor to spin the helicopter clockwise
// constantly reads PC4 to check if the helicopter is at the reference
// Once the reference is found, set reset yaw to be zero and set yaw reference to 0
// *******************************************************
bool
findYawRef(void);

// *******************************************************
// Once yaw is at 0 degrees, give or take 5, decrease altitude by 5% if over 10%
// If altitude is under 10%, shut off motors
// *******************************************************
void
landing(void);


// *******************************************************
// Returns main duty cycle
// *******************************************************
const char*
getMode(void);

// *******************************************************
// Returns main duty cycle
// *******************************************************
uint32_t
getMainDuty(void);

// *******************************************************
// Returns tail duty cycle
// *******************************************************
uint32_t
getTailDuty(void);

// *******************************************************
// Yaw PID control
// Constantly gets called, only runs during takeoff, flying and landing mode
// *******************************************************
void
PIDControlYaw(void);

// *******************************************************
// Altitude PID control
// Constantly gets called, only runs during takeoff, flying and landing mode
// *******************************************************
void PIDControlAlt(void);

// *******************************************************
// Reset all error and integral error to 0
// *******************************************************
void
resetIntControl(void);

// *******************************************************
// Only runs when the helicopter is in flying mode
// Checks if any of the buttons have been pushed
// UP and DOWN are used to increase/decrease altitude reference
// LEFT and RIGHT are used to increase/decrease yaw reference
// *******************************************************
void
RefUpdate(void);


#endif /* CONTROL_H_ */
