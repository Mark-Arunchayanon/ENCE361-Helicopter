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

//*****************************************************************************
// Global Variables
//*****************************************************************************
static uint8_t count = 0;

//*****************************************************************************
//
// Initialise Display
//
//*****************************************************************************
void
initDisplay (void)
{
    // initialise the Orbit OLED display
    OLEDInitialise ();
}

void introLine (void)
{
    OLEDStringDraw ("Milestone 2", 0, 0);
}

void printString(char* restrict line_format, int32_t line_contents, int line_number)
{
    const char string[17];
    usnprintf (string, sizeof(string), line_format, line_contents);
    OLEDStringDraw (string, 0, line_number); // Update line on display.
}

void OutputToDisplay (void)
{
    if(checkButton(LEFT) == PUSHED){        // If left button pushed, set the current Height as the minHeight
        resetAltitude();
    } if(checkButton(UP) == PUSHED){        // If up button pushed, increment count
        count++;                            // Count goes from 0 to 3 and resets back to 0
    }
    switch(count%3) {
        case 0:         // If count is changed to 0, display the Altitude in percentage
            introLine();
            printString("Height = %4d%%", percentAltitude(), 1);
            break;
        case 1:         // If count is changed to 1, display the raw ADC value
            introLine();
            printString("ADC Value = %4d", computeAltitude(), 1);
            break;
        case 2:         // If count is changed to 2, turn off display
            OrbitOledClear();
    }
}

