/*
 * motor.h
 *
 *  Created on: 13/05/2019
 *      Author: ltr28
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include <stdbool.h>

void
initialiseMainPWM (void);

void
setMainPWM (uint32_t ui32MainDuty);

void
setTailPWM (uint32_t ui32TailDuty);

void
initialiseTailPWM (void);

void
initmotor(void);

void
updatePWM(void);


#endif /* MOTOR_H_ */
