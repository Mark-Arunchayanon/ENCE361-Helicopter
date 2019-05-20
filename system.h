#ifndef SYSTEM_H_
#define SYSTEM_H_

//*****************************************************************************
//
// System - System Related Files and includes
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   20.4.2019
//*****************************************************************************

//*****************************************************************************
// System Constants and Includes
//*****************************************************************************

#define SAMPLE_RATE_HZ   40
#define SYSTICK_RATE_HZ  100

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "buttons4.h"

//#include "driverlib/pwm.h"

static uint32_t g_ulSampCnt;    // Counter for the interrupts
extern volatile uint8_t slowTick;
//*****************************************************************************
// initClock: The interrupt handler for the for SysTick interrupt.
// Taken from Week4Lab ADCDemo1
void
SysTickIntHandler(void);

//*****************************************************************************
//  initSysTick: Initialises system ticks for the system clock
//  and sets SysTickIntHandler to handle interrupts from the clock
//  Taken from Week4Lab ADCDemo1
void
initSysTick (void);

//*****************************************************************************
// initClock: Initialisation functions for the clock (incl. SysTick), ADC, display
// Taken from Week4Lab ADCDemo1
void
initClock (void);

#endif /*SYSTEM_H_*/
