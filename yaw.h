#ifndef YAW_H_
#define YAW_H_


//*****************************************************************************
//
// Yaw - Calculating yaw slot numbers and angles functions through an Interrupt
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   23.4.2019
//*****************************************************************************

#include "system.h"

// *******************************************************
// getYaw: Uses the current slot number on the disk to
// return an angle in degrees from the original reference point.
// RETURNS a angle value between -180 < Yaw < 180 degrees.
int32_t
getYaw (void);

// *******************************************************
// resetYaw: Resets the reference point for slot number.
void
resetYaw (void);


// *******************************************************
//  YawIntHandler: Interrupt handler for the yaw interrupt.
//  Measures PhaseB. If PhaseB generated the interrupt, add 1 to slot.
//  If PhaseA generated the interrupt, minus 1 from slot.
void
YawIntHandler (void);

// *******************************************************
//  YawIntHandler: Interrupt initialisation for the yaw interrupt.
//  Sets PB0 and PB1 to be inputs, enables interrupts on GPIOB.
//  An interrupt occurs on both edges of PB0 and PB1 and when triggered,
//  runs the YawIntHandler function
void
initYaw(void);


#endif /* YAW_H_*/
