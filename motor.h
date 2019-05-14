/*
 * motor.h
 *
 *  Created on: 13/05/2019
 *      Author: ltr28
 */

#ifndef MOTOR_H_
#define MOTOR_H_

void
SetMainPWM (uint32_t ui32Duty);

/*********************************************************
 * initialiseMainPWM
 * M0PWM7 (J4-05, PC5) is used for the main rotor motor
 *********************************************************/

void
initialiseMainPWM (void);


/********************************************************
 * Function to set the freq, duty cycle of M1PWM5
 ********************************************************/
void
SetTailPWM (uint32_t ui32Duty);


/*********************************************************
 * initialiseTailPWM
 * M1PWM5 (J3-10, PF1) is used for the secondary rotor motor
 *********************************************************/
void
initialiseTailPWM (void);

void
initmotor(void);

void
changeMainMotor(int change);

void
changeSecMotor(int change);


#endif /* MOTOR_H_ */
