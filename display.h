#ifndef DISPLAY_H_
#define DISPLAY_H_

//*****************************************************************************
//
// Display - Display related functions
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   20.4.2019
//*****************************************************************************


#include "system.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "altitude.h"

#define MAX_STR_LEN 100

//*****************************************************************************
//  initDisplay: Initialises Display using OrbitLED functions
void
initDisplay (void);

//*****************************************************************************
//  introLine: Prints the intro line on the OLED Display
void
introLine (void);

//*****************************************************************************
//  printString: Prints the input format and line contents on the given line number on the OLED Display
//  INTPUTS: line_format - The format to print the string in, including a integer placeholder
//  INTPUTS: line_contents - The integer to print on the line
//  INTPUTS: line_number - The line number integer to print the string on.
void
printString(char* restrict line_format, int32_t line_contents, uint8_t line_number);

//*****************************************************************************
//  initButtonCheck: Initialises left and up buttons on the micro-controller
void
initButtonCheck (void);

//*****************************************************************************
//  ButtonCheck: Checks if the buttons are active.
//  Changes displayCount if UP is active
//  Resets the Reference Altitude and Yaw if the left button is pushed.


//*****************************************************************************
//  OutputToDisplay: Switches the display depending on the displayCount.
//  1st Screen: Percentage Altitude and Angle
//  2nd Screen: ADC Value and Yaw Angle
//  3rd Screen: Blank
void
OutputToDisplay (void);

#endif /*DISPLAY_H_*/
