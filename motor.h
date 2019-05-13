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
initialisePWM (void);

void
setPWM (uint32_t ui32Freq, uint32_t ui32Duty);

void
setPWM2 (uint32_t ui32Freq2, uint32_t ui32Duty2);

void
initialisePWM2 (void);

void
initmotor(void);

void
updatePWM(void);


#endif /* MOTOR_H_ */
