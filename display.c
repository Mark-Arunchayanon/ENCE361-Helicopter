//*****************************************************************************
//
//  Display - Display related functions
//
//  Author:  N. James
//           L. Trenberth
//           M. Arunchayanon
//     Last modified:   20.4.2019
//*****************************************************************************
#define MAX_STR_LEN 16

#include "system.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"
#include "altitude.h"
#include "yaw.h"
#include "control.h"


int8_t button = 0;

//*****************************************************************************
//  initDisplay: Initialises Display using OrbitLED functions
void
initDisplay (void)
{
    // initialise the Orbit OLED display
    OLEDInitialise ();
}

//*****************************************************************************
//  introLine: Prints the intro line on the OLED Display
void introLine (void)
{
    OLEDStringDraw ("Heli Project", 0, 0);
}

//*****************************************************************************
//  printString: Prints the input format and line contents on the given line number on the OLED Display
//  INTPUTS: line_format - The format to print the string in, including a integer placeholder
//  INTPUTS: line_contents - The integer to print on the line
//  INTPUTS: line_number - The line number integer to print the string on.
void printString(char* restrict line_format, int32_t line_contents, int line_number)
{
    char string[MAX_STR_LEN + 1];
    usnprintf (string, sizeof(string), line_format, line_contents);
    OLEDStringDraw (string, 0, line_number); // Update line on display.
}



//*****************************************************************************
//  initButtonCheck: Initialises left and up buttons on the micro-controller
void initButtonCheck (void) {
    SysCtlPeripheralReset (LEFT_BUT_PERIPH);//setting up the LEFT button GPIO
    SysCtlPeripheralReset (UP_BUT_PERIPH);//setting the UP button GPIO
    SysCtlPeripheralReset (DOWN_BUT_PERIPH);//setting the DOWN button GPIO
    SysCtlPeripheralReset (RIGHT_BUT_PERIPH);//setting the RIGHT button GPIO

}
//*****************************************************************************
//  OutputToDisplay: Switches the display depending on the displayCount.
//  1st Screen: Percentage Altitude and Angle
//  2nd Screen: ADC Value and Yaw Angle
//  3rd Screen: Blank
void OutputToDisplay (void)
{
    printString("Altitude = %4d%%", percentAltitude(), 0);
    printString("Yaw Angle = %4d", getYaw(), 1);
    printString("Alt Ref = %4d", GetAltRef(), 2);
    printString("Yaw Ref = %4d", GetYawRef(), 3);
}

