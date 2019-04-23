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

//*****************************************************************************
//  Global Variables
static uint8_t displayCount = 0;


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
    OLEDStringDraw ("Milestone 2", 0, 0);
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
}


//*****************************************************************************
//  ButtonCheck: Checks if the buttons are active.
//  Changes displayCount if UP is active
//  Resets the Reference Altitude and Yaw if the left button is pushed.
void ButtonCheck (void) {
    if(checkButton(LEFT) == PUSHED){        // If left button pushed, set the current Height as the minHeight
        resetAltitude();
        resetYaw();
    } if(checkButton(UP) == PUSHED){        // If up button pushed, increment count
        displayCount++;                            // Count goes from 0 to 3 and resets back to 0
    }
}

//*****************************************************************************
//  OutputToDisplay: Switches the display depending on the displayCount.
//  1st Screen: Percentage Altitude and Angle
//  2nd Screen: ADC Value and Yaw Angle
//  3rd Screen: Blank
void OutputToDisplay (void)
{
    ButtonCheck();
    switch(displayCount%3) {
        case 0:         // If count is changed to 0, display the Altitude in percentage
            introLine();
            printString("Altitude = %4d%%", percentAltitude(), 1);
            printString("Yaw Angle = %4d", getYaw(), 2);
            break;
        case 1:         // If count is changed to 1, display the raw ADC value
            introLine();
            printString("ADC Value = %4d", computeAltitude(), 1);
            printString("Yaw Angle = %4d", getYaw(), 2);
            break;
        case 2:         // If count is changed to 2, turn off display
            OrbitOledClear();
    }
}

